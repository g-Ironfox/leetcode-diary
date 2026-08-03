/*
 * stdlib.h — 仅用于 malloc（结果数组仍需动态分配以满足 LeetCode 接口约定）。
 * limits.h — 提供 INT_MIN 作为哨兵值。
 *
 * ═══════════════════════════════════════════════════════════════
 *  本文件与 twoSum_hashmap.c 的区别：
 *  - HashMap 的 entries 数组是编译期固定大小的（栈上分配），
 *    不使用 calloc / free。
 *  - HashMap 结构体本身也直接在栈上声明，不通过 malloc 创建。
 *  - 省去了 createHashMap / freeHashMap 两个函数。
 *  - 代价：容量固定为 HASH_CAPACITY，无法按输入规模弹性伸缩。
 * ═══════════════════════════════════════════════════════════════
 */
#include <stdlib.h>
#include <limits.h>

/* ============================================================
   Two Sum — 纯 C 实现（HashMap 解法，开放寻址，静态分配）
   LeetCode 格式：int* twoSum(int* nums, int numsSize,
                              int target, int* returnSize)
   ============================================================ */

/*
 * 题目约束：numsSize ≤ 10⁴
 * 容量取 2 × 10⁴ + 1，保证负载因子 < 0.5，减少哈希冲突。
 * 这是一个编译期常量，直接决定 entries 数组的大小。
 */
#define HASH_CAPACITY 20001

typedef struct {
    int key;    /* 存入 map 的"值"（nums[i]） */
    int value;  /* 该值在原数组中的下标 i */
} HashEntry;

/*
 * 静态版 HashMap：
 * - entries 是固定长度的数组，编译期就确定了大小（约 160 KB），
 *   分配在栈上（作为局部变量时）或数据段（作为 static/全局变量时）。
 * - 不再存储 capacity（直接用宏 HASH_CAPACITY），
 *   这里保留是为了和动态版接口保持一致、方便对比学习。
 */
typedef struct {
    HashEntry entries[HASH_CAPACITY];  /* 固定数组，不需要 calloc */
    int capacity;                      /* 恒等于 HASH_CAPACITY */
} HashMap;

/*
 * initHashMap：初始化一个栈上的 HashMap。
 * 遍历所有槽位，把 key 设为 INT_MIN 作为"空槽"标记。
 *
 * 和动态版 createHashMap 的对比：
 * - 动态版：在堆上分配 map 和 entries，返回指针。
 * - 静态版：调用方先在栈上声明 HashMap map;，然后把 &map 传进来原地初始化。
 */
void initHashMap(HashMap* map) {
    map->capacity = HASH_CAPACITY;
    for (int i = 0; i < HASH_CAPACITY; i++) {
        map->entries[i].key = INT_MIN;  /* 标记为空 */
    }
}

/*
 * hashMapPut：向哈希表中插入 (key, value) 对。
 * 逻辑和动态版完全一致，只是访问 entries 的方式不变（仍通过指针）。
 */
void hashMapPut(HashMap* map, int key, int value) {
    int idx = (key % map->capacity + map->capacity) % map->capacity;
    while (map->entries[idx].key != INT_MIN && map->entries[idx].key != key) {
        idx = (idx + 1) % map->capacity;
    }
    map->entries[idx].key   = key;
    map->entries[idx].value = value;
}

/*
 * hashMapGet：在哈希表中查找 key。
 * 返回值：1 = 找到了，0 = 没找到。
 * 找到时通过指针参数 value 传出对应的下标。
 */
int hashMapGet(HashMap* map, int key, int* value) {
    int idx = (key % map->capacity + map->capacity) % map->capacity;
    while (map->entries[idx].key != INT_MIN) {
        if (map->entries[idx].key == key) {
            *value = map->entries[idx].value;
            return 1;
        }
        idx = (idx + 1) % map->capacity;
    }
    return 0;
}

/*
 * twoSum：LeetCode 1. Two Sum 的静态分配实现。
 *
 * 和动态版的关键区别：
 * ┌──────────────┬─────────────────────┬──────────────────────┐
 * │              │  动态版              │  静态版（本文件）      │
 * ├──────────────┼─────────────────────┼──────────────────────┤
 * │ map 声明     │ HashMap* map = ...  │ HashMap map;         │
 * │ 初始化       │ createHashMap()     │ initHashMap(&map)    │
 * │ 清理         │ freeHashMap(map)    │ 不需要（自动回收）    │
 * │ entries 位置 │ 堆（heap）           │ 栈（stack）           │
 * └──────────────┴─────────────────────┴──────────────────────┘
 *
 * 注意：结果数组 result 仍然用 malloc，因为 LeetCode 的调用方
 * 会负责 free 它。这是 LeetCode C 接口的硬性约定，无法改成静态。
 */
int* twoSum(int* nums, int numsSize, int target, int* returnSize) {
    /*
     * 直接在栈上声明 HashMap（约 160 KB）。
     * 栈空间通常有 1~8 MB，足够容纳。
     * 函数返回时，map 自动销毁，无需手动释放。
     */
    HashMap map;
    initHashMap(&map);

    for (int i = 0; i < numsSize; i++) {
        int need    = target - nums[i];
        int prevIdx = 0;

        if (hashMapGet(&map, need, &prevIdx)) {
            int* result = (int*)malloc(2 * sizeof(int));
            result[0] = prevIdx;
            result[1] = i;
            *returnSize = 2;
            /* 不需要 freeHashMap — map 在栈上，函数返回时自动回收 */
            return result;
        }

        hashMapPut(&map, nums[i], i);
    }

    *returnSize = 0;
    return NULL;
}
