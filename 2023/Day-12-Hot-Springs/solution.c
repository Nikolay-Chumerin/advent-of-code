#define _GNU_SOURCE
#include <stdbool.h>
// #include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRING_CAPACITY (256)
#define DEFAULT_INPUT_FILE "input.txt"

#ifdef DEBUG
#define PUTS puts
#define PRINTF printf
#else
#define PUTS(a)                                                                \
  {}
#define PRINTF(fmt, ...)                                                       \
  {}
#endif // DEBUG

#define ARR_CAPACITY (32)
#define ROWS_CAPACITY (1024)

typedef char val_t;
typedef val_t data_t[ARR_CAPACITY];

typedef struct {
  data_t data;
  size_t size;
} array_t;

typedef struct {
  array_t springs;
  array_t groups;
} row_t;

row_t rows[ROWS_CAPACITY];
size_t rows_num = 0U;
row_t *row;

/******************************************************************************/
void line2row(const char *line, row_t *out_row) {
  size_t i = 0U;
  while (*line != ' ') {
    out_row->springs.data[i++] = *line++;
  }
  out_row->springs.data[i] = 0;
  out_row->springs.size = i;
  ++line;
  int vals_read, chars_read, val;
  do {
    vals_read = sscanf(line, "%d %n", &val, &chars_read);
    if (vals_read != 1)
      break;
    out_row->groups.data[out_row->groups.size++] = (val_t)val;
    line += chars_read;
  } while (true);
} /* line2row(..) */
/******************************************************************************/
#ifdef DEBUG
void print_row(const row_t *row) {
  PRINTF("springs[%02ld]=[%s] ", row->springs.size, row->springs.data);
  PRINTF("groups[%02ld]=[ ", row->groups.size);
  for (size_t i = 0; i < row->groups.size; ++i) {
    PRINTF("%d ", (int)row->groups.data[i]);
  }
  PUTS("]");
} /* print_row(.) */
#else
#define print_row(_)                                                           \
  {}
#endif /* DEBUG */
/******************************************************************************/
void trim(char *line) {
  size_t line_len = strlen(line);
  while (line_len && (line[line_len - 1] <= ' ')) {
    line[--line_len] = 0;
  }
} /* trim(.) */
/******************************************************************************/
void replace_char(const char from, const char to, char *line) {
  while (*line) {
    if (from == *line)
      *line = to;
    ++line;
  }
} /* replace_char(...) */
/******************************************************************************/
int read_input_data(const char *input_file_path) {
  PRINTF("Reading input data from the '%s' file.\n", input_file_path);
  char line[STRING_CAPACITY];
  FILE *file = fopen(input_file_path, "r");
  if (!file) {
    perror("Error opening the file");
    return (EXIT_FAILURE);
  }
  rows_num = 0U;
  while (true) {
    if (!fgets(line, sizeof(line), file))
      break;
    trim(line);
    replace_char(',', ' ', line);
    line2row(line, &rows[rows_num++]);
    // PRINTF("'%s' -> ", line);
    print_row(&rows[rows_num - 1]);
  } /* loop over lines */
  fclose(file);
  PUTS("Input data were read.");
  return EXIT_SUCCESS;
} /* read_input_data(.) */
/******************************************************************************/
size_t count(size_t spring_idx) {
  // print_row(row);
  while ((spring_idx < row->springs.size) &&
         (row->springs.data[spring_idx] != '?'))
    ++spring_idx;

  if (spring_idx < row->springs.size) {
    size_t ans = 0U;
    size_t cnt;

    row->springs.data[spring_idx] = '#';
    cnt = count(spring_idx + 1);
    ans += cnt;
    if (cnt) {
      PRINTF("sprint_idx=%-3lu cnt=%-3lu ans=%-3lu ", spring_idx, cnt, ans);
      print_row(row);
    }

    row->springs.data[spring_idx] = '.';
    cnt = count(spring_idx + 1);
    ans += cnt;
    if (cnt) {
      PRINTF("sprint_idx=%-3lu cnt=%-3lu ans=%-3lu ", spring_idx, cnt, ans);
      print_row(row);
    }
    row->springs.data[spring_idx] = '?';
    return ans;
  }
  char *p = row->springs.data;
  for (size_t group_idx = 0U; group_idx < row->groups.size; ++group_idx) {
    int group = row->groups.data[group_idx];
    while ('.' == *p)
      ++p;
    while (*p && group && '#' == *p) {
      ++p;
      --group;
    }
    if (group || (*p == '#'))
      return 0U;
  }
  while (*p) {
    if ('.' != *p++)
      return 0U;
  }
  return 1U;
} /* count(..) */
/******************************************************************************/
void solve_part1(void) {
  /* solution of the part1 */
  size_t ans = 0U;
  for (size_t i = 0U; i < rows_num; ++i) {
    row = rows + i;
    const size_t num = count(0U);
    ans += num;
    // print_row(row);
    // PRINTF("num=%lu  ans=%lu\n", num, ans);
  }
  printf("%lu\n", ans);
} /* solve_part1() */
/******************************************************************************/
void solve_part2(void) {
  /* solution of the part2 */
  printf("%d\n", 0);
} /* solve_part2() */
/******************************************************************************/
int main(int argc, char *argv[]) {
  const char *input_file_path = (argc > 1) ? argv[1] : DEFAULT_INPUT_FILE;
  if (EXIT_SUCCESS != read_input_data(input_file_path)) {
    perror("Error reading input data from file");
    return (EXIT_FAILURE);
  }

  solve_part1();
  solve_part2();

  return EXIT_SUCCESS;
}
