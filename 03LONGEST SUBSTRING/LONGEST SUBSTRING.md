# Longest Substring Without Repeating Characters 解题笔记

## 题目

给定一个字符串 `s`，找出其中不含重复字符的最长子串，并返回该子串的长度。

注意：题目要求的是连续的**子串**，不是可以跳过字符的子序列。

## 第一反应

第一反应是使用窗口：枚举窗口的左端点，然后让右端点不断向右扩展；一旦遇到重复字符，就停止扩展，并从下一个左端点重新开始。

### 我一开始天真地以为,只要right和left不同就可以继续下去...

```text
for 每个左端点 left:
    for right 从 left 向右移动:
        检查 s[right] 是否已在 [left, right) 中出现
        如果已经出现:
            停止当前窗口
        更新最大长度
```

这里最容易忽略的是：

> “检查 `s[right]` 是否已经出现”不是一个天然的 $O(1)$ 操作，它的成本取决于如何保存和查询历史字符。

如果不用 hash、字符数组等额外记录，就必须逐个扫描当前窗口 `[left, right)`。窗口长度最坏为 $O(n)$，因此一次查重最坏就是 $O(n)$。

而枚举左端点并反复扩展右端点，本身需要检查的候选字符数量约为：

$$
n+(n-1)+\cdots+1=\frac{n(n+1)}{2}
$$

也就是 $O(n^2)$ 次扩展。再乘上每次线性查重的 $O(n)$，得到：

$$
O(n^2)\times O(n)=O(n^3)
$$

只有使用 `seen[]` 等结构把每次查重降为 $O(1)$ 后，这种“枚举左端点 + 向右扩展”的总复杂度才是 $O(n^2)$。大 O 记号会忽略常数系数和低阶项，所以不能写成 $O(n^2/2)$。

这个思路虽然也维护了一个窗口，但每次更换左端点后，右端点都会重新扫描已经访问过的字符，所以更准确地说是“**枚举左端点 + 向右扩展**”，还不是真正的线性滑动窗口。

### `5_expanding_window.c` 错在哪里

原来的判断是：

```c
if (s[left] == s[right]) {
    break;
}
```

它只检查新字符 `s[right]` 是否与窗口的第一个字符 `s[left]` 重复，并没有检查窗口中的其他字符。

例如输入 `"abcba"`，当 `left = 0`、`right = 3` 时，新字符 `b` 实际上与 `s[1]` 重复，但 `s[0]` 是 `a`，所以代码不会发现这个重复。

这不是单纯的复杂度计算错误，而是查重逻辑不完整。若不用 hash，正确判断必须扫描整个已有窗口：

```c
int duplicate = 0;
for (int k = left; k < right; k++) {
    if (s[k] == s[right]) {
        duplicate = 1;
        break;
    }
}
```

此外，原代码让 `right` 从 `left + 1` 开始，并且只在内层循环中更新长度，因此输入 `"a"` 时内层循环不会执行，最终错误地返回 `0`。让 `right` 从 `left` 开始即可把单字符窗口也纳入统计。

所以这次错误可以拆成三层：

1. **正确性错误**：只与 `s[left]` 比较，不能完成窗口查重；
2. **边界错误**：遗漏长度为 1 的窗口；
3. **复杂度认识错误**：完整的无 hash 查重需要扫描窗口，每次最坏为 $O(n)$。

### 纯暴力枚举（$O(n^3)$）

再退一步看最朴素的写法：枚举所有 $[left, right]$，对每个子串用第三重循环扫描窗口内部，逐字符查重。没有任何优化，三重循环：

```c
int lengthOfLongestSubstring(char *s)
{
    int maxLength = 0;

    for (int left = 0; s[left] != '\0'; left++) {

        for (int right = left; s[right] != '\0'; right++) {
            /* 检查 s[right] 是否与窗口 [left, right-1] 内的某个字符重复 */
            int duplicate = 0;
            for (int k = left; k < right; k++) {
                if (s[k] == s[right]) {
                    duplicate = 1;
                    break;
                }
            }
            if (duplicate) {
                break;
            }

            int currentLength = right - left + 1;
            if (currentLength > maxLength) {
                maxLength = currentLength;
            }
        }
    }

    return maxLength;
}
```

虽然最外层两个循环看起来也是 $n(n+1)/2 \approx n^2/2$，但每次都要用 `k` 扫描 $[left, right)$ 来查重（最坏扫描 $O(n)$ 个字符），所以总复杂度是 $O(n^3)$。这是最高、最慢的起点。

### 暴力枚举也可以使用 hash

可以给暴力枚举加上 `seen[128]`，让每次查重从线性扫描变为 $O(1)$：

```c
int lengthOfLongestSubstring(char *s)
{
    int maxLength = 0;

    for (int left = 0; s[left] != '\0'; left++) {
        int seen[128] = {0};

        for (int right = left; s[right] != '\0'; right++) {
            unsigned char current = (unsigned char)s[right];
            if (seen[current]) {
                break;
            }

            seen[current] = 1;
            int currentLength = right - left + 1;
            if (currentLength > maxLength) {
                maxLength = currentLength;
            }
        }
    }

    return maxLength;
}
```

但它仍是 $O(n^2)$。当字符串中的字符都不重复时，内层循环总执行次数仍为：

$$
n+(n-1)+\cdots+1=O(n^2)
$$

因此，**使用 hash 不等于算法必然变成 $O(n)$**。这个版本的 hash 只把“检查字符是否重复”降为 $O(1)$，但没有消除不同左端点之间的重复扫描。

对于原始 `1_sliding_window_no_hash.c` 则不同：它本来已经让 `end` 只向右走一次，唯一的额外一层复杂度来自 `temp` 扫描。用 `last[]` 取代 `temp` 后，确实正好省掉这层扫描：

| 版本 | 外层扫描 | 查重或定位重复字符 | 总复杂度 |
| --- | --- | --- | --- |
| 暴力 + hash | $O(n)$ 个左端点，每个重新扩展右端点 | $O(1)$ | $O(n^2)$ |
| 原始 `1_sliding_window_no_hash.c` | `end` 共移动 $O(n)$ 次 | `temp` 每轮最多扫描 $O(n)$ | $O(n^2)$ |
| `last[]` + 滑动窗口 | `end` 共移动 $O(n)$ 次 | $O(1)$ | $O(n)$ |

这里省掉的是第三个扫描指针 `temp`。`start` 不能省，因为仍需保存当前有效子串的左边界；`end` 可以直接由 `for` 循环下标承担。所以优化版在概念上仍有两个边界：`start` 和 `end`，只是它们不一定要写成 C 指针。

## 四个实现到底分别是什么

这几个实现容易混淆，是因为“代码里有两个边界”“可以写出 DP 状态”和“时间复杂度是 $O(n)$”是三件不同的事。

| 文件 | 算法结构 | 右边界是否回退 | 如何查重或定位重复 | 时间复杂度 |
| --- | --- | --- | --- | --- |
| `5_expanding_window.c` | 错误的枚举左端点 + 向右扩展 | 会随下一个 `left` 重新开始 | 只比较 `s[left]` 和 `s[right]`，会漏判 | 结果不正确，复杂度没有讨论意义 |
| `4_bruteforce.c` | 枚举左端点 + 向右扩展 | 会随下一个 `left` 重新开始 | `k` 线性扫描当前窗口 | $O(n^3)$ |
| `3_bruteforce_hash.c` | 枚举左端点 + 向右扩展 | 会随下一个 `left` 重新开始 | `seen[]` 直接寻址 | $O(n^2)$ |
| `1_sliding_window_no_hash.c` | 双指针滑动窗口 | `end` 从不回退 | `temp` 线性扫描当前窗口 | $O(n^2)$ |
| `2_sliding_window_hash.c` | 最后位置 + 双指针滑动窗口 | `end` 从不回退 | `last[]` 直接寻址并让 `start` 跳转 | $O(n)$ |

### `1_sliding_window_no_hash.c` 的准确名称

`1_sliding_window_no_hash.c` 是：

> **无哈希表、使用线性扫描查重的双指针滑动窗口。**

它已经具备滑动窗口的关键结构：

- `start` 和 `end` 表示当前窗口的左右边界；
- `end` 从字符串开头走到末尾，从不回退；
- 遇到重复字符后，`start` 只向右移动，不会回到更左侧。

但是，“右边界不回退”只能说明外层扫描是 $O(n)$，不能单独保证整个算法是 $O(n)$。`1_sliding_window_no_hash.c` 每次加入 `*end` 后，都用 `temp` 从 `start` 扫到 `end`：

```c
char *temp = start;
while (temp < end) {
    if (*temp == *end) {
        start = temp + 1;
        break;
    }
    temp++;
}
```

一次 `temp` 扫描最坏需要 $O(n)$，而 `end` 共有 $O(n)$ 轮，因此：

$$
O(n)\times O(n)=O(n^2)
$$

所以，`1_sliding_window_no_hash.c` 是滑动窗口，但不是线性滑动窗口。更准确地说，它的**窗口移动方式已经优化了，查重方式还没有优化**。

### 双指针不等于 DP

双指针和 DP 不是同一个概念：

- **双指针/滑动窗口**描述如何维护一个区间，以及边界如何移动；
- **DP**描述如何定义状态，并由旧状态计算新状态；
- 同一段代码有时可以同时从两种角度解释，但二者不存在必然等号。

例如，`1_sliding_window_no_hash.c` 使用 `start`、`end` 两个指针，因此它是双指针滑动窗口；但它没有保存“每个字符最后出现位置”这一转移所需的信息，只能用 `temp` 重新搜索。把它直接称为 $O(n)$ DP 会掩盖这次 $O(n)$ 搜索。

### DP 也不必然是 $O(n)$

DP 是一种组织状态和转移的方法，不是复杂度保证。一般来说：

$$
	ext{总时间}=\text{状态数量}\times\text{计算每个状态的成本}
$$

本题有 $n$ 个“以 `s[i]` 结尾”的状态：

- 如果用 `last[s[i]]` 在 $O(1)$ 时间取得上次出现位置，每个状态 $O(1)$，总时间是 $O(n)$；
- 如果像 `1_sliding_window_no_hash.c` 一样用 `temp` 在线性时间内搜索重复位置，每个状态最坏 $O(n)$，总时间是 $O(n^2)$。

因此，前文的 DP 递推式之所以能得到 $O(n)$，不是因为“它叫 DP”，而是因为递推式配合了 `last[]`，使每次状态转移都能在 $O(1)$ 内完成。

### `1_sliding_window_no_hash.c` 和线性 DP 的关系

二者维护的目标相近：都要知道“当前无重复子串从哪里开始”。区别在于获取冲突位置的方法：

| 实现 | 已保存的信息 | 遇到 `s[end]` 时怎么找重复位置 |
| --- | --- | --- |
| `1_sliding_window_no_hash.c` | 只有窗口边界 `start`、`end` | 用 `temp` 重新扫描窗口，最坏 $O(n)$ |
| DP / `2_sliding_window_hash.c` | 窗口边界和 `last[字符]` | 直接读取上次位置，$O(1)$ |

所以不能简单说“`1_sliding_window_no_hash.c` 就是那个 $O(n)$ DP”。准确说法是：`1_sliding_window_no_hash.c` 与 DP/优化滑动窗口维护相同的有效区间，但没有缓存状态转移需要的历史位置，因此复杂度仍是 $O(n^2)$。

## 优化方向

要把复杂度降到 $O(n)$，需要同时满足两点：右指针不回退，并且不能再用第三个指针重复扫描窗口。具体过程是：

1. 右指针逐个加入字符。
2. 遇到重复字符时，只移动左指针来缩小窗口。
3. 当窗口重新满足“没有重复字符”后，继续移动右指针。
4. 用计数表逐步收缩窗口，或用最后位置表让左边界直接跳转。
5. 每个字符最多被左右边界各处理常数次，因此总时间复杂度为 $O(n)$。

后续需要继续确定：使用字符计数数组逐步收缩左边界，还是记录字符最后出现的位置并让左边界直接跳转。

## 联想到 KMP：能不能计算前缀

看到“右端点重新扫描已经访问过的字符”，很容易联想到 KMP。两者确实有一个共同思想：

> 保存之前扫描得到的信息，在发生冲突时直接跳转，避免从头重复检查。

但是，这道题不需要 KMP 的前缀函数。

KMP 解决的是固定模式串的匹配问题。匹配失败时，它利用模式串内部“相等前缀与后缀”的长度，决定模式串应该跳到哪里。本题没有固定的模式串，也不关心字符串的前缀和后缀是否相等；本题真正需要知道的是：

> 当前字符上一次出现在哪里，而且那个位置是否仍在当前窗口中。

例如扫描 `"abcabcbb"`，遇到第二个 `a` 时，已知第一个 `a` 在下标 `0`，就可以直接把窗口左边界移动到下标 `1`。中间的 `b`、`c` 不需要重新扫描，因为它们仍然可以留在窗口中。

因此两种算法的跳转依据不同：

- KMP 根据前缀与后缀的匹配关系跳转。
- 本题根据重复字符最后出现的位置跳转。

## 能不能看成 DP

可以从动态规划的角度理解。

定义：

$$
dp[i]=\text{以 }s[i]\text{ 结尾的最长无重复子串长度}
$$

同时使用 `last[s[i]]` 记录字符 `s[i]` 上次出现的下标。上一个无重复子串的左边界为：

$$
left=i-dp[i-1]
$$

加入 `s[i]` 时分为两种情况。

### 情况一：上次出现位置在当前窗口之外

如果：

$$
last[s[i]]<left
$$

那么 `s[i]` 可以直接接在上一个无重复子串后面：

$$
dp[i]=dp[i-1]+1
$$

### 情况二：上次出现位置在当前窗口之内

此时必须舍弃上次出现位置及其左侧字符，新子串从该位置的下一个字符开始：

$$
dp[i]=i-last[s[i]]
$$

两种情况可以合并为：

$$
dp[i]=\min\left(dp[i-1]+1,\ i-last[s[i]]\right)
$$

实际实现时，还需要单独处理字符从未出现过的情况。最终答案是所有 `dp[i]` 中的最大值。

以 `"abba"` 为例：

| 下标 `i` | 字符 | `dp[i]` |
| --- | --- | --- |
| 0 | `a` | 1 |
| 1 | `b` | 2 |
| 2 | `b` | 1 |
| 3 | `a` | 2 |

所以最长无重复子串的长度为 `2`。

## 为什么通常仍叫滑动窗口

虽然可以写出 DP 状态转移，但每次只依赖上一个状态，没有必要保存整个 `dp` 数组。只要维护：

- 当前窗口的左边界 `left`；
- 每个字符最后出现的位置 `last`；
- 当前找到的最大长度 `maxLength`。

核心过程为：

```c
if (last[current] >= left) {
    left = last[current] + 1;
}

last[current] = right;
maxLength = max(maxLength, right - left + 1);
```

其中最关键的是：

```c
left = last[current] + 1;
```

它让左边界直接越过窗口内的重复字符，而右边界不需要回退。因此，这个解法：

- 思想上和 KMP 一样，都利用历史信息避免重复扫描；
- 状态上可以解释为 DP；
- 实现和分类上更适合称为“最后出现位置 + 滑动窗口”。

## 滑动窗口和双指针分别怎么解

首先需要澄清：**滑动窗口并不是与双指针并列的另一种算法，滑动窗口通常就是双指针的一种应用。**

在这道题中：

- `left` 是窗口的左边界；
- `right` 是窗口的右边界；
- `[left, right]` 表示当前无重复字符子串。

两种写法的主要区别是：发现重复字符后，`left` 如何移动。

### 解法一：字符计数 + 逐步收缩窗口

使用 `count[256]` 记录每个字符在当前窗口中出现的次数。

右指针加入一个新字符后，如果该字符重复，就让左指针逐步右移，同时删除窗口左侧的字符，直到重复消失。

```c
int lengthOfLongestSubstring(char *s)
{
    int count[256] = {0};
    int left = 0;
    int maxLength = 0;

    for (int right = 0; s[right] != '\0'; right++) {
        unsigned char current = (unsigned char)s[right];
        count[current]++;

        while (count[current] > 1) {
            unsigned char removed = (unsigned char)s[left];
            count[removed]--;
            left++;
        }

        int currentLength = right - left + 1;
        if (currentLength > maxLength) {
            maxLength = currentLength;
        }
    }

    return maxLength;
}
```

以 `"abba"` 为例：

```text
right = 0: [a]       无重复，最大长度为 1
right = 1: [ab]      无重复，最大长度为 2
right = 2: [abb]     b 重复
           删除 a    [bb]，仍然重复
           删除 b    [b]，重复消失
right = 3: [ba]      无重复，最大长度仍为 2
```

虽然代码中存在内层 `while`，但 `left` 和 `right` 都只会从左向右移动，不会回退。每个字符最多被加入窗口一次、移出窗口一次，所以总时间复杂度仍然是 $O(n)$。

字符数组的大小固定为 256，因此空间复杂度是 $O(1)$。

### 解法二：最后出现位置 + 直接跳转

这个版本使用 `last[256]` 记录每个字符最后一次出现的下标。

遇到窗口内的重复字符时，不再逐个删除左侧字符，而是让 `left` 直接跳到重复字符上次出现位置的后面。

```c
#include <string.h>

int lengthOfLongestSubstring(char *s)
{
    int last[256];
    memset(last, -1, sizeof(last));

    int left = 0;
    int maxLength = 0;

    for (int right = 0; s[right] != '\0'; right++) {
        unsigned char current = (unsigned char)s[right];

        if (last[current] >= left) {
            left = last[current] + 1;
        }

        last[current] = right;

        int currentLength = right - left + 1;
        if (currentLength > maxLength) {
            maxLength = currentLength;
        }
    }

    return maxLength;
}
```

仍然以 `"abba"` 为例：

```text
right = 0, a:
窗口 [0, 0] = "a"，记录 last[a] = 0

right = 1, b:
窗口 [0, 1] = "ab"，记录 last[b] = 1

right = 2, b:
上一个 b 在下标 1，并且 1 >= left
left 直接跳到 1 + 1 = 2
窗口 [2, 2] = "b"

right = 3, a:
上一个 a 在下标 0，但是 0 < left
旧的 a 已经不在当前窗口中，因此不移动 left
窗口 [2, 3] = "ba"
```

判断条件必须写成：

```c
if (last[current] >= left)
```

不能每次都无条件执行：

```c
left = last[current] + 1;
```

否则 `left` 可能向左倒退。比如 `"abba"` 扫描到最后一个 `a` 时，`left` 已经是 `2`，而上一个 `a` 位于下标 `0`。如果无条件赋值，`left` 会退回 `1`，窗口会变成包含重复字符的 `"bba"`。

### 两种写法对比

| 写法 | 保存的信息 | 遇到重复后的操作 |
| --- | --- | --- |
| 字符计数 + 滑动窗口 | 当前窗口内每个字符的出现次数 | `left` 一步一步向右移动 |
| 最后位置 + 滑动窗口 | 每个字符最后出现的下标 | `left` 直接跳到冲突位置之后 |

两种写法都是双指针维护滑动窗口，时间复杂度都是 $O(n)$，固定字符集下的空间复杂度都是 $O(1)$。

因此更准确的命名是：

1. **字符计数 + 逐步收缩滑动窗口**；
2. **最后出现位置 + 跳跃式滑动窗口**。

第二种并不是脱离滑动窗口的“双指针解法”，而是利用字符最后出现的位置，让左指针直接完成跳转。对于这道题，第二种实现通常更加简洁。

## 实际提交跑分

### 优化前（原始双指针 + while 线性扫描，$O(n^2)$）

| 指标 | 数值 | 击败 |
| --- | --- | --- |
| 执行用时 | 11 ms | 21.08% |
| 消耗内存 | 11.40 MB | 7.53% |

瓶颈：内层 `while (temp < end)` 每次从头扫描窗口查重。

### 优化后（最后出现位置 + hash 直接寻址，$O(n)$）

| 指标 | 数值 | 击败 |
| --- | --- | --- |
| 执行用时 | **0 ms** | **100.00%** |
| 消耗内存 | 12.60 MB | 5.09% |

时间从 11ms → 0ms，击败从 21% → 100%。内存略微上升（多了 128 个 int 的 `last[]` 数组），但仍然在 $O(1)$ 范围内。

### 为什么这么快

核心在于用 `last[128]` 数组实现了 **O(1) 的 hash 查重**：

- **key**：字符的 ASCII 码（0~127）
- **value**：该字符上次出现的下标
- **hash 函数**：$h(c) = c$（恒等映射，直接寻址）

因为 key 空间只有 128，数组可以直接寻址，无需处理冲突。对于原始 `1_sliding_window_no_hash.c`，`end` 已经只向右移动一次，因此用 `last[]` 替换内层 `while` 后，才从 $O(n^2)$ 降到 $O(n)$。如果仍然枚举所有左端点并重新扩展右端点，即使使用 hash，也仍然是 $O(n^2)$。

关于 Hash 为什么能消掉复杂度中的一个 $O(n)$ 因子、什么时候又不能继续降阶，参见 [Hash 查重与复杂度降阶](Hash查重与复杂度降阶.md)。

```c
#include <string.h>

int lengthOfLongestSubstring(char* s) {
    int last[128];
    memset(last, -1, sizeof(last));

    int maxLen = 0;
    int start = 0;

    for (int end = 0; s[end] != '\0'; end++) {
        unsigned char c = s[end];

        if (last[c] >= start) {
            start = last[c] + 1;
        }

        int curLen = end - start + 1;
        if (curLen > maxLen) {
            maxLen = curLen;
        }

        last[c] = end;
    }

    return maxLen;
}
```

### 可以从三种角度理解

这段优化代码同时体现了三种思想，但不表示三者彼此等价：

1. **滑动窗口**：`start` 和 `end` 维护无重复字符区间；
2. **DP 状态解释**：窗口长度对应 $dp[i]$，窗口左边界是同一状态的另一种表示；
3. **Hash 直接寻址**：`last[c]` 用字符做 key，在 $O(1)$ 时间定位上次出现位置。

滑动窗口负责避免右边界回退，`last[]` 负责避免窗口内的线性搜索。两项同时成立，才得到 $O(n)$ 实现。

## 废话

这题我高中好像做过很多遍啊...要我手写居然写不出来了。思路就是左收缩右扩展嘛。