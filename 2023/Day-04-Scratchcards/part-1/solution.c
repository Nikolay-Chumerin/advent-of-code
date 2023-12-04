#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH (1024)
#define DEFAULT_INPUT_FILE "input.txt"

#define RED_CUBES_MAX_NUM (12)
#define GREEN_CUBES_MAX_NUM (13)
#define BLUE_CUBES_MAX_NUM (14)
char *s = NULL;

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

int process(char *str) {
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

  const int ans = found_lucky_nums ? 1 << (found_lucky_nums - 1) : 0;
  return ans;
} /* process(.) */

int main(int argc, char *argv[]) {
  FILE *file = fopen((argc > 1) ? argv[1] : DEFAULT_INPUT_FILE, "r");
  if (!file) {
    perror("Error opening the file");
    return EXIT_FAILURE;
  }
  char line[MAX_LINE_LENGTH];
  int ans = 0;
  while (fgets(line, sizeof(line), file)) {
    trim(line);
    const int win_points = process(line);
    ans += win_points;
  } /* loop over lines */
  fclose(file);
  printf("%d\n", ans);
  return EXIT_SUCCESS;
}
