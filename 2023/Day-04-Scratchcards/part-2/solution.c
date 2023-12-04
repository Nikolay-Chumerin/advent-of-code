#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH (1024)
#define DEFAULT_INPUT_FILE "input.txt"

#define CARD_MAX_IDX (255)
char *s = NULL;

int card_nums[CARD_MAX_IDX + 1];
int cards_num = 0;

#define SKIP_WHITE_SPACES                                                      \
  while (*s && *s <= ' ') {                                                    \
    ++s;                                                                       \
  }

#define SKIP_LETTERS                                                           \
  while (((*s & ~0x20) >= 'A') && ((*s & ~0x20) <= 'Z')) {                     \
    ++s;                                                                       \
  }

#define SKIP_DIGITS                                                            \
  while ((*s >= '0') && (*s <= '9')) {                                         \
    ++s;                                                                       \
  }
#define SKIP_NON_DIGITS                                                        \
  while (*s && ((*s < '0') || (*s > '9'))) {                                   \
    ++s;                                                                       \
  }

int parse_int(void) {
  int num = 0;
  while ((*s >= '0') && (*s <= '9')) {
    num = 10 * num + (*s - '0');
    ++s;
  }
  return num;
}

void trim(char *line) {
  int line_len = strlen(line);
  while (line_len && (line[line_len - 1] <= ' ')) {
    line[--line_len] = 0;
  }
}

void process(char *str, const int card_idx) {
  bool lucky_nums_set[100];
  memset(lucky_nums_set, false, sizeof(lucky_nums_set));
  s = str;
  SKIP_NON_DIGITS;
  SKIP_DIGITS;
  SKIP_NON_DIGITS;
  do {
    const int lucky_num = parse_int();
    lucky_nums_set[lucky_num] = true;
    SKIP_WHITE_SPACES;
  } while (*s != '|');

  int found_lucky_nums = 0;
  SKIP_NON_DIGITS;
  do {
    const int num = parse_int();
    found_lucky_nums += lucky_nums_set[num];
    SKIP_WHITE_SPACES;
  } while (*s);

  if (!found_lucky_nums)
    return;

  int start_idx = card_idx + 1;
  if (start_idx > CARD_MAX_IDX)
    start_idx = CARD_MAX_IDX;

  int end_idx = start_idx + found_lucky_nums - 1;
  if (end_idx > CARD_MAX_IDX)
    end_idx = CARD_MAX_IDX;

  for (int i = start_idx; i <= end_idx; ++i)
    card_nums[i] += card_nums[card_idx];
} /* process(.) */

int main(int argc, char *argv[]) {
  FILE *file = fopen((argc > 1) ? argv[1] : DEFAULT_INPUT_FILE, "r");
  if (!file) {
    perror("Error opening the file");
    return EXIT_FAILURE;
  }

  memset(card_nums, 0, sizeof(card_nums));

  char line[MAX_LINE_LENGTH];
  cards_num = 0;
  while (fgets(line, sizeof(line), file)) {
    ++card_nums[++cards_num];
    trim(line);
    process(line, cards_num);
  } /* loop over lines */
  fclose(file);

  int ans = 0;
  for (int i = 0; i <= cards_num; ++i) {
    ans += card_nums[i];
  }
  printf("%d\n", ans);

  return EXIT_SUCCESS;
}
