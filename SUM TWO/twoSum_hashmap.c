/*
 * #include：C 语言的"导入"语句。它把指定头文件(.h)的内容原样复制到此处。
 * stdlib.h  — 提供 malloc / calloc / free / NULL 等内存管理函数和常量。
 * limits.h  — 提供 INT_MIN 等整型边界常量。
 */
#include <stdlib.h>
#include <limits.h>

/* ============================================================
   Two Sum — 纯 C 实现（HashMap 解法，开放寻址）
   LeetCode 格式：int* twoSum(int* nums, int numsSize,
                              int target, int* returnSize)
   ============================================================ */

/*
 * typedef struct { ... } HashEntry;
 * C 语言中，struct 定义了一个复合数据类型（类似其他语言的 class 但没有方法）。
 * typedef 给这个 struct 起一个别名 "HashEntry"，之后就可以直接用 HashEntry 声明变量，
 * 而不需要每次都写 "struct HashEntry"。
 */
typedef struct {
    int key;    /* 存入 map 的"值"（这里是 nums[i]） */
    int value;  /* 该值在原数组中的下标 i */
} HashEntry;

/*
 * HashMap：用"开放寻址法"实现的哈希表。
 * entries 是一个动态分配的数组，capacity 是数组容量。
 * "开放寻址"的意思是：发生哈希冲突时，向后线性探测下一个位置，
 * 而不是像"拉链法"那样在每个槽位挂链表。
 */
typedef struct {
    HashEntry* entries;  /* 指向堆上分配的 HashEntry 数组 */
    int capacity;        /* 数组长度（槽位数） */
} HashMap;

/*
 * createHashMap：在堆上创建并初始化一个 HashMap。
 *
 * C 语言要点：
 * 1) malloc(n)    — 在堆上分配 n 字节的连续内存，内容未初始化（随机值）。
 * 2) calloc(n, s) — 分配 n 个大小为 s 的元素，并把所有字节清零。
 * 3) (HashMap*)    — 强制类型转换。malloc 返回 void*（无类型指针），
 *                     需要显式转换成目标指针类型才能赋值。
 * 4) sizeof(X)    — 编译期运算符，返回 X 所占内存的字节数。
 * 5) ->           — 指针访问结构体成员的语法糖，等价于 (*ptr).member。
 */
HashMap* createHashMap(int capacity) {
    /* 分配 HashMap 结构体本身 */
    HashMap* map = (HashMap*)malloc(sizeof(HashMap));
    /* 分配 entries 数组并用 calloc 清零（所有字节为 0, 即 key = 0） */
    map->entries = (HashEntry*)calloc(capacity, sizeof(HashEntry));
    map->capacity = capacity;
    /*
     * 遍历所有槽位，把 key 设为 INT_MIN（int 类型的最小值，约 -21 亿）。
     * 因为 key 本来可能是 0（calloc 后的默认值），
     * 而 nums 数组中的元素完全可能是 0，所以不能用 0 标记"空槽"。
     * INT_MIN 作为哨兵值（sentinel），表示该槽位空闲。
     */
    for (int i = 0; i < capacity; i++) {
        map->entries[i].key = INT_MIN;  /* 标记为空 */
    }
    return map;
}

/*
 * hashMapPut：向哈希表中插入 (key, value) 对。
 * 如果 key 已存在，则更新其 value。
 *
 * C 语言要点：
 * 1) C 中 % 运算符对负数取模的结果也是负数（如 -7 % 5 = -2），
 *    所以用 (key % cap + cap) % cap 的技巧确保索引一定落在 [0, cap)。
 *    这个式子等价于数学意义上的 "key mod capacity"。
 */
void hashMapPut(HashMap* map, int key, int value) {
    /* 计算哈希值并映射到 [0, capacity) */
    int idx = (key % map->capacity + map->capacity) % map->capacity;
    /*
     * 线性探测：如果当前位置已被占用（且 key 不同），就往后找下一个位置。
     * 当 idx 超出数组末尾时，通过 % capacity 回绕到开头（环形探测）。
     */
    while (map->entries[idx].key != INT_MIN && map->entries[idx].key != key) {
        idx = (idx + 1) % map->capacity;
    }
    /* 找到空槽或相同 key 的槽，写入数据 */
    map->entries[idx].key   = key;
    map->entries[idx].value = value;
}

/*
 * hashMapGet：在哈希表中查找 key，把对应的 value 通过指针参数传出。
 * 返回值：1 = 找到了，0 = 没找到。
 *
 * C 语言要点：
 * 1) int* value — 这是一个"输出参数"。因为 C 函数只能有一个返回值，
 *    所以需要额外返回的数据通过指针传出。调用方传入一个变量的地址 &var，
 *    函数内部通过 *value = ... 把结果写入那个变量。
 * 2) return 1 / return 0 — C 中通常用 0 表示 false/失败，非 0 表示 true/成功。
 */
int hashMapGet(HashMap* map, int key, int* value) {
    int idx = (key % map->capacity + map->capacity) % map->capacity;
    /* 沿着探测链查找，遇到空槽（INT_MIN）就说明 key 不存在 */
    while (map->entries[idx].key != INT_MIN) {
        if (map->entries[idx].key == key) {
            *value = map->entries[idx].value;  /* 把 value 写入调用方的变量 */
            return 1;
        }
        idx = (idx + 1) % map->capacity;
    }
    return 0;
}

/*
 * freeHashMap：释放 HashMap 占用的所有堆内存。
 *
 * C 语言要点（重要！）：
 * C 没有垃圾回收（GC）。每次 malloc/calloc 分配的内存，
 * 必须用 free() 手动释放，否则会"内存泄漏"。
 * 释放顺序：先释放内部的 entries 数组，再释放 map 结构体本身。
 * 如果反过来（先 free map），entries 指针就丢失了，再也无法释放。
 */
void freeHashMap(HashMap* map) {
    free(map->entries);  /* 先释放内部数组 */
    free(map);           /* 再释放结构体本身 */
}

/*
 * twoSum：LeetCode 1. Two Sum 的纯 C 实现。
 *
 * C 语言要点 — 关于返回值和参数：
 * 1) int* 返回值        — 指向堆上分配的 int 数组（结果）。
 * 2) int* returnSize    — 输出参数，告诉调用方返回的数组长度。
 *    因为 C 的数组不携带长度信息，调用方拿到 int* 后不知道里面有几个元素，
 *    所以必须通过这个参数把长度传回去。LeetCode 的所有 C 题目都用这个模式。
 * 3) int* nums          — C 中数组以指针形式传递。nums[i] 等价于 *(nums + i)。
 * 4) 调用方负责 free —— 注释说明调用者需要 free 返回的数组，
 *    这是 C 手动内存管理的"契约"：谁分配、谁释放。
 */
int* twoSum(int* nums, int numsSize, int target, int* returnSize) {
    /*
     * 容量取 numsSize * 2 + 1，保证负载因子 < 0.5，
     * 减少哈希冲突，线性探测时能更快找到空槽。
     */
    HashMap* map = createHashMap(numsSize * 2 + 1);

    for (int i = 0; i < numsSize; i++) {
        int need    = target - nums[i];  /* 需要的互补值 */
        int prevIdx = 0;                 /* 用于接收查找结果的变量 */

        if (hashMapGet(map, need, &prevIdx)) {  /* &prevIdx 取地址，传给指针参数 */
            /* 分配 2 个 int 的空间存放结果 */
            int* result = (int*)malloc(2 * sizeof(int));
            result[0] = prevIdx;   /* 先存入的互补值的下标 */
            result[1] = i;         /* 当前下标 */
            *returnSize = 2;       /* 通过指针告知调用方数组长度为 2 */
            freeHashMap(map);      /* 用完了，释放 map 内存 */
            return result;         /* 返回结果数组（调用方负责 free） */
        }

        /* 没找到互补值，把当前 (nums[i], i) 存入 map 供后续查找 */
        hashMapPut(map, nums[i], i);
    }

    /* 遍历结束仍未找到 — 根据题目保证这不会发生，但还是要处理 */
    *returnSize = 0;
    freeHashMap(map);
    return NULL;  /* NULL 是 C 的空指针宏（定义在 stdlib.h 中） */
}
