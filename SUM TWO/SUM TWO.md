# Two Sum 解题笔记

## 题目

给定一个整数数组 nums 和一个整数目标值 target，请你在该数组中找出 和为目标值 target  的那 两个 整数，并返回它们的数组下标。

你可以假设每种输入只会对应一个答案，并且你不能使用两次相同的元素。

你可以按任意顺序返回答案。

 
```
示例 1：

输入：nums = [2,7,11,15], target = 9
输出：[0,1]
解释：因为 nums[0] + nums[1] == 9 ，返回 [0, 1] 。
示例 2：

输入：nums = [3,2,4], target = 6
输出：[1,2]
示例 3：

输入：nums = [3,3], target = 6
输出：[0,1]
 ```

提示：
```
2 <= nums.length <= 10⁴
-10⁹ <= nums[i] <= 10⁹
-10⁹ <= target <= 10⁹
只会存在一个有效答案
 ```

进阶：你可以想出一个时间复杂度小于 O(n²) 的算法吗？

## 摘要

这道题本身很简单，但我当时没有直接做出来，思考过程走了两条线：

- **A 线（排序）**：两两匹配 → 类比排序（浙江高考技术科）→ 从 `O(n²)` 优化到 `O(n log n)`；
- **B 线（Hash）**：n-gram / CSP-S2024 T3「消消乐」状态 Hash → 空间换时间 → 但一度没想清 Hash 里该存什么。

完整的心路历程：

1. 从两两匹配出发，得到 `O(n²)` 的暴力算法；
2. 类比排序中的两两比较，从熟悉的排序优化出发，尝试降到 `O(n log n)`；
3. 联想到 n-gram 和以前做过的状态 Hash，意识到可以空间换时间；
4. 但一度忘记 Hash 除了定位桶，还可以存储答案所需的下标，于是错误退回到普通数组模型；
5. 最后重新区分数组、直接寻址、HashSet 与 HashMap，并延伸到 Hash 碰撞、状态 Hash 与内存模型。

# 一、为什么没做出来
这题当时没做出来，最核心的原因其实很具体：

> 这题我高三联考做过,几乎一模一样。然后我大意了,以为只要返回两个数字而非下标就可以了[i,target-i](记忆可鲜明了)
> 我把它错误地理解成了一个 HashSet 判存在问题；
> 但那时我压根不知道 HashSet，只知道 HashMap。

我以为我高中毕业了...

不知道是独属于我这种性格的人还是很多没天赋的人都有的问题(好像算法题、语文作文都有这个问题来着): 题目看着眼熟，脑子里先蹦出一个很鲜明的旧印象，然后就很激动,就顺着那个印象滑下去。不是不会，是先入为主；不是没见过，是见过之后反而更容易想当然。最后做错了，回头一看，原因往往就很扯淡，就是活该。

这次也差不多。

我想到那套联考卷,想到高中炉火纯青的排序O(n^2)降到O(nlogn),想到ngram算法的空间换时间
但因为一开始就把题理解歪了，后面整个模型都跟着歪：

本来题目要的是下标；
我脑子里却先入成了“找两个数”；
于是问题就被我自动降格成了“补数在不在”；
再于是，它在我脑子里就越来越像一个“判存在”的题。
偏偏那时我又根本不知道 HashSet，只知道 HashMap。
所以事情就变得很别扭：我隐约觉得这题可能和 Hash 有关，但又说不清到底该怎么存；越想越乱，最后甚至会退回去把 Hash 想成普通数组多一个O(n)，然后把本来能压下去的复杂度又自己捞回来。

做题的时候脑子当然没有现在写出来这么清楚。
做题的时候就是乱的，而且那种乱也不罕见，我很熟：

觉得自己大概知道；
但其实题意已经看歪了；
后面所有推理都建立在一个歪掉的起点上；
越想越像那么回事；
最后卡住。
所以如果这篇东西后面看起来还有点条理，那也是因为题做出来以后，才有机会回头整理。
不是因为我当时真有多清醒。


# 二、问题形式化：寻找满足约束的二元组

设整数数组为：

```text
nums = [a₀, a₁, ..., aₙ₋₁]
```

目标值为 `target`。题目要求找到两个不同下标 `i` 和 `j`，满足：

```text
nums[i] + nums[j] == target
```

并返回：

```text
[i, j]
```

为避免重复枚举 `(i, j)` 和 `(j, i)`，可以统一规定：

```text
i < j
```

最直接的解法是枚举所有下标对。

```python
def two_sum_brute_force(nums, target):
    n = len(nums)

    for i in range(n):
        for j in range(i + 1, n):
            if nums[i] + nums[j] == target:
                return [i, j]
```

数组长度为 `n` 时，需要检查的二元组数量为：

```text
(n - 1) + (n - 2) + ... + 1
= n(n - 1) / 2
```

因此：

- 时间复杂度：`O(n²)`
- 额外空间复杂度：`O(1)`

这是后续优化的基线。


# 三、第一条优化路线：先建立数组的秩序

我当时首先想到的是排序。

经过高三的联考地狱以后，我对 array 留下了一种不太严格的经验：**有序可以降低判断次数，而且经常和候选范围按照 `1/2` 的幂收缩之类的关系沾边。**

我有时会把它理解成给 array 增加秩序，或者用“信息熵下降”作一个玄乎其玄的比喻。数组里的元素没有变化，但元素之间的关系变得可以利用。一次判断不再只排除眼前这一种情况，而可能排除一批候选。

Two Sum 也可以沿着这条路解决。


## 3.1 排序前必须保留原下标

题目要求返回原数组下标。如果直接排序 `nums`，元素原来的位置就会丢失。

因此，先把每个元素和原下标绑在一起：

```text
nums = [7, 2, 15, 11]

转换为：

[(7, 0), (2, 1), (15, 2), (11, 3)]
```

其中每一项表示：

```text
(元素值, 原下标)
```

按元素值排序后得到：

```text
[(2, 1), (7, 0), (11, 3), (15, 2)]
```

数值变得有序，但原下标仍被保留。


## 3.2 排序后使用双指针

排序完成后：

- `left` 指向最小值；
- `right` 指向最大值。

计算两个元素之和：

```text
current = items[left][0] + items[right][0]
```

然后分三种情况：

### 当前和等于目标值

直接返回两个元素保存的原下标。

### 当前和小于目标值

当前结果太小，需要让左边的数变大：

```python
left += 1
```

因为右边已经是当前范围内最大的数。左端元素连它都凑不到 `target`，与其他更小的元素相加也不可能得到答案。

### 当前和大于目标值

当前结果太大，需要让右边的数变小：

```python
right -= 1
```

因为左边已经是当前范围内最小的数。右端元素连它都超过 `target`，与其他更大的元素相加只会更大。

完整代码如下：

```python
def two_sum_sort(nums, target):
    items = [(value, index) for index, value in enumerate(nums)]
    items.sort(key=lambda item: item[0])

    left = 0
    right = len(items) - 1

    while left < right:
        current = items[left][0] + items[right][0]

        if current == target:
            return [items[left][1], items[right][1]]
        elif current < target:
            left += 1
        else:
            right -= 1
```

双指针最多移动 `n - 1` 次，因此排序后的扫描阶段是：

```text
O(n)
```

整个算法的复杂度主要取决于排序阶段。

---

## 3.3 将不同排序算法接入 Two Sum

排序和查找可以拆成两个独立阶段：

```text
保留原下标
→ 排序
→ 双指针
```

可以先写一个公共框架：

```python
def two_sum_with_sort(nums, target, sort_function):
    items = [(value, index) for index, value in enumerate(nums)]
    sort_function(items)

    left = 0
    right = len(items) - 1

    while left < right:
        current = items[left][0] + items[right][0]

        if current == target:
            return [items[left][1], items[right][1]]
        elif current < target:
            left += 1
        else:
            right -= 1
```

只要不同排序函数最终将 `items` 按元素值排好，后面的双指针就不需要改变。


### 3.3.1 选择排序

选择排序每轮从未排序区间中找出最小元素，放到当前起始位置。

```python
def selection_sort(items):
    n = len(items)

    for i in range(n):
        minimum = i

        for j in range(i + 1, n):
            if items[j][0] < items[minimum][0]:
                minimum = j

        items[i], items[minimum] = items[minimum], items[i]
```

调用：

```python
answer = two_sum_with_sort(nums, target, selection_sort)
```

选择排序无论数组原来是否有序，都要进行约 `n² / 2` 次比较：

- 排序时间：`O(n²)`
- 双指针：`O(n)`
- 总时间：`O(n²)`

它可以正确解题，但没有在复杂度数量级上优于暴力枚举。


### 3.3.2 插入排序

插入排序逐个处理元素，把当前元素插入前面已经有序的区间。

```python
def insertion_sort(items):
    for i in range(1, len(items)):
        current = items[i]
        j = i - 1

        while j >= 0 and items[j][0] > current[0]:
            items[j + 1] = items[j]
            j -= 1

        items[j + 1] = current
```

调用：

```python
answer = two_sum_with_sort(nums, target, insertion_sort)
```

复杂度为：

- 最好情况：`O(n)`
- 平均情况：`O(n²)`
- 最坏情况：`O(n²)`

如果输入本来接近有序，插入排序可能很快；但一般不能保证把整个 Two Sum 稳定地优化到 `O(n log n)`。


### 3.3.3 二分插入排序

“二分”本身不是排序算法。这里可以讨论的是二分插入排序。

插入排序处理第 `i` 个元素时，前面的区间已经有序，因此可以用二分查找确定插入位置：

```python
def binary_insertion_sort(items):
    for i in range(1, len(items)):
        current = items[i]

        left = 0
        right = i

        while left < right:
            middle = (left + right) // 2

            if items[middle][0] <= current[0]:
                left = middle + 1
            else:
                right = middle

        position = left

        for j in range(i, position, -1):
            items[j] = items[j - 1]

        items[position] = current
```

二分查找插入位置只需要：

```text
O(log n)
```

但找到位置之后，仍要移动数组元素：

```text
O(n)
```

所以一次插入的主要开销仍是 `O(n)`，全部插入仍可能达到：

```text
O(n²)
```

这一区别很重要：

> 查找位置是 `O(log n)`，不代表完成数组插入也是 `O(log n)`。

连续数组中间插入时，元素移动不能因为使用二分查找而消失。

---

### 3.3.4 快速排序

快速排序选择一个基准值，将元素划分到基准值两侧，再递归处理。

```python
def quick_sort(items):
    def partition(left, right):
        pivot = items[right][0]
        boundary = left

        for i in range(left, right):
            if items[i][0] <= pivot:
                items[boundary], items[i] = items[i], items[boundary]
                boundary += 1

        items[boundary], items[right] = items[right], items[boundary]
        return boundary

    def sort(left, right):
        if left >= right:
            return

        pivot_index = partition(left, right)
        sort(left, pivot_index - 1)
        sort(pivot_index + 1, right)

    sort(0, len(items) - 1)
```

调用：

```python
answer = two_sum_with_sort(nums, target, quick_sort)
```

复杂度为：

- 平均排序时间：`O(n log n)`
- 最坏排序时间：`O(n²)`
- 双指针时间：`O(n)`
- 平均总时间：`O(n log n)`

如果每次选出的基准都极不均衡，快速排序会退化。实际实现通常会使用随机基准、三数取中或混合排序来降低退化风险。


### 3.3.5 归并排序

归并排序不断将数组分成两部分，分别排好后再合并。

```python
def merge_sort(items):
    temporary = [None] * len(items)

    def sort(left, right):
        if right - left <= 1:
            return

        middle = (left + right) // 2

        sort(left, middle)
        sort(middle, right)

        i = left
        j = middle
        k = left

        while i < middle and j < right:
            if items[i][0] <= items[j][0]:
                temporary[k] = items[i]
                i += 1
            else:
                temporary[k] = items[j]
                j += 1
            k += 1

        while i < middle:
            temporary[k] = items[i]
            i += 1
            k += 1

        while j < right:
            temporary[k] = items[j]
            j += 1
            k += 1

        for k in range(left, right):
            items[k] = temporary[k]

    sort(0, len(items))
```

调用：

```python
answer = two_sum_with_sort(nums, target, merge_sort)
```

归并排序在最好、平均和最坏情况下都能保证：

```text
O(n log n)
```

因此：

- 排序：`O(n log n)`
- 双指针：`O(n)`
- 总时间：`O(n log n)`
- 额外空间：`O(n)`


## 3.4 不同排序算法的复杂度对比

| 排序方法 | 最好时间 | 平均时间 | 最坏时间 | Two Sum 总时间 |
|---|---:|---:|---:|---:|
| 选择排序 | `O(n²)` | `O(n²)` | `O(n²)` | `O(n²)` |
| 插入排序 | `O(n)` | `O(n²)` | `O(n²)` | `O(n²)` |
| 二分插入排序 | `O(n log n)` 次比较 | `O(n²)` 次移动 | `O(n²)` | `O(n²)` |
| 快速排序 | `O(n log n)` | `O(n log n)` | `O(n²)` | 平均 `O(n log n)` |
| 归并排序 | `O(n log n)` | `O(n log n)` | `O(n log n)` | `O(n log n)` |

选择、插入和二分插入都可以正确完成排序，但排序本身仍可能是 `O(n²)`，所以不能在一般情况下改善整个问题的渐近复杂度。

快速排序和归并排序才真正把排序路线推进到了：

```text
O(n log n)
```

## 3.5 排序后的另一种查找方法：逐个二分补数

排序后不一定要用双指针。也可以依次枚举每个元素，再用二分查找寻找补数。

```python
from bisect import bisect_left

def two_sum_binary_search(nums, target):
    items = [(value, index) for index, value in enumerate(nums)]
    items.sort(key=lambda item: item[0])

    for i in range(len(items)):
        need = target - items[i][0]

        left = i + 1
        right = len(items)

        while left < right:
            middle = (left + right) // 2

            if items[middle][0] < need:
                left = middle + 1
            else:
                right = middle

        if left < len(items) and items[left][0] == need:
            return [items[i][1], items[left][1]]
```

复杂度为：

- 排序：`O(n log n)`
- 执行 `n` 次二分查找：`O(n log n)`
- 总时间：`O(n log n)`

它和排序加双指针的总复杂度相同，但排序后的查找阶段不同：

```text
双指针扫描：O(n)
逐个二分查找：O(n log n)
```

因此，在 Two Sum 中，双指针通常更直接。

---

# 四、为什么有序性可以减少判断次数

前面使用“秩序”“信息熵”描述的只是一种经验。更正式地说，它和三个理论视角有关：

1. 决策树模型；
2. 信息论下界；
3. 单调性与剪枝。

前两个主要解释“判断次数为什么和候选数量的对数有关”，第三个解释排序后的 Two Sum 为什么能够安全排除候选。


## 4.1 决策树模型

只通过比较获取信息的算法，可以表示成一棵决策树：

- 一个内部节点表示一次比较；
- 比较结果对应不同分支；
- 一个叶子表示最终确定的一种结果；
- 根到叶子的路径长度表示该输入所需的比较次数；
- 树高表示最坏情况下的比较次数。

假设算法需要区分 `L` 种可能结果。若每次判断只有两个分支，做 `h` 次判断最多形成：

```text
2ʰ
```

个叶子。

为了区分所有结果，必须满足：

```text
2ʰ ≥ L
```

所以：

```text
h ≥ log₂L
```

这给出了二值判断次数的理论下界：

```text
至少需要 ceil(log₂L) 次判断
```

## 4.2 信息论下界

同一个结论也可以用信息量解释。

如果答案有 `L` 种等可能情况，那么确定答案所需的信息量为：

```text
H = log₂L
```

一次“是或否”的判断最多区分两种结果，因此最多提供：

```text
log₂2 = 1 bit
```

的信息。

所以，要从 `L` 种可能性中确定唯一结果，最坏至少需要：

```text
log₂L
```

次二值判断。

决策树和信息论说的是同一件事：

- 决策树计算树能容纳多少叶子；
- 信息论计算消除不确定性需要多少信息。


## 4.3 二分查找中的 `1/2` 幂收缩

在长度为 `n` 的有序数组中，二分查找每次都能排除大约一半候选：

```text
n
n / 2
n / 2²
n / 2³
...
n / 2ᵏ
```

当候选数量缩小到 1 时：

```text
n / 2ᵏ ≤ 1
```

所以：

```text
2ᵏ ≥ n
```

进而得到：

```text
k ≥ log₂n
```

因此二分查找只需要：

```text
O(log n)
```

次判断。

这就是我原来所谓“有序以后，判断次数和 `1/2` 的幂有某种关系”的主要来源。不过，Two Sum 的双指针不是每次排除一半，而是每次排除区间的一端，因此双指针阶段是 `O(n)`，不是 `O(log n)`。


## 4.4 比较排序为什么不能普遍突破 `O(n log n)`

对于 `n` 个互不相同的元素，一共有：

```text
n!
```

种可能排列。

比较排序必须区分这 `n!` 种情况，所以决策树至少需要 `n!` 个叶子。若最坏情况下进行 `h` 次二值比较，则：

```text
2ʰ ≥ n!
```

因此：

```text
h ≥ log₂(n!)
```

又因为 `n!` 的后半部分至少有 `n / 2` 个因子不小于 `n / 2`：

```text
n! ≥ (n / 2)^(n / 2)
```

取对数：

```text
log₂(n!)
≥ (n / 2) log₂(n / 2)
```

其数量级为：

```text
Ω(n log n)
```

因此，任何只依赖元素比较的通用排序算法，最坏情况下至少需要：

```text
Ω(n log n)
```

次比较。

归并排序能达到 `O(n log n)`，因此在比较模型下已经渐近最优。

计数排序、桶排序和基数排序能够在特定条件下突破这个下界，是因为它们利用了值域、数位等额外结构，不属于纯比较模型。


## 4.5 Two Sum 双指针的单调性证明

设当前有序区间为：

```text
items[left:right + 1]
```

### 当前和过小

如果：

```text
items[left].value + items[right].value < target
```

右端已经是当前区间中最大的元素。

左端元素连最大的元素都凑不到 `target`，那么它与其他更小的元素相加，也一定小于 `target`。

因此，可以一次排除左端元素参与的所有剩余组合：

```python
left += 1
```

### 当前和过大

如果：

```text
items[left].value + items[right].value > target
```

左端已经是当前区间中最小的元素。

右端元素连最小的元素都超过 `target`，那么它与其他更大的元素相加，也一定超过 `target`。

因此，可以一次排除右端元素参与的所有剩余组合：

```python
right -= 1
```

这就是排序后产生的单调关系。

暴力算法的一次失败判断，通常只排除当前这一对元素；双指针的一次失败判断，可以排除一个端点及其对应的一批组合。

“排序降低信息熵”在这里仍然只是一种比喻。更准确的说法是：

> 排序付出预处理成本，为数据建立可利用的单调结构；后续算法利用这种结构，使一次判断能够安全排除更多候选。


# 五、第二条优化路线：建立历史索引

排序通过改变数据的组织顺序减少搜索范围。另一种思路是不改变原数组，而是在遍历过程中建立一个查询结构。

遍历到当前元素：

```text
x = nums[i]
```

时，与它配对的元素必须满足：

```text
need = target - x
```

因此问题可以改写为：

> 在已经遍历过的元素中，是否存在值为 `need` 的元素？如果存在，它的下标是多少？

这已经不是“两两枚举”问题，而是“历史查询”问题。

如果每次都线性扫描历史元素，仍然需要 `O(n²)`。如果建立：

```text
数值 -> 下标
```

的 Hash 索引，就可以把每次查询的平均复杂度降到 `O(1)`。


## 5.1 HashMap 中应该存什么

题目要求返回下标，因此 HashMap 中应该保存：

```text
key   = 已经出现过的元素值
value = 该元素在原数组中的下标
```

即：

```text
index_by_value[value] = index
```

遍历到 `nums[i]` 时：

1. 计算 `need = target - nums[i]`；
2. 查询 `need` 是否已存在；
3. 如果存在，取出其下标；
4. 如果不存在，记录当前元素及其下标。


## 5.2 Python 实现

```python
class Solution:
    def twoSum(self, nums, target):
        index_by_value = {}

        for i, x in enumerate(nums):
            need = target - x

            if need in index_by_value:
                return [index_by_value[need], i]

            index_by_value[x] = i
```


## 5.3 C++ 实现

```cpp
#include <unordered_map>
#include <vector>
using namespace std;

class Solution {
public:
    vector<int> twoSum(vector<int>& nums, int target) {
        unordered_map<int, int> indexByValue;

        for (int i = 0; i < static_cast<int>(nums.size()); ++i) {
            int need = target - nums[i];

            auto it = indexByValue.find(need);
            if (it != indexByValue.end()) {
                return {it->second, i};
            }

            indexByValue[nums[i]] = i;
        }

        return {};
    }
};
```

## 5.4 为什么必须先查再存

正确顺序是：

```python
if need in index_by_value:
    return [index_by_value[need], i]

index_by_value[x] = i
```

HashMap 中只保存当前位置以前的元素，因此查询成功时，取出的下标必然和当前下标不同。

例如：

```text
nums = [3, 3]
target = 6
```

处理第一个 `3` 时：

```text
need = 3
```

HashMap 为空，所以先不返回，再记录：

```text
3 -> 0
```

处理第二个 `3` 时查询到：

```text
3 -> 0
```

于是返回：

```text
[0, 1]
```

如果先保存当前元素再查询，就可能把同一个元素和自己配对。


## 5.5 正确性说明

可以在遍历过程中维持一个不变条件：

> 处理下标 `i` 以前，HashMap 中保存了部分或全部已经遍历过的元素值及其下标，不包含当前位置及其后的元素。

处理当前元素 `x = nums[i]` 时：

- 如果 `target - x` 在 HashMap 中，那么该下标一定小于 `i`；
- 两个下标不同；
- 两数之和一定等于 `target`。

如果补数不在 HashMap 中，则当前元素暂时不能和此前元素组成答案，把它加入索引，供后续元素查询。

如果题目保证存在唯一答案，算法最终一定会在处理答案中位置较后的元素时找到位置较前的元素。


## 5.6 复杂度分析

遍历数组一次，共处理 `n` 个元素。

HashMap 的查询和插入平均为：

```text
O(1)
```

因此：

- 平均时间复杂度：`O(n)`
- 空间复杂度：`O(n)`

严格来说，普通 Hash 表在大量碰撞时，最坏情况可能退化。不过在通常的算法分析和标准容器假设下，查询与插入按平均 `O(1)` 计算。


# 六、解法对比

Two Sum 的几种主要解法可以整理如下：

| 方法 | 时间复杂度 | 空间复杂度 | 是否保留原下标 |
|---|---:|---:|---|
| 暴力枚举 | `O(n²)` | `O(1)` | 是 |
| `O(n²)` 排序 + 双指针 | `O(n²)` | 依实现而定 | 需要额外保存 |
| 快速排序 + 双指针 | 平均 `O(n log n)` | 依实现而定 | 需要额外保存 |
| 归并排序 + 双指针 | `O(n log n)` | `O(n)` | 需要额外保存 |
| 排序 + 逐个二分 | `O(n log n)` | `O(n)` | 需要额外保存 |
| HashMap | 平均 `O(n)` | `O(n)` | 直接保存 |

对应的核心结构分别是：

```text
暴力：
显式枚举全部二元组

排序：
建立大小顺序，再利用单调性排除候选

Hash：
建立 value -> index 的历史索引
```

对于原题，HashMap 中最小且充分的信息是：

```text
HashMap[元素值] = 原数组下标
```

如果题目只要求判断存在，则 HashSet 足够；如果题目要求返回位置，则必须进一步保存位置。

Two Sum 的最终代码很短。前面的区别主要不在代码量，而在于选择了哪一种组织信息和减少重复判断的方式。

---

## 延伸阅读

- [Hash：从集合、映射到状态 Hash](./Hash.md)
- [计算机原理：存储、寻址与状态转移](./计算机原理.md)

# 附录：写在最后

（个人随感，与算法无关。）

因为刷 B 站看到了一个大牛 br1 炫耀获奖记录，然后很愤懑自己没有天赋，然后无心迭代 AAgent，跑来做 LeetCode。

实际上我一直觉得文档性格喜欢解决工程问题，很讨厌做题目，也许这次也会是三分钟热度。虽然高中已经做了 3 年的狗屎浙江高考技术科题目了，多少还是有点得心应手。我高中最拿手的就是默默估算 O()。

我也许适合 TI 杯、智能车和 RoboMaster。不过这些比赛都要 10 月份再慢慢考虑。不想做微积分和英语，只能先看看 LeetCode 了。哎……我应该也没有什么天赋……
