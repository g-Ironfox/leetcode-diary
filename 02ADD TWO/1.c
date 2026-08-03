#include <stdlib.h>   // malloc / NULL 都在这里声明

// 以下 struct ListNode 仅供本地编译测试
// 提交到 LeetCode 时请删除，因为平台已自动定义该结构体，否则会报 redefinition 错误。
struct ListNode {
    int val;             // 存一位数字
    struct ListNode *next;  // 指向下一个节点
};

/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     struct ListNode *next;
 * };
 */
struct ListNode* addTwoNumbers(struct ListNode* l1, struct ListNode* l2) {
    /*
     * 标准解法：模拟小学竖式加法。
     * 两个链表本来就是"逆序存储"（个位在头），所以从头到尾遍历，
     * 天然就是从个位往高位加，不需要反转。
     *
     * 三个要点：
     * 1) dummy（哨兵）节点：避免处理"第一个节点"的特殊情况，
     *    最后返回 dummy->next 即可。
     * 2) carry（进位）：sum = 两个数位 + 进位，carry = sum / 10，
     *    当前位存 sum % 10。两个节点都没有且进位为 0 时结束。
     * 3) 两个链表可能长度不同：短的遍历完就当它的位是 0。
     */
    struct ListNode *dummy = (struct ListNode *)malloc(sizeof(struct ListNode));
    dummy->next = NULL;              // 哨兵节点，不存数据
    struct ListNode *cur = dummy;    // cur 永远指向"正在拼接的最后一个节点"
    int carry = 0;                   // 进位，初始为 0

    // 循环条件：只要还有一个数位没加完，或者还有进位，就继续
    while (l1 != NULL || l2 != NULL || carry != 0) {
        int sum = carry;             // 先加上一次循环的进位
        if (l1 != NULL) {
            sum += l1->val;          // 取 l1 当前位
            l1 = l1->next;           // 指针后移
        }
        if (l2 != NULL) {
            sum += l2->val;          // 取 l2 当前位
            l2 = l2->next;
        }

        carry = sum / 10;            // 进位给下一位
        cur->next = (struct ListNode *)malloc(sizeof(struct ListNode));
        cur = cur->next;             // 让 cur 指向新节点
        cur->val = sum % 10;         // 当前位只存个位数
        cur->next = NULL;            // 关键！新节点 next 必须置 NULL
    }

    struct ListNode *result = dummy->next;  // 跳过哨兵节点
    free(dummy);                            // 释放哨兵，避免内存泄漏
    return result;
}