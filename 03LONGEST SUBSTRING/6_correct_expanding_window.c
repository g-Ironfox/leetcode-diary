#include <string.h>

int lengthOfLongestSubstring(char* s) {
    int last[128];               // ASCII 字符上次出现的位置
    memset(last, -1, sizeof(last));

    int maxLen = 0;
    int start = 0;               // 窗口左边界（下标）

    for (int end = 0; s[end] != '\0'; end++) {
        unsigned char c = s[end];

        // 如果该字符在当前窗口内出现过，收缩窗口
        if (last[c] >= start) {
            break;
        }

        // 更新答案：当前窗口长度 = end - start + 1
        int curLen = end - start + 1;
        if (curLen > maxLen) {
            maxLen = curLen;
        }

        last[c] = end;           // 记录当前字符的最新位置
    }

    return maxLen;
}
