/*
 * LeetCode 1. Two Sum - AVL 平衡二叉搜索树解法
 *
 * 思路：从左到右扫描 nums，AVL 树中只保存已经看过的元素。
 * 对 nums[i]：
 *   1. 在树中查找 target - nums[i]；
 *   2. 找到就返回树中保存的旧下标和 i；
 *   3. 找不到就把 (nums[i], i) 插入树中。
 *
 * AVL 树保证任意节点左右子树的高度差不超过 1，因此查找和插入
 * 最坏都是 O(log n)。整个算法时间 O(n log n)，额外空间 O(n)。
 */

#include <limits.h>
#include <stdlib.h>

typedef struct AvlNode {
    int key;                 /* nums 中的值，也是二叉搜索树的排序键 */
    int originalIndex;       /* 该值第一次出现时的原数组下标 */
    int height;              /* 以当前节点为根的子树高度 */
    struct AvlNode* left;    /* key 更小的节点 */
    struct AvlNode* right;   /* key 更大的节点 */
} AvlNode;

static int nodeHeight(const AvlNode* node) {
    return node == NULL ? 0 : node->height;
}

static int maxInt(int left, int right) {
    return left > right ? left : right;
}

/* 平衡因子 = 左子树高度 - 右子树高度；AVL 要求它只能是 -1、0、1。 */
static int balanceFactor(const AvlNode* node) {
    return nodeHeight(node->left) - nodeHeight(node->right);
}

static void updateHeight(AvlNode* node) {
    node->height = 1 + maxInt(nodeHeight(node->left), nodeHeight(node->right));
}

/*
 *       root                 newRoot
 *       /  \                 /    \
 * newRoot  C    右旋后       A     root
 *   / \                         /  \
 *  A   B                       B    C
 */
static AvlNode* rotateRight(AvlNode* root) {
    AvlNode* newRoot = root->left;
    AvlNode* movedSubtree = newRoot->right;

    newRoot->right = root;
    root->left = movedSubtree;

    /* 先更新降下去的旧根，再更新升上来的新根。 */
    updateHeight(root);
    updateHeight(newRoot);
    return newRoot;
}

/*
 * root                         newRoot
 * /  \                         /    \
 * A  newRoot     左旋后       root    C
 *      / \                    /  \
 *     B   C                  A    B
 */
static AvlNode* rotateLeft(AvlNode* root) {
    AvlNode* newRoot = root->right;
    AvlNode* movedSubtree = newRoot->left;

    newRoot->left = root;
    root->right = movedSubtree;

    updateHeight(root);
    updateHeight(newRoot);
    return newRoot;
}

/* 普通二叉搜索树查找；AVL 的高度是 O(log n)，所以查找也是 O(log n)。 */
static const AvlNode* findNode(const AvlNode* root, int key) {
    while (root != NULL) {
        if (key == root->key) {
            return root;
        }
        root = key < root->key ? root->left : root->right;
    }
    return NULL;
}

/*
 * 递归插入后，从递归路径向上更新高度并做旋转。
 * allocationFailed 是输出参数：malloc 失败时通知最外层停止处理。
 */
static AvlNode* insertNode(AvlNode* root, int key, int originalIndex,
                           int* allocationFailed) {
    if (root == NULL) {
        AvlNode* node = (AvlNode*)malloc(sizeof(AvlNode));
        if (node == NULL) {
            *allocationFailed = 1;
            return NULL;
        }
        node->key = key;
        node->originalIndex = originalIndex;
        node->height = 1;
        node->left = NULL;
        node->right = NULL;
        return node;
    }

    if (key < root->key) {
        AvlNode* newLeft = insertNode(root->left, key, originalIndex,
                                      allocationFailed);
        if (*allocationFailed) {
            return root;
        }
        root->left = newLeft;
    } else if (key > root->key) {
        AvlNode* newRight = insertNode(root->right, key, originalIndex,
                                       allocationFailed);
        if (*allocationFailed) {
            return root;
        }
        root->right = newRight;
    } else {
        /* 相同的值无需重复建节点；保留第一次出现的下标即可。 */
        return root;
    }

    updateHeight(root);
    int balance = balanceFactor(root);

    /* LL：新节点插到了左孩子的左侧，做一次右旋。 */
    if (balance > 1 && key < root->left->key) {
        return rotateRight(root);
    }
    /* RR：新节点插到了右孩子的右侧，做一次左旋。 */
    if (balance < -1 && key > root->right->key) {
        return rotateLeft(root);
    }
    /* LR：先把左孩子左旋，转成 LL，再把根右旋。 */
    if (balance > 1 && key > root->left->key) {
        root->left = rotateLeft(root->left);
        return rotateRight(root);
    }
    /* RL：先把右孩子右旋，转成 RR，再把根左旋。 */
    if (balance < -1 && key < root->right->key) {
        root->right = rotateRight(root->right);
        return rotateLeft(root);
    }

    return root;
}

static void freeTree(AvlNode* root) {
    if (root == NULL) {
        return;
    }
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

int* twoSum(int* nums, int numsSize, int target, int* returnSize) {
    if (returnSize == NULL) {
        return NULL;
    }
    *returnSize = 0;
    if (nums == NULL || numsSize < 2) {
        return NULL;
    }

    AvlNode* root = NULL;

    for (int index = 0; index < numsSize; index++) {
        /* 先扩大到 long long 做减法，避免有符号 int 溢出。 */
        long long needed = (long long)target - nums[index];
        if (needed >= INT_MIN && needed <= INT_MAX) {
            const AvlNode* match = findNode(root, (int)needed);
            if (match != NULL) {
                int* result = (int*)malloc(2 * sizeof(int));
                if (result == NULL) {
                    freeTree(root);
                    return NULL;
                }
                result[0] = match->originalIndex;
                result[1] = index;
                *returnSize = 2;
                freeTree(root);
                return result;
            }
        }

        int allocationFailed = 0;
        root = insertNode(root, nums[index], index, &allocationFailed);
        if (allocationFailed) {
            freeTree(root);
            return NULL;
        }
    }

    freeTree(root);
    return NULL;
}