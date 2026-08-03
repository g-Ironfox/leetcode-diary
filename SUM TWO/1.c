#include <stdlib.h>
#include <limits.h>

/* ============================================================
   Two Sum — 纯 C 实现（HashMap 解法，开放寻址）
   LeetCode 格式：int* twoSum(int* nums, int numsSize,
                              int target, int* returnSize)
   ============================================================ */

typedef struct {
    int key;
    int value;
} HashEntry;

typedef struct {
    HashEntry* entries;
    int capacity;
} HashMap;

HashMap* createHashMap(int capacity) {
    HashMap* map = (HashMap*)malloc(sizeof(HashMap));
    map->entries = (HashEntry*)calloc(capacity, sizeof(HashEntry));
    map->capacity = capacity;
    for (int i = 0; i < capacity; i++) {
        map->entries[i].key = INT_MIN;  /* 标记为空 */
    }
    return map;
}

void hashMapPut(HashMap* map, int key, int value) {
    int idx = (key % map->capacity + map->capacity) % map->capacity;
    while (map->entries[idx].key != INT_MIN && map->entries[idx].key != key) {
        idx = (idx + 1) % map->capacity;
    }
    map->entries[idx].key   = key;
    map->entries[idx].value = value;
}

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

void freeHashMap(HashMap* map) {
    free(map->entries);
    free(map);
}

/**
 * Note: The returned array must be malloced, assume caller calls free().
 */
int* twoSum(int* nums, int numsSize, int target, int* returnSize) {
    HashMap* map = createHashMap(numsSize * 2 + 1);

    for (int i = 0; i < numsSize; i++) {
        int need    = target - nums[i];
        int prevIdx = 0;

        if (hashMapGet(map, need, &prevIdx)) {
            int* result = (int*)malloc(2 * sizeof(int));
            result[0] = prevIdx;
            result[1] = i;
            *returnSize = 2;
            freeHashMap(map);
            return result;
        }

        hashMapPut(map, nums[i], i);
    }

    *returnSize = 0;
    freeHashMap(map);
    return NULL;
}
