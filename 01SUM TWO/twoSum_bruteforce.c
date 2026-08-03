/*
 * Two Sum — 纯 C 实现（暴力枚举，O(1) 额外空间）
 * LeetCode 格式：int* twoSum(int* nums, int numsSize,
 *                              int target, int* returnSize)
 *
 * ═══════════════════════════════════════════════════════════════
 *  空间最优解：不分配任何辅助结构，仅用两个循环变量。
 *
 *  与其他解法的对比：
 *  ┌────────────┬──────────┬──────────┬──────────┐
 *  │            │ 暴力枚举  │ HashMap  │ 排序+二分 │
 *  ├────────────┼──────────┼──────────┼──────────┤
 *  │ 额外空间   │ O(1)     │ O(n)     │ O(n)     │
 *  │ 时间复杂度 │ O(n²)    │ O(n)     │ O(nlogn) │
 *  └────────────┴──────────┴──────────┴──────────┘
 *
 *  本题 n ≤ 10⁴，最坏 5×10⁷ 次比较，C 语言在 LeetCode
 *  评测机上约 0.1~0.3 秒即可完成，不会超时。
 * ═══════════════════════════════════════════════════════════════
 */

#include <stdlib.h>   /* malloc, NULL */

/*
 * twoSum：枚举所有 i < j 的下标对，找到即返回。
 *
 * 额外空间分析：
 * - 只用到 i, j, need 三个栈上 int 变量（共 12 字节）。
 * - result 数组是 LeetCode 接口强制要求的返回值，
 *   任何解法都无法省去这 2 个 int（8 字节）。
 * - 除此以外，零额外堆分配。
 */
int* twoSum(int* nums, int numsSize, int target, int* returnSize) {
    for (int i = 0; i < numsSize; i++) {
        for (int j = i + 1; j < numsSize; j++) {
            if (nums[i] + nums[j] == target) {
                int* result = (int*)malloc(2 * sizeof(int));
                result[0] = i;
                result[1] = j;
                *returnSize = 2;
                return result;
            }
        }
    }

    /* 根据题目保证，不会走到这里 */
    *returnSize = 0;
    return NULL;
}
