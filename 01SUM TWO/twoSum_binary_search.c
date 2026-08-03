/*
 * ============================================================
 * LeetCode 1. Two Sum — 排序 + 二分查找 解法
 * ============================================================
 *
 * 【算法思路】
 *   1. 把原数组的 (值, 下标) 打包成一个结构体数组
 *   2. 按"值"从小到大排序（qsort）
 *   3. 遍历每个元素 A[i]，算出需要的搭档 needed = target - A[i].value
 *   4. 在 A[i+1 .. n-1] 中用二分查找找 needed
 *   5. 找到 → 返回两个原始下标
 *
 *   时间复杂度: O(n log n)（排序 O(n log n) + n 次二分 O(n log n)）
 *   空间复杂度: O(n)（额外开了结构体数组）
 */

/* ---------- C 语言头文件 ---------- */

#include <limits.h>   /* INT_MIN, INT_MAX 常量，表示 int 的边界值 */
#include <stdlib.h>   /* malloc, free, qsort, size_t, NULL */

/*
 * ---------- 结构体定义 ----------
 *
 * C 语言里没有内置的 pair / tuple，所以自己定义一个结构体。
 * typedef 让后续使用"NumberWithIndex"即可，不用每次写"struct NumberWithIndex"。
 */
typedef struct {
    int value;          /* 数值 */
    int originalIndex;  /* 在原数组中的下标（排序后也不会丢） */
} NumberWithIndex;

/*
 * ---------- qsort 的比较函数 ----------
 *
 * qsort 是 C 标准库的通用排序函数，需要你传一个"比较函数指针"。
 * 比较函数签名必须为: int cmp(const void* a, const void* b)
 *   - 返回值 < 0  表示 a 排在 b 前面
 *   - 返回值 > 0  表示 b 排在 a 前面
 *   - 返回值 == 0 表示两者相等（顺序无所谓）
 *
 * 【为什么参数是 const void*，不能直接传 NumberWithIndex*？】
 *   C 语言没有泛型（template），qsort 要能排 int、排 double、排任意
 *   struct，就必须用一种"万能类型"来接收——这就是 void*（通用指针）。
 *   void* 可以指向任何类型，但不携带类型信息，就像快递员只认"包裹"，
 *   不管里面是手机还是衣服。qsort 第四个参数的类型是写死的：
 *     int (*cmp)(const void*, const void*)
 *   如果你写成 compareByValue(const NumberWithIndex* a, ...)，
 *   函数指针类型不匹配，编译直接报错。所以必须用 void* 接收，
 *   然后在函数内部强制转换回真正的类型，才能访问 ->value 等成员。
 *
 * 【C 语言知识点】
 *   const void* : 通用指针，qsort 用它实现"泛型"排序。
 *   (const NumberWithIndex*) : C 风格强制类型转换，把 void* 还原为实际类型。
 *   -> : 箭头运算符，等价于 (*指针).成员。p->value 就是 (*p).value。
 *   static : 限定函数仅在本文件内可见（类似 private）。
 */
static int compareByValue(const void* left, const void* right) {
    /* 把通用指针 void* 转成我们自己的类型 */
    const NumberWithIndex* leftItem  = (const NumberWithIndex*)left;
    const NumberWithIndex* rightItem = (const NumberWithIndex*)right;

    /* 按 value 升序排列 */
    if (leftItem->value < rightItem->value) {
        return -1;   /* left 比 right 小，left 排前面 */
    }
    if (leftItem->value > rightItem->value) {
        return 1;    /* left 比 right 大，right 排前面 */
    }
    return 0;        /* 相等 */
}

/*
 * ---------- 二分查找 ----------
 *
 * 在已排序数组 items[left .. right] 中查找 target。
 * 返回匹配元素的下标；找不到返回 -1。
 *
 * 【C 语言知识点】
 *   const NumberWithIndex* items : 指针参数，指向数组首元素。
 *      加了 const 表示函数不会修改数组内容（只读）。
 *   int middle = left + (right - left) / 2;
 *      不写 (left + right) / 2 是为了防止 left + right 溢出。
 */
static int binarySearch(const NumberWithIndex* items,
                        int left, int right, int target) {
    while (left <= right) {   /* 区间还有元素就继续 */
        /* 计算中间位置（防溢出写法） */
        int middle = left + (right - left) / 2;

        if (items[middle].value == target) {
            return middle;                           /* 找到了 */
        }
        if (items[middle].value < target) {
            left = middle + 1;                       /* target 在右半边 */
        } else {
            right = middle - 1;                      /* target 在左半边 */
        }
    }

    return -1;   /* 没找到 */
}

/*
 * ---------- 主函数: twoSum ----------
 *
 * LeetCode 要求的函数签名:
 *   int* twoSum(int* nums, int numsSize, int target, int* returnSize)
 *
 * 参数:
 *   nums      : 输入数组（int 指针在 C 中即代表数组）
 *   numsSize  : 数组长度
 *   target    : 目标和
 *   returnSize: 【输出参数】告诉 LeetCode 返回数组有几个元素（0 或 2）
 *
 * 返回值:
 *   指向 int[2] 的指针（malloc 分配），存两个下标。
 *   无解时返回 NULL。
 *
 * 【C 语言知识点】
 *   int* nums : C 语言中，数组以指针形式传递，不携带长度信息，
 *               所以需要额外的 numsSize 参数。
 *   returnSize : 这是一个"输出参数"模式。因为 C 函数只能有一个返回值，
 *               额外的输出信息通过指针传出来。调用方先准备好一个 int 变量，
 *               把它的地址传进来，函数通过 *returnSize 修改它。
 *   malloc / free : C 的动态内存分配 / 释放，类似 C++ 的 new / delete。
 *   NULL : 空指针，表示"没有分配"或"不存在"。
 */
int* twoSum(int* nums, int numsSize, int target, int* returnSize) {
    /* 先设返回大小为 0，防止调用方读到脏数据 */
    *returnSize = 0;

    /* 参数合法性检查：空指针或无意义的长度直接返回 NULL */
    if (nums == NULL || returnSize == NULL || numsSize < 2) {
        return NULL;
    }

    /*
     * malloc 分配内存:
     *   (size_t)numsSize : 把 int 显式转成 size_t（无符号整数类型），
     *                      size_t 是 malloc 期望的参数类型，避免警告。
     *   sizeof(NumberWithIndex) : 编译期计算该结构体占多少字节。
     *   (NumberWithIndex*) : malloc 返回 void*，需要强制转换。
     */
    NumberWithIndex* items =
        (NumberWithIndex*)malloc((size_t)numsSize * sizeof(NumberWithIndex));
    /* malloc 失败时返回 NULL，必须检查（尤其在嵌入式或内存紧张的场景） */
    if (items == NULL) {
        return NULL;
    }

    /* 把原数组的值和下标一起打包存起来 */
    for (int index = 0; index < numsSize; index++) {
        items[index].value         = nums[index];   /* 数值 */
        items[index].originalIndex = index;          /* 原始下标 */
    }

    /*
     * qsort 快速排序:
     *   参数1: 数组首地址
     *   参数2: 元素个数
     *   参数3: 每个元素的大小（字节）
     *   参数4: 比较函数指针
     */
    qsort(items, (size_t)numsSize, sizeof(NumberWithIndex), compareByValue);

    /* 遍历排序后的数组，为每个元素找搭档 */
    for (int index = 0; index < numsSize - 1; index++) {
        /*
         * 【溢出处理】
         * target - items[index].value 可能超出 int 范围（如 target = INT_MAX,
         * value = -1 时结果 > INT_MAX），所以先用 long long 接收。
         *
         * long long : C99 引入的至少 64 位整数类型，范围远超 int（通常 32 位）。
         */
        long long needed = (long long)target - items[index].value;

        /* 如果 needed 超出 int 能表示的范围，数组中肯定不存在，跳过 */
        if (needed < INT_MIN || needed > INT_MAX) {
            continue;
        }

        /* 二分查找：只在 index 右侧找（因为搭档不可能在左边，避免重复） */
        int match = binarySearch(items, index + 1, numsSize - 1, (int)needed);
        if (match != -1) {
            /* 找到搭档，分配返回数组 */
            int* result = (int*)malloc(2 * sizeof(int));
            if (result == NULL) {         /* 分配失败也要记得释放之前的内存 */
                free(items);              /* free 释放 items 占用的堆内存 */
                return NULL;
            }

            /* 填入找到的两个原始下标 */
            result[0] = items[index].originalIndex;
            result[1] = items[match].originalIndex;
            *returnSize = 2;

            /* items 的使命完成，释放后再返回结果 */
            free(items);
            return result;
        }
    }

    /* 遍历完没找到，释放内存，返回 NULL 表示无解 */
    free(items);
    return NULL;
}