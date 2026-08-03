# 摘要
- **A 线：两两匹配 → 类比排序(浙江高考技术科信息) → 从 `O(n²)` 往 `O(n log n)` 优化**
- **B 线：n-gram / csp-s2024 T3`消消乐` 状态 Hash → 空间换时间 → 但一时没想清 Hash 中应该存什么**

Two Sum 是一道很简单的题：在数组中找到两个和为 `target` 的数，并返回它们的下标。

标准答案是用 HashMap，在 `O(n)` 时间内完成。但如果只看最终代码，这道题几乎没有讨论价值。真正值得记录的是，我解决它时经历的几层思路：

1. 从两两匹配出发，得到 `O(n²)` 的暴力算法；
2. 将它类比成冒泡排序中的两两比较，从熟悉的排序优化出发，尝试降到 `O(n log n)`；
3. 联想到 n-gram 和以前做过的状态 Hash，意识到可以空间换时间；
4. 但一度忘记 Hash 除了定位桶，还可以存储答案所需的下标，于是错误地退回到了普通数组模型；
5. 最后重新区分数组、直接寻址、HashSet 和 HashMap，并进一步想到 Hash 碰撞、状态 Hash 与内存模型。

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


# 六、HashSet 与 HashMap：题目需要什么，就保存什么

Hash 并不自动决定应该保存哪些内容。Hash 表中的 payload 取决于查询目标。



## 6.1 只判断答案是否存在：HashSet

如果题目只问：

> 是否存在两个不同元素之和等于 `target`？

那么只需要知道某个值以前是否出现过：

```python
def has_two_sum(nums, target):
    seen = set()

    for x in nums:
        if target - x in seen:
            return True

        seen.add(x)

    return False
```

这里不需要保存下标，使用 HashSet 即可。


## 6.2 返回下标：`value -> index`

Two Sum 原题要求返回位置，所以保存：

```text
value -> index
```


## 6.3 统计频率：`value -> count`

如果题目需要统计每个数出现多少次，则保存：

```text
value -> count
```

例如：

```python
frequency[x] = frequency.get(x, 0) + 1
```


## 6.4 保存所有位置：`value -> list[index]`

如果后续需要获取一个值的全部出现位置，则保存：

```text
value -> 下标列表
```

例如：

```python
positions = {}

for i, x in enumerate(nums):
    positions.setdefault(x, []).append(i)
```

因此，所谓空间换时间并不只是“多存一些数据”，而是：

> 根据后续查询需要，提前建立能够回答问题的索引。


# 七、普通数组、直接寻址、HashSet 与 HashMap

这些结构底层都可能使用数组，但它们的访问方式不同。


## 7.1 普通数组

普通数组提供：

```text
下标 -> 元素
```

如果已知下标，可以通过地址计算直接访问元素：

```text
base_address + index × element_size
```

但如果只知道某个值，想判断它是否存在，通常仍要线性扫描。

---

## 7.2 直接寻址表

如果 key 的值域很小，可以直接把 key 当作数组下标：

```python
position[value] = index
```

查询和更新都可以做到严格的 `O(1)`。

但 Two Sum 的数值可能很大，也可能为负数。如果为整个值域开数组，会浪费大量空间，因此通常不适合直接寻址。

---

## 7.3 HashSet

HashSet 表达的是：

```text
某个 key 是否属于集合
```

它只提供存在性，不为 key 关联额外业务数据。

---

## 7.4 HashMap

HashMap 表达的是：

```text
key -> payload
```

Two Sum 中：

```text
数值 -> 原数组下标
```

Hash 表底层使用数组，并不意味着它和普通数组的查询方式相同：

```text
普通数组：已知 index，访问 value
HashMap：已知 key，经过 Hash 后定位记录
```

真正的区别不在于底层是否使用数组，而在于它们采用了不同的寻址协议。

---

# 八、Hash 表的底层结构与碰撞

Hash 表通常按以下过程查找：

```text
key
→ 计算 hash(key)
→ 映射到 bucket
→ 在桶中确认完整 key
→ 取得对应 payload
```

由于桶的数量有限，不同 key 可能进入同一个桶。

例如桶数量为 8：

```text
2  % 8 = 2
10 % 8 = 2
18 % 8 = 2
```

这就是 Hash 碰撞。

---

## 8.1 拉链法

拉链法让同一个桶保存多个记录：

```text
bucket[2]
    ├── (2, payload)
    ├── (10, payload)
    └── (18, payload)
```

查询时先定位桶，再逐个比较完整 key。

---

## 8.2 开放寻址

开放寻址只使用一个桶数组。发生碰撞时，根据某种规则继续寻找其他空槽：

- 线性探测；
- 二次探测；
- 双重 Hash。

例如线性探测：

```text
hash(key) 占用
→ 检查下一个位置
→ 直到找到 key 或空槽
```

---

## 8.3 扩容

当 Hash 表过满时，碰撞会增加，查询效率下降。

因此实现通常会在负载因子达到阈值后：

1. 创建更大的桶数组；
2. 重新计算所有 key 的桶位置；
3. 将记录迁移到新数组。

扩容本身可能需要 `O(n)`，但它并不是每次插入都发生。通过摊还分析，普通插入仍可视为平均或摊还 `O(1)`。

---

## 8.4 为什么 HashSet 底层不能只保存一个布尔值

从抽象接口看，HashSet 只回答：

```text
存在 / 不存在
```

但普通 HashSet 的桶中不能真的只保存一个 `True`，完全不保存 key。

假设：

```text
hash(a) == hash(b)
```

但：

```text
a != b
```

如果桶中只有一个布尔值，就无法判断存在的是 `a` 还是 `b`。

因此普通 HashSet 仍要保存完整 key，用于碰撞后的相等性判断。

只有在以下特殊场景中，才可能只用一个 bit 表示存在性：

- key 值域很小，可以直接寻址；
- 使用无碰撞的完美 Hash；
- 允许概率性误判，例如 Bloom Filter。

---

# 九、手撕一个简单的 HashMap

标准库已经提供 HashMap，但为了区分“普通数组”和“桶数组”，可以手写一个使用拉链法的简化版本。

下面只支持整数 key 和整数 value。

```cpp
#include <list>
#include <utility>
#include <vector>
using namespace std;

class SimpleHashMap {
private:
    static const int CAPACITY = 1009;

    vector<list<pair<int, int>>> buckets;

    int getBucketIndex(int key) const {
        int index = key % CAPACITY;

        if (index < 0) {
            index += CAPACITY;
        }

        return index;
    }

public:
    SimpleHashMap() : buckets(CAPACITY) {}

    void put(int key, int value) {
        int index = getBucketIndex(key);

        for (auto& entry : buckets[index]) {
            if (entry.first == key) {
                entry.second = value;
                return;
            }
        }

        buckets[index].push_back({key, value});
    }

    bool get(int key, int& value) const {
        int index = getBucketIndex(key);

        for (const auto& entry : buckets[index]) {
            if (entry.first == key) {
                value = entry.second;
                return true;
            }
        }

        return false;
    }
};
```

使用它完成 Two Sum：

```cpp
#include <vector>
using namespace std;

vector<int> twoSum(vector<int>& nums, int target) {
    SimpleHashMap indexByValue;

    for (int i = 0; i < static_cast<int>(nums.size()); ++i) {
        int need = target - nums[i];
        int previousIndex;

        if (indexByValue.get(need, previousIndex)) {
            return {previousIndex, i};
        }

        indexByValue.put(nums[i], i);
    }

    return {};
}
```

这里的桶数组并不是按顺序保存原数组元素，而是按照：

```text
hash(key) -> bucket
```

组织记录。

一次查询包含两个步骤：

1. 根据 key 计算桶位置；
2. 在桶中比较完整 key，处理碰撞。

这就是 HashMap 和普通数组在线性查找上的根本区别。

该实现没有自动扩容，因此只能用于说明基本结构，不适合作为完整工业实现。

---

# 十、Hash 表碰撞与状态 Hash 碰撞

“撞 Hash”在两种场景中都存在，但后果不同。

---

## 10.1 Hash 表中的碰撞

HashMap 保存完整 key。

即使两个 key 进入同一个桶，容器仍会继续比较：

```text
stored_key == query_key
```

因此，正常处理的碰撞通常只影响性能，不影响正确性。

---

## 10.2 状态 Hash 中的碰撞

有些算法会把复杂状态压缩成一个整数：

```text
完整状态 -> 64 位 Hash
```

然后只保存这个整数：

```cpp
unordered_set<uint64_t> visited;
```

如果两个不同状态产生相同 Hash：

```text
state_a != state_b
hash(state_a) == hash(state_b)
```

算法就可能把它们误判为同一个状态。

这里没有保存完整状态进行二次确认，因此碰撞可能直接影响正确性。

常见处理方法包括：

- 使用双 Hash；
- 使用 128 位或更宽的 Hash；
- 使用随机 Hash；
- 命中后比较完整状态；
- 直接以完整状态作为容器的 key。

---

## 10.3 `mt19937` 的位置

`mt19937` 是伪随机数生成器，不是 HashMap，也不等于 Hash 函数。

它可以用于生成随机权值，从而构造：

- Zobrist Hash；
- 随机状态 Hash；
- 自定义 Hash 的随机种子；
- 防止输入针对固定 Hash 函数构造大量碰撞。

因此，状态 Hash 和 Two Sum 使用的 HashMap 属于同一片知识范围，但处于不同层次：

```text
Two Sum：
建立 value -> index 的精确索引

状态 Hash：
将复杂状态压缩成较短表示，用于判重或比较
```

---

# 十一、从数据库角度看：在线建立 Hash 索引

如果把数组元素看成记录：

```text
(index, value)
```

那么遍历到当前记录 `(i, x)` 时，需要执行的查询近似于：

```sql
SELECT index
FROM history
WHERE value = target - x;
```

如果没有索引，就要扫描所有历史记录。

如果建立：

```text
HashIndex[value] -> index
```

就可以平均 `O(1)` 查询补数。

因此，Two Sum 的 Hash 解法也可以理解为：

> 一边扫描数据，一边为已扫描记录建立 `value -> index` 的在线 Hash 索引。

它和 n-gram、词频统计、状态判重等任务有相似的处理模式：

```text
组织历史信息
→ 当前输入生成查询键
→ 查询历史索引
→ 更新历史索引
```

具体匹配关系不同，但“建立历史索引以避免重复扫描”这一结构是相同的。

---
可以保留，而且这部分其实能作为前文的进一步抽象。问题主要是原来的十二、十三、十四节层次有些混杂：

1. 数据结构如何落到存储中；
2. “位置”不等于物理地址；
3. 程序如何通过状态转移执行；
4. 最后再联系图灵完备。

建议按这个顺序重写。下面这版可以直接替换原来的十二、十三、十四节。

---

# 十二、从数据结构回到存储与寻址

数组、链表、树和 Hash 表看起来是不同的数据结构，但它们最终都要表示在某种存储介质中。

它们之间的区别，不在于使用了不同种类的“物质”，而主要在于：

```text
状态如何布局
+ 状态之间如何关联
+ 程序按照什么规则访问这些状态
```

换句话说，数据结构不仅是“存了什么”，还包括“如何找到它”。

---

## 12.1 数组

数组通常可以理解为：

```text
连续存储
+ 下标寻址
```

如果数组首地址为 `base`，每个元素占用 `element_size` 个字节，那么第 `i` 个元素的位置可以表示为：

```text
base + i × element_size
```

因此，已知下标时，数组可以直接计算元素位置，实现 `O(1)` 随机访问。

---

## 12.2 链表

链表可以理解为：

```text
节点内容
+ 下一节点的位置
```

这里的“位置”不一定是裸指针，也可以是数组下标、对象引用或其他标识。

例如，可以在数组中模拟链表：

```cpp
struct Node {
    int value;
    int next;
};

Node pool[100000];
```

其中：

```text
next = 下一个节点在 pool 中的下标
```

虽然所有节点都存放在数组里，但访问规则不再是“下标连续递增”，而是沿着 `next` 所描述的关系移动。

因此，决定它是链表的，不是底层有没有数组，而是节点之间采用了链式寻址关系。

---

## 12.3 树

树可以理解为：

```text
节点内容
+ 子节点的位置
```

子节点的位置同样可以使用不同方式表示：

- 指针；
- 对象引用；
- 数组下标；
- 节点编号；
- 数据库中的记录 ID。

例如，二叉树节点可以保存左右子节点的数组下标：

```cpp
struct TreeNode {
    int value;
    int left;
    int right;
};
```

只要程序能够根据 `left` 和 `right` 找到对应节点，就能表达树形关系，并不要求程序直接操作物理地址。

---

## 12.4 Hash 表

Hash 表通常可以理解为：

```text
桶数组
+ key 到桶的映射规则
+ 碰撞处理规则
+ 扩容与重建规则
```

它的底层可能同样使用数组，但访问方式不是：

```text
已知数组下标 -> 读取元素
```

而是：

```text
已知 key
→ 计算 hash(key)
→ 定位桶
→ 比较完整 key
→ 取得对应 payload
```

因此，普通数组和 Hash 表即使都以数组作为底层存储，也属于不同的数据结构。区别不在于“是不是数组”，而在于它们采用了不同的寻址协议。

---

## 12.5 数据结构是一组存储与访问规则

从这个角度看，数据结构可以统一描述为：

> 对状态采用某种布局，通过位置、引用或键建立关系，再规定读取、写入和寻址的方式。

例如：

```text
数组：
连续布局 + 下标寻址

链表：
节点布局 + 后继关系

树：
节点布局 + 父子关系

Hash 表：
桶布局 + Hash 寻址

数据库索引：
页面布局 + key 到记录位置的映射
```

同一块存储空间，可以因为访问规则不同而表达不同的数据结构。

---

# 十三、“位置”不等于裸物理地址

数据结构经常需要表达：

```text
另一个对象在哪里
```

但这里的“位置”不一定是实际的物理内存地址。

现代应用程序通常不能任意读取和写入物理地址。程序看到的地址还可能受到以下机制的管理：

- 虚拟内存；
- 进程隔离；
- 内存保护；
- 类型系统；
- 数组边界检查；
- 垃圾回收；
- 所有权与生命周期管理。

不过，不能直接访问物理地址，并不妨碍程序表达链表、树、图和索引。

数据结构真正需要的是：

> 一种能够稳定标识某个状态，并通过该标识再次访问它的方式。

这种标识可以是：

- C/C++ 中的指针；
- Java、Python 中的对象引用；
- 数组下标；
- 文件偏移量；
- 数据库页号和槽号；
- 对象 ID；
- 键值映射中的 key。

例如，下面两个结构在抽象意义上都能表示链表：

```cpp
struct PointerNode {
    int value;
    PointerNode* next;
};
```

```cpp
struct IndexNode {
    int value;
    int next;
};
```

前者通过指针寻找下一节点，后者通过数组下标寻找下一节点。两者底层实现不同，但都表达了同一种关系：

```text
当前节点 -> 下一节点
```

因此，数据结构依赖的并不是对裸物理地址的控制，而是：

```text
可保存的位置标识
+ 根据标识完成寻址的能力
```

只要系统提供某种稳定的间接访问机制，就能够表达连接、层级、索引和引用关系。

---

# 十四、从数据存储到程序执行：状态与状态转移

如果继续向下抽象，程序运行本身也可以看成对状态的不断读取和更新。

一个正在执行的程序至少需要保存：

- 当前数据；
- 中间结果；
- 控制信息；
- 下一步执行的位置。

这些内容在现代计算机中可能分散在内存、寄存器、栈、缓存和程序计数器中。但从抽象机器的角度看，它们都可以被视为程序状态的一部分。

---

## 14.1 寄存器也可以抽象为特殊的存储单元

假设一个抽象机器没有独立的寄存器，但拥有可读写、可寻址的存储空间，那么可以划出一部分存储单元，约定它们承担寄存器的作用：

```text
memory[R0] = 当前操作数
memory[R1] = 中间结果
memory[SP] = 当前栈顶位置
memory[PC] = 下一条指令的位置
```

其中 `PC` 表示程序计数器，即下一条将要执行的指令位置。

在这个抽象模型中，修改：

```text
memory[PC]
```

就相当于修改下一步执行的位置，从而改变程序的执行流。

当然，现实中的 CPU 不会简单地把所有东西都当作普通内存处理。现代处理器通常拥有：

- 通用寄存器；
- 程序计数器；
- 栈指针；
- 多级缓存；
- 流水线；
- 分支预测；
- 虚拟内存。

这些结构之间的速度、用途和访问方式存在显著差异。寄存器通常比主存更快、数量更少，并直接参与指令执行。

但从抽象计算模型来看，寄存器仍然可以被理解为：

> 一组特殊、快速、数量有限的状态存储单元。

因此，把寄存器抽象成内存中的特殊位置，不是在描述真实 CPU 的物理实现，而是在说明：它们在表达计算状态时具有一定的功能等价性。

---

## 14.2 程序执行可以表示为状态转移

设程序在时刻 `t` 的完整状态为：

```text
Sₜ
```

这个状态可以包含：

```text
Sₜ = {
    当前数据,
    中间结果,
    寄存器内容,
    内存内容,
    栈状态,
    下一条指令的位置
}
```

执行一步指令，就是根据当前状态产生下一个状态：

```text
Sₜ → Sₜ₊₁
```

也可以写成：

```text
Sₜ₊₁ = F(Sₜ)
```

其中 `F` 表示机器的状态转移规则。

例如，一条加法指令可能完成：

```text
读取两个操作数
→ 计算它们的和
→ 把结果写回某个位置
→ 更新下一条指令的位置
```

于是，程序运行可以统一描述为：

```text
读取状态
→ 执行基本运算
→ 写回状态
→ 更新控制状态
```

---

## 14.3 顺序、分支、循环和跳转也是状态更新

在这个模型中，顺序执行、条件分支、循环和跳转并不是完全不同的现象。它们都可以理解为对“下一步执行位置”的不同更新。

### 顺序执行

```text
PC = PC + 指令长度
```

程序继续执行下一条指令。

### 条件分支

```text
if condition:
    PC = branch_target
else:
    PC = next_instruction
```

程序根据当前状态选择不同的下一位置。

### 无条件跳转

```text
PC = jump_target
```

程序直接跳到另一个位置。

### 循环

循环可以理解为：

```text
执行循环体
→ 检查条件
→ 条件成立时把 PC 更新回循环起点
```

因此，循环并不需要一种完全独立的执行机制。它可以由：

```text
状态保存
+ 条件判断
+ 跳转
```

组合得到。

---

## 14.4 数据状态与控制状态

通常我们会区分：

```text
数据：
程序正在处理什么

控制：
程序下一步执行什么
```

这种区分在编程和硬件设计中非常重要。但继续向下抽象时，二者都可以表示为某种可保存、可读取、可更新的状态。

例如：

```text
变量 x 的值
```

是数据状态；

```text
程序计数器 PC 的值
```

是控制状态。

它们的语义不同，但都可以通过某种存储单元表示，并在程序运行过程中发生变化。

因此，从更一般的角度看，数据和控制可以统一到下面的框架中：

```text
可读写状态
+ 状态转移规则
```

程序的执行过程，就是机器按照规则不断把当前状态变换为下一个状态。

---

## 14.5 与计算能力和图灵完备的关系

这种“状态 + 状态转移”的描述与图灵机、寄存器机等计算模型的方向是一致的，但还不能简单地说：

```text
只要能保存和更新状态，就一定图灵完备
```

因为有限状态机同样能够保存状态并执行状态转移，但它的计算能力仍然有限。

如果希望一个抽象系统具备通用计算能力，通常还需要：

- 可读写并可寻址的存储；
- 基本的数据运算；
- 条件判断；
- 跳转或其他形式的重复执行；
- 在理论模型中可无限扩展的存储能力。

最后一点是理论上的理想化条件。现实计算机的内存当然是有限的，因此严格来说，任何固定配置的现实机器都只有有限数量的状态。讨论图灵完备时，通常假设存储空间可以根据需要继续扩展。

在这个前提下，基本操作可以不断组合：

```text
读写状态
→ 进行运算
→ 根据条件选择路径
→ 跳转并重复执行
```

由此表达顺序、分支、循环、递归以及更复杂的算法过程。

因此，更准确的总结是：

> 程序可以被理解为一个持续进行状态转移的系统。数据结构规定状态如何组织和寻址，指令规定状态如何变化，控制状态决定下一次应用哪条变化规则。

从 Two Sum 中的数组和 Hash 表出发，继续向下看，最终遇到的仍然是同一个问题：

```text
状态存在哪里？
如何找到它？
读取之后如何更新？
下一步又要访问哪里？
```

数组、链表、树和 Hash 表回答的是“数据如何组织”；程序计数器、分支和跳转回答的是“计算如何推进”。二者虽然处在不同层次，但都建立在：

```text
状态表示
+ 寻址方式
+ 状态转移规则
```

这一共同框架之上。


---

# 十五、解法对比

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

# 废话
因为刷b站看到了一个大牛br1炫耀获奖记录,然后很愤懑自己没有天赋,然后无心迭代AAgent,跑来做leetcode。
实际上我一直觉得文档性格喜欢解决工程问题,很讨厌做题目,也许这次也会是三分钟热度。虽然高中已经做了3年的狗屎浙江高考技术科题目了,多少还是有点得心应手。我高中最拿手的就是默默估算O()。
我也许适合TI杯、智能车和robomaster。不过这些比赛都要10月份再慢慢考虑。不想做微积分和英语,只能先看看leetcode了。哎...我应该也没有什么天赋...