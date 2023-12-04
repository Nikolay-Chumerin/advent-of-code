#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH (16384)
#define DEFAULT_INPUT_FILE "input.txt"

typedef bool char_lut[256];
typedef char three_lines_t[3][MAX_LINE_LENGTH];
char_lut is_symbol, is_digit;

three_lines_t lines;
int line_len;
int prev_line_idx = 0;
int curr_line_idx = 1;
int next_line_idx = 2;
char *prev_line;
char *curr_line;
char *next_line;

/******************************************************************************/
void setup_char_luts(void) {
  memset(is_symbol, true, sizeof(is_symbol));
  is_symbol['.'] = is_symbol['0'] = is_symbol['1'] = is_symbol['2'] =
      is_symbol['3'] = is_symbol['4'] = is_symbol['5'] = is_symbol['6'] =
          is_symbol['7'] = is_symbol['8'] = is_symbol['9'] = false;

  memset(is_digit, false, sizeof(is_symbol));
  is_digit['0'] = is_digit['1'] = is_digit['2'] = is_digit['3'] =
      is_digit['4'] = is_digit['5'] = is_digit['6'] = is_digit['7'] =
          is_digit['8'] = is_digit['9'] = true;
} /* setup_char_luts() */
/******************************************************************************/
void update_line_pointers(void) {
  prev_line = lines[prev_line_idx];
  curr_line = lines[curr_line_idx];
  next_line = lines[next_line_idx];
} /* update_line_pointers() */
/******************************************************************************/
void setup_char_lines(void) {
  memset(lines[prev_line_idx], '.', MAX_LINE_LENGTH);
  lines[curr_line_idx][0] = lines[next_line_idx][0] = '.';
  update_line_pointers();
} /* setup_char_lines() */
/******************************************************************************/
void advance_line_indices(void) {
  prev_line_idx = (prev_line_idx < 2) ? prev_line_idx + 1 : 0;
  curr_line_idx = (curr_line_idx < 2) ? curr_line_idx + 1 : 0;
  next_line_idx = (next_line_idx < 2) ? next_line_idx + 1 : 0;
} /* advance_line_indices() */
/******************************************************************************/
void pad_line(char *line) {
  line[line_len + 1] = '.';
  line[line_len + 2] = 0;
} /* pad_line(.) */
/**********************/
void pad_lines(void) {
  for (int i = 0; i < 3; ++i)
    pad_line(lines[i]);
} /* pad_lines(.) */
/******************************************************************************/
bool read_next_line(FILE *file) {
  const char *read_line =
      fgets(lines[next_line_idx] + 1, MAX_LINE_LENGTH, file);
  if (!read_line) {
    memset(lines[next_line_idx], '.', line_len + 1);
  }
  return read_line;
} /* read_next_line(.) */
/******************************************************************************/
int estimate_curr_line_len() {
  int len = strlen(lines[curr_line_idx]);
  while (lines[curr_line_idx][len] < ' ')
    --len;
  return len;
} /* estimate_curr_line_len() */
/******************************************************************************/
int seek_and_erase_num(char *line, const int char_idx) {
  if (!is_digit[line[char_idx]])
    return -1;
  int i = char_idx;
  while (i && is_digit[line[i]]) {
    if (!is_digit[line[i - 1]]) break;
    --i;
  }
  int num = 0;
  while (i && is_digit[line[i]]) {
    num = 10 * num + (line[i] - '0');
    line[i++] = '.';
  }
  return num;
} /* seek_and_erase_num() */
/******************************************************************************/
int get_adjacent_nums_product(const int gear_char_idx) {
  three_lines_t lines_backup;
  memcpy(lines_backup, lines, sizeof(lines)); /* backup lines */
  int nums[2] = {-1, -1};
  int nums_found = 0;

  for (int line_idx = 0; line_idx < 3; ++line_idx) {
    char *line = lines[line_idx];
    for (int i = -1; i <= 1; ++i) {
      const int char_idx = gear_char_idx + i;
      const int num = seek_and_erase_num(line, char_idx);
      if (num >= 0) {
        nums[nums_found++] = num;
      }
    } /* loop over chars */
  }   /* loop over lines */

  memcpy(lines, lines_backup, sizeof(lines)); /* restore lines */
  return (nums_found < 2) ? 0 : nums[0] * nums[1];
} /* get_adjacent_nums_product() */
/******************************************************************************/
int main(int argc, char *argv[]) {
  FILE *file = fopen((argc > 1) ? argv[1] : DEFAULT_INPUT_FILE, "r");
  if (!file) {
    perror("Error opening the file");
    return EXIT_FAILURE;
  }

  setup_char_luts();
  setup_char_lines();

  if (!fgets(lines[curr_line_idx] + 1, MAX_LINE_LENGTH, file)) {
    perror("Error reading the file");
    return EXIT_FAILURE;
  }

  line_len = estimate_curr_line_len();
  pad_lines();

  int ans = 0;
  bool next_line_was_read = true;
  do {
    next_line_was_read = read_next_line(file);
    update_line_pointers();
    pad_line(next_line);
    for (int i = 1; i <= line_len + 1; ++i) {
      if ('*' != curr_line[i])
        continue;
      ans += get_adjacent_nums_product(i);
    } /* scan of current line*/
    advance_line_indices();
  } while (next_line_was_read);

  fclose(file);
  printf("%d\n", ans);
  return EXIT_SUCCESS;
}
