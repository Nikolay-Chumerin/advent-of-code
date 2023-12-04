#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH (1024)
#define DEFAULT_INPUT_FILE "input.txt"

#define RED_CUBES_MAX_NUM (12)
#define GREEN_CUBES_MAX_NUM (13)
#define BLUE_CUBES_MAX_NUM (14)
char *s = NULL;

#define SKIP_SPACES                                                            \
  while (*s == ' ') {                                                          \
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

int get_game_power(char *str) {
  int max_nums[256] = {['r'] = 0, ['g'] = 0, ['b'] = 0};
  s = str + 5;
  SKIP_DIGITS;
  while (*s) {
    SKIP_NON_DIGITS;
    const int num = parse_int();
    SKIP_SPACES;
    const char color_first_char = *s;
    if (num > max_nums[color_first_char])
      max_nums[color_first_char] = num;
  }
  return max_nums['r'] * max_nums['g'] * max_nums['b'];
}

int main(int argc, char *argv[]) {
  FILE *file = fopen((argc > 1) ? argv[1] : DEFAULT_INPUT_FILE, "r");
  if (!file) {
    perror("Error opening the file");
    return EXIT_FAILURE;
  }
  char game_str[MAX_LINE_LENGTH];
  int ans = 0;
  while (fgets(game_str, sizeof(game_str), file)) {
    ans += get_game_power(game_str);
  } /* loop over lines */
  fclose(file);
  printf("%d\n", ans);
  return EXIT_SUCCESS;
}
