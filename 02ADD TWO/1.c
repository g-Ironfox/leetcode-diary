#include <stdlib.h>   // malloc / NULL 都在这里声明

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
    // 分配内存，并初始化为 0（避免 next 是野指针）
    struct ListNode *node = (struct ListNode *)malloc(sizeof(struct ListNode));
    node->val = 7;
    node->next = NULL;   // 关键！不指向任何节点就置 NULL
}