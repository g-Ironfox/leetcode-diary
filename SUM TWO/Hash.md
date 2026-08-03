# Hash 学习笔记

[Two Sum](./SUM%20TWO.md) · [Hash](./Hash.md) · [计算机原理](./计算机原理.md)

> 从 Two Sum 的 `value -> index` 出发，整理 HashSet、HashMap、碰撞处理、状态 Hash 与数据库索引。

# 一、Hash 家族：题目需要什么，就保存什么

Hash 并不自动决定应该保存哪些内容。Hash 表中的 payload 取决于查询目标。



## 1.1 只判断答案是否存在：HashSet

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


## 1.2 返回下标：`value -> index`

Two Sum 原题要求返回位置，所以保存：

```text
value -> index
```


## 1.3 统计频率：`value -> count`

如果题目需要统计每个数出现多少次，则保存：

```text
value -> count
```

例如：

```python
frequency[x] = frequency.get(x, 0) + 1
```


## 1.4 保存所有位置：`value -> list[index]`

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


## 1.5 底层都是数组，但寻址方式不同

这些结构底层都可能使用数组，但它们的访问方式不同。


### 1.5.1 普通数组

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

### 1.5.2 直接寻址表

如果 key 的值域很小，可以直接把 key 当作数组下标：

```python
position[value] = index
```

查询和更新都可以做到严格的 `O(1)`。

但 Two Sum 的数值可能很大，也可能为负数。如果为整个值域开数组，会浪费大量空间，因此通常不适合直接寻址。

---

### 1.5.3 HashSet

HashSet 表达的是：

```text
某个 key 是否属于集合
```

它只提供存在性，不为 key 关联额外业务数据。

---

### 1.5.4 HashMap

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

# 二、Hash 表的底层结构与碰撞

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

## 2.1 拉链法

拉链法让同一个桶保存多个记录：

```text
bucket[2]
    ├── (2, payload)
    ├── (10, payload)
    └── (18, payload)
```

查询时先定位桶，再逐个比较完整 key。

---

## 2.2 开放寻址

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

## 2.3 扩容

当 Hash 表过满时，碰撞会增加，查询效率下降。

因此实现通常会在负载因子达到阈值后：

1. 创建更大的桶数组；
2. 重新计算所有 key 的桶位置；
3. 将记录迁移到新数组。

扩容本身可能需要 `O(n)`，但它并不是每次插入都发生。通过摊还分析，普通插入仍可视为平均或摊还 `O(1)`。

---

## 2.4 为什么 HashSet 底层不能只保存一个布尔值

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

# 三、手撕一个简单的 HashMap

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

# 四、Hash 碰撞与状态 Hash

“撞 Hash”在两种场景中都存在，但后果不同。

---

## 4.1 Hash 表中的碰撞

HashMap 保存完整 key。

即使两个 key 进入同一个桶，容器仍会继续比较：

```text
stored_key == query_key
```

因此，正常处理的碰撞通常只影响性能，不影响正确性。

---

## 4.2 状态 Hash 中的碰撞

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

## 4.3 `mt19937` 的位置

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

# 五、从数据库角度看：在线建立 Hash 索引

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

## 延伸阅读

- [返回 Two Sum 解题笔记](./SUM%20TWO.md)
- [继续阅读计算机原理：存储、寻址与状态转移](./计算机原理.md)
