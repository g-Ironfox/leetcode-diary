# 从 DP 到滑动窗口：为什么可以删除 dp 数组

## 一、问题来源

对于“无重复字符的最长子串”，可以先定义动态规划状态：

$$
dp[i]=\text{以 }s[i]\text{ 结尾的最长无重复子串长度}
$$

最终答案不是 `dp[n - 1]`，而是：

$$
\max_{0\le i<n}dp[i]
$$

因为最长无重复子串不一定以字符串最后一个字符结尾。

为了判断新字符是否与当前子串重复，还需要使用：

$$
last[c]=\text{字符 }c\text{ 上一次出现的下标}
$$

如果字符从未出现，令 `last[c] = -1`。

## 二、DP 状态转移

在处理 `s[i]` 之前，以 `s[i - 1]` 结尾的最长无重复子串长度为 `dp[i - 1]`，所以它的左边界是：

$$
left=i-dp[i-1]
$$

设当前字符上一次出现的位置为：

$$
previous=last[s[i]]
$$

加入 `s[i]` 时有两种情况。

### 情况一：重复字符不在当前子串中

如果：

$$
previous<left
$$

那么上一次出现的相同字符已经位于当前子串左侧，`s[i]` 可以直接接到子串末尾：

$$
dp[i]=dp[i-1]+1
$$

### 情况二：重复字符在当前子串中

如果：

$$
previous\ge left
$$

为了保留当前字符，新的子串必须从 `previous + 1` 开始：

$$
dp[i]=i-previous
$$

合并后可以写成：

$$
dp[i]=\min\left(dp[i-1]+1,\ i-last[s[i]]\right)
$$

当字符从未出现时，需要单独判断，或者把对应距离理解为无限大。

## 三、为什么可以删除 dp 数组

观察状态转移：

$$
dp[i]\leftarrow dp[i-1],\ last[s[i]]
$$

计算 `dp[i]` 时只使用 `dp[i - 1]`，不会再访问 `dp[i - 2]`、`dp[i - 3]` 等更早状态。因此没有必要保存长度为 `n` 的数组。

可以只使用一个变量 `currentLength` 保存上一轮的 `dp[i - 1]`，计算完成后直接覆盖：

```c
currentLength = dp[i];
```

这种技巧叫作 **DP 空间压缩**。空间由：

$$
O(n)
$$

降低为：

$$
O(1)
$$

这里的 $O(1)$ 指 DP 状态本身只占常数空间。字符位置表 `last[256]` 的大小也固定，所以总辅助空间仍为 $O(1)$。

## 四、压缩后的 DP 写法

```c
#include <string.h>

int lengthOfLongestSubstring(char *s)
{
    int last[256];
    memset(last, -1, sizeof(last));

    int currentLength = 0;
    int maxLength = 0;

    for (int i = 0; s[i] != '\0'; i++) {
        unsigned char current = (unsigned char)s[i];
        int previous = last[current];

        if (previous == -1 || i - previous > currentLength) {
            currentLength++;
        } else {
            currentLength = i - previous;
        }

        last[current] = i;

        if (currentLength > maxLength) {
            maxLength = currentLength;
        }
    }

    return maxLength;
}
```

判断条件：

```c
i - previous > currentLength
```

表示上一个相同字符到当前位置的距离，大于当前子串长度。也就是说，上一个相同字符位于当前子串左边界之外，不会造成冲突。

## 五、从 currentLength 变成 left

当前无重复子串的长度为：

$$
currentLength=i-left+1
$$

因此 `currentLength` 和 `left` 描述的是同一个区间状态：

$$
left=i-currentLength+1
$$

与其维护“当前长度”，还可以直接维护“当前窗口左边界”。遇到窗口内的重复字符时执行：

```c
left = last[current] + 1;
```

于是压缩后的 DP 可以改写为滑动窗口：

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

这不是把 DP 换成了一个完全无关的算法，而是换了一种更直观的状态表示：

| DP 表示 | 滑动窗口表示 |
| --- | --- |
| `dp[i]` | `right - left + 1` |
| 上一个状态 `dp[i - 1]` | 移动前的窗口长度 |
| `last[s[i]]` 判断能否扩展 | `last[current] >= left` 判断是否冲突 |
| 更新当前长度 | 更新窗口左边界 |

## 六、为什么 left 不能向左回退

更新左边界时必须先判断：

```c
if (last[current] >= left)
```

以 `"abba"` 为例：

```text
right = 0: 窗口 "a"，  left = 0
right = 1: 窗口 "ab"， left = 0
right = 2: b 重复，    left = 2
right = 3: 上一个 a 在下标 0，已经位于窗口外
```

扫描最后一个 `a` 时，不能把 `left` 从 `2` 改回 `1`。否则窗口会变成 `"bba"`，再次包含重复字符。

因此 `left` 必须满足一个滑动窗口的重要不变量：

> `left` 和 `right` 都只能向右移动，不能回退。

## 七、用 abba 对照三种状态

| `right` | 字符 | `last` 中的旧位置 | `left` | 当前窗口 | `dp[right]` |
| --- | --- | --- | --- | --- | --- |
| 0 | `a` | -1 | 0 | `"a"` | 1 |
| 1 | `b` | -1 | 0 | `"ab"` | 2 |
| 2 | `b` | 1 | 2 | `"b"` | 1 |
| 3 | `a` | 0 | 2 | `"ba"` | 2 |

可以看到每一行都满足：

$$
dp[right]=right-left+1
$$

这说明 DP 的“当前长度”和滑动窗口的“左右边界”是等价状态。

## 八、优化过程总结

完整的思考路线是：

```text
定义 dp[i]
    ↓
发现 dp[i] 只依赖 dp[i - 1]
    ↓
用 currentLength 替代整个 dp 数组
    ↓
发现 currentLength = right - left + 1
    ↓
改为直接维护 left 和 right
    ↓
得到“最后出现位置 + 滑动窗口”
```

对应的复杂度变化为：

| 实现 | 时间复杂度 | 辅助空间复杂度 |
| --- | --- | --- |
| 完整 DP 数组 + `last` | $O(n)$ | $O(n)$ |
| 压缩 DP + `last` | $O(n)$ | $O(1)$ |
| 滑动窗口 + `last` | $O(n)$ | $O(1)$ |

表中三个版本都是 $O(n)$，有一个不可省略的前提：它们都用 `last[s[i]]` 在 $O(1)$ 时间取得字符上次出现的位置。DP 本身不保证时间复杂度是 $O(n)$。一般应按下面的式子分析：

$$
	ext{总时间}=\text{状态数量}\times\text{单次状态转移成本}
$$

本题共有 $O(n)$ 个状态。配合 `last[]` 时，单次转移为 $O(1)$，所以总时间为 $O(n)$；如果不保存 `last[]`，而是在每轮用第三个指针线性搜索重复字符，单次转移最坏为 $O(n)$，总时间仍是 $O(n^2)$。

因此，“DP 状态可以压缩成窗口边界”只说明两种状态表示等价，不表示任意双指针实现都自动拥有 $O(1)$ 的状态转移。原始 `1_sliding_window_no_hash.c` 就是反例：它是双指针滑动窗口，但 `temp` 的重复扫描使它仍为 $O(n^2)$。

核心不是简单地“删除数组”，而是先证明：

1. 当前状态只依赖前一个 DP 状态；
2. 更早的 DP 值不会再次使用；
3. 最终最大值可以在遍历过程中同步更新；
4. 被压缩的当前长度可以由窗口边界直接表示。

满足这些条件后，才能安全地从完整 DP 数组压缩到变量，并进一步写成滑动窗口。