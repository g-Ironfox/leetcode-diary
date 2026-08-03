#include <limits.h>
#include <stdlib.h>

typedef struct {
    int value;
    int originalIndex;
} NumberWithIndex;

static int compareByValue(const void* left, const void* right) {
    const NumberWithIndex* leftItem = (const NumberWithIndex*)left;
    const NumberWithIndex* rightItem = (const NumberWithIndex*)right;

    if (leftItem->value < rightItem->value) {
        return -1;
    }
    if (leftItem->value > rightItem->value) {
        return 1;
    }
    return 0;
}

static int binarySearch(const NumberWithIndex* items, int left, int right, int target) {
    while (left <= right) {
        int middle = left + (right - left) / 2;

        if (items[middle].value == target) {
            return middle;
        }
        if (items[middle].value < target) {
            left = middle + 1;
        } else {
            right = middle - 1;
        }
    }

    return -1;
}

int* twoSum(int* nums, int numsSize, int target, int* returnSize) {
    *returnSize = 0;
    if (nums == NULL || returnSize == NULL || numsSize < 2) {
        return NULL;
    }

    NumberWithIndex* items =
        (NumberWithIndex*)malloc((size_t)numsSize * sizeof(NumberWithIndex));
    if (items == NULL) {
        return NULL;
    }

    for (int index = 0; index < numsSize; index++) {
        items[index].value = nums[index];
        items[index].originalIndex = index;
    }

    qsort(items, (size_t)numsSize, sizeof(NumberWithIndex), compareByValue);

    for (int index = 0; index < numsSize - 1; index++) {
        long long needed = (long long)target - items[index].value;
        if (needed < INT_MIN || needed > INT_MAX) {
            continue;
        }

        int match = binarySearch(items, index + 1, numsSize - 1, (int)needed);
        if (match != -1) {
            int* result = (int*)malloc(2 * sizeof(int));
            if (result == NULL) {
                free(items);
                return NULL;
            }

            result[0] = items[index].originalIndex;
            result[1] = items[match].originalIndex;
            *returnSize = 2;
            free(items);
            return result;
        }
    }

    free(items);
    return NULL;
}