int lengthOfLongestSubstring(char *s)
{
    int maxLength = 0;

    for (int left = 0; s[left] != '\0'; left++) {

        for (int right = left; s[right] != '\0'; right++) {

            if (s[left] == s[right]) {
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
