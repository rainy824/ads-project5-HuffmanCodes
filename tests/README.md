# Huffman 判题测试

在项目根目录运行。脚本需要 Python 3 和 GCC 或 Clang，只用 Python 标准库。每次在临时目录用 `-O2` 编译，结束后清理。

```powershell
python tests/run.py check
python tests/run.py bench --symbols 63 --submissions 1000 --repeat 100 --seed 42
python tests/run.py bench --symbols 16 --submissions 100 --repeat 500 --seed 42 --mixed
```

编译器未加入 PATH 时，在子命令前指定，例如 `python tests/run.py --cc C:/msys64/ucrt64/bin/gcc.exe check`。Python 未加入 PATH 时，可以用 `C:/Python314/python.exe`。

## 正确性测试

`cases/` 的 `.in` 和 `.out` 分别是手写输入和预期输出。覆盖打乱行序、重复或未知字符、两种插入顺序的前缀冲突、重复码、不同的最优编码、合法但非最优的编码、最少字符数、最高频率和 63 位码字输入。

`check` 将每个用例跑遍 8 种算法组合。随机用例由 Python 的 `heapq` 构造 Huffman 树来计算最佳加权长度，用集合逐个检查实际码字的前缀。每份提交都打乱行序，并混入正例、等长前缀冲突、非最优前缀码和随机码。每次额外跑一组 `N=63, M=1000`。用 `--seed` 固定种子，`--cases` 指定随机输入文件数，`--symbols` 指定随机字符数上限，`--submissions` 指定每份随机输入的提交数。

## 性能测试

`bench` 将同一份输入交给 8 种组合，先核对每种组合的输出，再输出 CSV。可以用 `--symbols`、`--submissions` 和 `--seed` 改变规模和数据。默认使用全部通过的提交。`--mixed` 使用含前缀冲突和非最优编码的混合输入。

CSV 分别列出最优长度算法与前缀算法的调用次数和平均每次耗时（微秒）。计时测量 C 程序内部的算法调用，包含前缀实现中的 Kraft 检查。输入输出、字符查表及提交加权长度计算在计时外。只有字符合法且加权长度达到最优值的提交才进入前缀检查。计时时同一输入连续调用 `--repeat` 次，数据较小时可以提高次数。结果反映重复调用的耗时，同一组实验请保持种子、数据类型和重复次数一致。

## 单独运行 C 程序

```powershell
$sources = (Get-ChildItem src -Recurse -Filter *.c).FullName
gcc -std=c17 -O2 -I src $sources -o huffman.exe
Get-Content tests/cases/order_and_conflicts.in | .\huffman.exe queue sort
```

第一个参数选 `heap` 或 `queue`，第二个选 `array`、`tree`、`sort` 或 `pairwise`。默认是 `heap array`，标准输出仍为每份提交一行 `Yes` 或 `No`。提供第三个参数（正整数）可开启计时，标准错误依次输出最优算法调用次数、总纳秒数、前缀算法调用次数、总纳秒数。
