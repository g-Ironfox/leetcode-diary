#include <stdlib.h>

// ⚠️ 以下 struct ListNode 仅供本地编译测试！
// 提交到 LeetCode 时请删除，平台已自动定义，否则报 redefinition 错误。
struct ListNode {
    int val;
    struct ListNode *next;
};

/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     struct ListNode *next;
 * };
 */

/*
 * 优化版：复用 l1 链表原地存储结果，几乎不调用 malloc。
 *
 * 核心思路：
 *   把 l1 当作"结果链表"，直接在 l1 的节点上修改 val。
 *   l1 不够长时，把 l2 剩余部分"接"到 l1 尾部，继续修改。
 *   只有 l1 和 l2 都走完但还有进位时，才 malloc 一个新节点。
 *
 * 时间复杂度 O(max(n, m))，空间复杂度 O(1)（不计返回链表）。
 */
struct ListNode* addTwoNumbers(struct ListNode* l1, struct ListNode* l2) {
    struct ListNode *head = l1;  // 结果链表的头，最终返回它
    struct ListNode *prev = NULL; // 指向 l1 的最后一个节点，用于"接"l2
    int carry = 0;

    // 第一阶段：l1 和 l2 都有节点，直接在 l1 上改
    while (l1 != NULL && l2 != NULL) {
        int sum = l1->val + l2->val + carry;
        l1->val = sum % 10;
        carry = sum / 10;
        prev = l1;
        l1 = l1->next;
        l2 = l2->next;
    }

    // 第二阶段：如果 l2 比 l1 长，把 l2 剩余部分接到 l1 尾部
    if (l2 != NULL) {
        prev->next = l2;  // 接上 l2 剩余节点（省掉 malloc！）
        l1 = l2;          // 现在 l1 指向的就是接过来的 l2 节点
    }

    // 第三阶段：处理剩余节点（来自较长的那个链表）
    while (l1 != NULL && carry != 0) {
        int sum = l1->val + carry;
        l1->val = sum % 10;
        carry = sum / 10;
        prev = l1;
        l1 = l1->next;
    }

    // 第四阶段：两个链表都走完了，但还有进位 → 这是唯一需要 malloc 的地方
    if (carry != 0) {
        prev->next = (struct ListNode *)malloc(sizeof(struct ListNode));
        prev->next->val = carry;
        prev->next->next = NULL;
    }

    return head;
}
