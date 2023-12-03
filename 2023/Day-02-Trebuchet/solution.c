#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH (16384)
#define DEFAULT_INPUT_FILE "input.txt"

char *rstrstr(char *s1, const int s1len, const char *s2,
              const int s2len) {
  if (s2len > s1len)
    return NULL;
  
  for (char *s = s1 + s1len - s2len; s >= s1; --s)
    if (strncmp(s, s2, s2len) == 0)
      return s;
  return NULL;
}

int main(int argc, char *argv[]) {
  FILE *file = fopen((argc > 1) ? argv[1] : DEFAULT_INPUT_FILE, "r");
  if (!file) {
    perror("Error opening the file");
    return EXIT_FAILURE;
  }
  const char *digit_words[] = {"1", "one",   "2", "two",   "3", "three",
                               "4", "four",  "5", "five",  "6", "six",
                               "7", "seven", "8", "eight", "9", "nine"};
  const int digit_words_num = sizeof(digit_words) / sizeof(digit_words[0]);
  int digit_word_lens[digit_words_num];
  for (int i = 0; i < digit_words_num; ++i) {
    digit_word_lens[i] = strlen(digit_words[i]);
  }

  char line[MAX_LINE_LENGTH];
  int ans = 0;
  while (fgets(line, sizeof(line), file)) {
    char *p = line;
    const int line_len = strlen(line);
    int first_digit = -1, first_idx = MAX_LINE_LENGTH;
    int last_digit = -1, last_idx = -1;

    for (int i = 0; i < digit_words_num; ++i) {
      const int digit = 1 + i / 2;
      const int digit_word_len = strlen(digit_words[i]);
      const char *found_first = strstr(line, digit_words[i]);
      if (found_first) {
        const int idx = found_first - line;
        if (idx < first_idx) {
          first_idx = idx;
          first_digit = digit;
        }
      } /* found_first branch */

      const char *found_last =
          rstrstr(line, line_len, digit_words[i], digit_word_lens[i]);
      if (found_last) {
        const int idx = found_last - line;
        if (idx > last_idx) {
          last_idx = idx;
          last_digit = digit;
        }
      } /* found_last branch */

    } /* loop over digit_words */

    const int num = (first_digit >= 0 && last_digit >= 0)
                        ? first_digit * 10 + last_digit
                        : 0;
    ans += num;
  } /* loop over lines */
  fclose(file);
  printf("%d\n", ans);
  return EXIT_SUCCESS;
}
