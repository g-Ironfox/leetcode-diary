int lengthOfLongestSubstring(char *s)
{
    int maxLength = 0;

    for (int left = 0; s[left] != '\0'; left++) {

        for (int right = left; s[right] != '\0'; right++) {
            /* 检查 s[right] 是否与窗口 [left, right-1] 内的某个字符重复 */
            int duplicate = 0;
            for (int k = left; k < right; k++) {
                if (s[k] == s[right]) {
                    duplicate = 1;
                    break;
                }
            }
            if (duplicate) {
                break;
            }

            int currentLength = right - left + 1;
            if (currentLength > maxLength) {
                maxLength = currentLength;
            }
        }
    }

    return maxLength;
}
