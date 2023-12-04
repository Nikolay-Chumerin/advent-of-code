#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH (16384)
#define DEFAULT_INPUT_FILE "input.txt"

#define PAD_LINE(s)                                                            \
  s[line_len + 1] = '.';                                                       \
  s[line_len + 2] = 0;

int main(int argc, char *argv[]) {
  FILE *file = fopen((argc > 1) ? argv[1] : DEFAULT_INPUT_FILE, "r");
  if (!file) {
    perror("Error opening the file");
    return EXIT_FAILURE;
  }

  bool is_symbol[256];
  memset(is_symbol, true, sizeof(is_symbol));
  is_symbol['.'] = is_symbol['0'] = is_symbol['1'] = is_symbol['2'] =
      is_symbol['3'] = is_symbol['4'] = is_symbol['5'] = is_symbol['6'] =
          is_symbol['7'] = is_symbol['8'] = is_symbol['9'] = false;

  char lines[3][MAX_LINE_LENGTH];
  int prev_line_idx = 0;
  int curr_line_idx = 1;
  int next_line_idx = 2;

  memset(lines[prev_line_idx], '.', MAX_LINE_LENGTH);
  lines[curr_line_idx][0] = lines[next_line_idx][0] = '.';

  if (!fgets(lines[curr_line_idx] + 1, MAX_LINE_LENGTH, file)) {
    perror("Error reading the file");
    return EXIT_FAILURE;
  }

  int line_len = strlen(lines[curr_line_idx]);
  while (lines[curr_line_idx][line_len] < ' ')
    --line_len;

  PAD_LINE(lines[prev_line_idx]);
  PAD_LINE(lines[curr_line_idx]);
  PAD_LINE(lines[next_line_idx]);

  int ans = 0;
  while (true) {
    const char *read_line =
        fgets(lines[next_line_idx] + 1, MAX_LINE_LENGTH, file);
    if (!read_line) {
      memset(lines[next_line_idx], '.', line_len + 1);
    }
    PAD_LINE(lines[next_line_idx]);

    char *s = lines[curr_line_idx];
    int num = 0;
    bool has_adjacent_symbol = false;

    const char *prev_line = lines[prev_line_idx];
    const char *curr_line = lines[curr_line_idx];
    const char *next_line = lines[next_line_idx];

    for (int i = 1; i <= line_len + 1; ++i) {
      const char c = curr_line[i];
      if (c >= '0' && c <= '9') {
        has_adjacent_symbol |=
            is_symbol[curr_line[i - 1]] || is_symbol[curr_line[i + 1]] ||
            is_symbol[prev_line[i - 1]] || is_symbol[prev_line[i]] ||
            is_symbol[prev_line[i + 1]] || is_symbol[next_line[i - 1]] ||
            is_symbol[next_line[i]] || is_symbol[next_line[i + 1]];
        num = num * 10 + (c - '0');
      } else {
        if (num) {
          if (has_adjacent_symbol) {
            ans += num;
            has_adjacent_symbol = false;
          }
          num = 0;
        }
      }
    } /* scan of current line*/

    if (!read_line)
      break;
    prev_line_idx = (prev_line_idx < 2) ? prev_line_idx + 1 : 0;
    curr_line_idx = (curr_line_idx < 2) ? curr_line_idx + 1 : 0;
    next_line_idx = (next_line_idx < 2) ? next_line_idx + 1 : 0;
  } /* loop over lines */
  fclose(file);
  printf("%d\n", ans);
  return EXIT_SUCCESS;
}
