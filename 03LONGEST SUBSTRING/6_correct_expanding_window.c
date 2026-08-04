int lengthOfLongestSubstring(char *s)
{
    int maxLength = 0;

    for (int left = 0; s[left] != '\0'; left++) {
        int seen[128];
        memset(seen, -1, sizeof(seen));
        for (int right = left; s[right] != '\0'; right++) {

            if (seen[(unsigned char)s[right]] != -1) {
                break;
            }

            seen[(unsigned char)s[right]] = right;
            int currentLength = right - left + 1;
            if (currentLength > maxLength) {
                maxLength = currentLength;
            }
        }
    }

    return maxLength;
}
