#include <stdlib.h>

/*
 * 滑动窗口 + 双指针 (无哈希表版本)
 *
 * 核心：用两个指针 start 和 end 维护一个"无重复字符的窗口"，
 *       end 向右扩张，发现重复字符时 start 跳到重复位置的下一位。
 *
 * 指针基础回顾：
 *   char *s       — s 是指向字符串首字符的指针（等价于 &s[0]）
 *   *s            — 解引用：取出 s 指向的那个字符
 *   s + 1         — 指针算术：指向下一个字符（移动 sizeof(char) 字节）
 *   end - start   — 指针相减：得到两个指针之间的元素个数
 */

int lengthOfLongestSubstring(char* s) {
    // 两个指针都初始化为字符串首地址
    // start: 滑动窗口的左边界（包含）
    // end:   滑动窗口的右边界（包含）
    char *start = s, *end = s;
    int maxLength = 0;

    // *end 解引用，拿到 end 当前指向的字符
    // '\0' 是 C 字符串的结束标志（ASCII 0）
    // 当 *end == '\0' 时，说明已经扫描完整个字符串
    for (; *end != '\0'; end++) {  // end++ 让指针向后移动一个字符

        // temp = start 只是把地址赋给 temp，两个指针指向同一块内存
        char *temp = start;

        // temp < end 是指针比较：判断 temp 是否还没走到 end
        // 同一数组内的指针可以比较大小
        while (temp < end) {
            // *temp 和 *end 都是解引用，比较两个字符是否相等
            if (*temp == *end) {
                // temp + 1 是指针算术，指向 temp 后面的那个字符
                start = temp + 1;
                break;  // 找到第一个重复就够了，不需要继续找
            }
            temp++;  // temp 向后移动，继续扫描
        }

        // end - start 是指针相减，得到窗口内字符个数（不含 end）
        // + 1 把 end 自己也算进去 → 当前窗口长度
        int currentLength = end - start + 1;
        if (currentLength > maxLength) {
            maxLength = currentLength;
        }
    }
    return maxLength;
}