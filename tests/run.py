import argparse
import csv
import heapq
import itertools
from pathlib import Path
import random
import string
import subprocess
import sys
import tempfile


ROOT = Path(__file__).resolve().parents[1]
SYMBOLS = string.ascii_letters + string.digits + "_"
FLIP_BITS = str.maketrans("01", "10")
METHODS = tuple(itertools.product(
    ("heap", "queue"), ("array", "tree", "sort", "pairwise")
))


def huffman_codes(weights):
    # Store each subtree as a dictionary of leaf codes relative to its root.
    serial = itertools.count()
    heap = [(weight, next(serial), {symbol: ""})
            for symbol, weight in weights.items()]
    heapq.heapify(heap)
    while len(heap) > 1:
        left_weight, _, left = heapq.heappop(heap)
        right_weight, _, right = heapq.heappop(heap)
        merged = {symbol: "0" + code for symbol, code in left.items()}
        merged.update({symbol: "1" + code for symbol, code in right.items()})
        heapq.heappush(heap, (left_weight + right_weight, next(serial), merged))
    return heap[0][2]


def verdict(weights, rows, optimum):
    by_symbol = dict(rows)
    if len(by_symbol) != len(rows) or by_symbol.keys() != weights.keys():
        return "No"
    if sum(weights[c] * len(code) for c, code in rows) != optimum:
        return "No"
    codes = set(by_symbol.values())
    if len(codes) != len(rows):
        return "No"
    for code in codes:
        if any(code[:length] in codes for length in range(1, len(code))):
            return "No"
    return "Yes"


def make_case(rng, n, m, mixed, max_weight=1000):
    weights = {c: rng.randint(1, max_weight) for c in rng.sample(SYMBOLS, n)}
    original = huffman_codes(weights)
    optimum = sum(weights[c] * len(code) for c, code in original.items())
    lines = [str(n), " ".join(f"{c} {w}" for c, w in weights.items()), str(m)]
    expected = []
    for i in range(m):
        codes = original.copy()
        if rng.getrandbits(1):
            codes = {c: code.translate(FLIP_BITS)
                     for c, code in codes.items()}
        variant = i % 5 if mixed else 0
        if variant == 1:  # Keep lengths and cost, but create a prefix conflict.
            codes = {c: "0" * len(code) for c, code in codes.items()}
        elif variant == 2:  # Keep prefix validity, but increase the cost.
            c = rng.choice(tuple(codes))
            codes[c] += "0"
        elif variant == 3:  # Swap the actual codes of two symbols.
            a, b = rng.sample(tuple(codes), 2)
            codes[a], codes[b] = codes[b], codes[a]
        elif variant == 4:
            for c in codes:
                length = rng.randint(1, 63)
                codes[c] = format(rng.getrandbits(length), f"0{length}b")
        rows = list(codes.items())
        rng.shuffle(rows)
        lines.extend(f"{c} {code}" for c, code in rows)
        expected.append(verdict(weights, rows, optimum))
    return "\n".join(lines) + "\n", expected


def run(exe, methods, data, expected, label, repeat=None):
    args = [str(exe), *methods]
    if repeat is not None:
        args.append(str(repeat))
    result = subprocess.run(args, input=data, text=True, capture_output=True, check=True)
    actual = result.stdout.splitlines()
    for i, (got, want) in enumerate(itertools.zip_longest(actual, expected), 1):
        if got != want:
            raise SystemExit(
                f"{label} / {' + '.join(methods)} / submission {i}: "
                f"expected {want!r}, got {got!r}"
            )
    return result.stderr


def check(exe, args):
    manual = sorted((ROOT / "tests" / "cases").glob("*.in"))
    total = 0
    for path in manual:
        data = path.read_text(encoding="ascii")
        expected = path.with_suffix(".out").read_text(encoding="ascii").splitlines()
        for methods in METHODS:
            run(exe, methods, data, expected, path.name)
        total += len(expected)

    rng = random.Random(args.seed)
    data, expected = make_case(rng, 63, 1000, mixed=True, max_weight=3)
    for methods in METHODS:
        run(exe, methods, data, expected, f"max-size case (seed={args.seed})")
    total += len(expected)

    for i in range(args.cases):
        n = rng.randint(2, args.symbols)
        max_weight = (1, 3, 1000)[i % 3]
        data, expected = make_case(rng, n, args.submissions, mixed=True,
                                   max_weight=max_weight)
        for methods in METHODS:
            run(exe, methods, data, expected, f"random case {i + 1} (seed={args.seed}, n={n})")
        total += len(expected)
    print(f"PASS: {len(manual)} manual files + {args.cases + 1} generated inputs, "
          f"{total} submissions per combination, {len(METHODS)} combinations "
          f"(seed={args.seed})")


def bench(exe, args):
    data, expected = make_case(random.Random(args.seed), args.symbols,
                               args.submissions, mixed=args.mixed)
    writer = csv.writer(sys.stdout, lineterminator="\n")
    writer.writerow(("n", "m", "seed", "workload", "optimal", "prefix",
                     "optimal_calls", "optimal_us_per_call",
                     "prefix_calls", "prefix_us_per_call"))
    for methods in METHODS:
        timing = run(exe, methods, data, expected, "benchmark", args.repeat)
        optimal_calls, optimal_ns, prefix_calls, prefix_ns = map(int, timing.split())
        writer.writerow((args.symbols, args.submissions, args.seed,
                         "mixed" if args.mixed else "valid", *methods,
                         optimal_calls, f"{optimal_ns / optimal_calls / 1000:.3f}",
                         prefix_calls, f"{prefix_ns / prefix_calls / 1000:.3f}"))


def main():
    parser = argparse.ArgumentParser(description="Check or benchmark the Huffman judge.")
    parser.add_argument("--cc", default="gcc", help="GCC or Clang executable")
    commands = parser.add_subparsers(dest="command", required=True)
    checks = commands.add_parser("check")
    checks.add_argument("--cases", type=int, default=100)
    benchmark = commands.add_parser("bench")
    benchmark.add_argument("--repeat", type=int, default=100)
    benchmark.add_argument("--mixed", action="store_true")
    for command in (checks, benchmark):
        command.add_argument("--symbols", type=int, default=63)
        command.add_argument("--submissions", type=int, default=100)
        command.add_argument("--seed", type=int, default=42)
    benchmark.set_defaults(submissions=1000)

    args = parser.parse_args()
    if not (2 <= args.symbols <= 63 and 1 <= args.submissions <= 1000):
        parser.error("symbols must be in 2..63 and submissions in 1..1000")
    if args.command == "check" and args.cases < 0:
        parser.error("cases must be non-negative")
    if args.command == "bench" and args.repeat < 1:
        parser.error("repeat must be positive")

    src = ROOT / "src"
    with tempfile.TemporaryDirectory(prefix="huffman-") as build:
        exe = Path(build) / "huffman.exe"
        sources = sorted(str(path) for path in src.rglob("*.c"))
        subprocess.run([args.cc, "-std=c17", "-O2", "-Wall", "-Wextra",
                        "-I", str(src), *sources, "-o", str(exe)], check=True)
        if args.command == "check":
            check(exe, args)
        else:
            bench(exe, args)


if __name__ == "__main__":
    main()
