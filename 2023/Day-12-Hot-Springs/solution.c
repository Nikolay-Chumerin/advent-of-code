#define _GNU_SOURCE
#include <stdbool.h>
// #include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRING_CAPACITY (1024)
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

#define ARR_CAPACITY (1024)
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
row_t row;

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

  char *p = row.springs.data;
  int group_idx = -1;
  int group_size = 0;
  char prev = '.';
  for (size_t i = 0; i < spring_idx; ++i) {
    const char c = row.springs.data[i];
    if ('#' == c && '#' != prev) {
      ++group_idx;
      if (group_size || (group_idx >= row.groups.size))
        return 0;
      group_size = (int)row.groups.data[group_idx];
    }

    if ('#' == c)
      --group_size;

    if (group_size < 0) {
      return 0;
    }
    prev = c;
  }

  while ((spring_idx < row.springs.size) &&
         ('?' != row.springs.data[spring_idx]))
    ++spring_idx;

  if (spring_idx < row.springs.size && '?' == row.springs.data[spring_idx]) {
    size_t ans = 0U;
    size_t cnt;

    row.springs.data[spring_idx] = '#';
    cnt = count(spring_idx + 1);
    ans += cnt;
    if (cnt) {
      PRINTF("sprint_idx=%-3lu cnt=%-5lu ans=%-4lu ", spring_idx, cnt, ans);
      print_row(&row);
    }

    // row.springs.data[spring_idx] = '.';
    p = row.springs.data + spring_idx;
    *p = '.';

    cnt = count(spring_idx + 1);
    ans += cnt;
    if (cnt) {
      PRINTF("sprint_idx=%-3lu cnt=%-5lu ans=%-4lu ", spring_idx, cnt, ans);
      print_row(&row);
    }
    row.springs.data[spring_idx] = '?';
    return ans;
  }

  p = row.springs.data;
  for (size_t group_idx = 0U; group_idx < row.groups.size; ++group_idx) {
    int group = row.groups.data[group_idx];
    while (*p && '#' != *p)
      ++p;
    while (group && '#' == *p) {
      ++p;
      --group;
    }
    if (group || ('#' == *p))
      return 0;
  }

  while (*p) {
    if ('#' == *p++)
      return 0;
  }

  return 1;
} /* count(..) */
/******************************************************************************/
void solve_part1(void) {
  size_t ans = 0U;
  for (size_t i = 0U; i < rows_num; ++i) {
    row = rows[i];
    const size_t num = count(0U);
    ans += num;
  }
  printf("%lu\n", ans);
} /* solve_part1() */
/******************************************************************************/
void unfold_array(const size_t times, array_t *arr) {
  const size_t beg = arr->size;
  const size_t end = beg * times;
  const size_t dif = beg;
  for (size_t j = beg; j < end; ++j)
    arr->data[j] = arr->data[j - dif];
  arr->data[end] = 0;
  arr->size *= times;
} /* unfold_array(..) */
/******************************************************************************/
void unfold_rows(const size_t times) {
  for (size_t i = 0LU; i < rows_num; ++i) {
    rows[i].springs.data[rows[i].springs.size++] = '?';
    unfold_array(times, &rows[i].springs);
    rows[i].springs.data[rows[i].springs.size-1] = '\0';
    unfold_array(times, &rows[i].groups);
  } /* loop over rows */
} /* unfold_rows(.) */
/******************************************************************************/
void solve_part2(void) {
  unfold_rows(5LU);
  size_t ans = 0;
  for (size_t i = 0; i < rows_num; ++i) {
    row = rows[i];
    const size_t num = count(0);
    ans += num;
    printf("i=%04lu %lu\n", i, ans);
  }
  printf("%lu\n", ans);
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
