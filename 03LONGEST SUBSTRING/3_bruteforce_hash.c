int lengthOfLongestSubstring(char *s)
{
    int maxLength = 0;

    for (int left = 0; s[left] != '\0'; left++) {
        int seen[128] = {0};

        for (int right = left; s[right] != '\0'; right++) {
            unsigned char current = (unsigned char)s[right];
            if (seen[current]) {
                break;
            }

            seen[current] = 1;
            int currentLength = right - left + 1;
            if (currentLength > maxLength) {
                maxLength = currentLength;
            }
        }
    }

    return maxLength;
}
