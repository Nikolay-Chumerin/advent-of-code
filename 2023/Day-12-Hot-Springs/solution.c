#define _GNU_SOURCE
#include <stdbool.h>
// #include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRING_CAPACITY (1024)
#define DEFAULT_INPUT_FILE "test1.txt"

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
typedef ssize_t idx_t;
typedef val_t data_t[ARR_CAPACITY];

typedef struct {
  data_t data;
  idx_t size;
} array_t;

typedef struct {
  array_t springs;
  array_t groups;
} row_t;

row_t rows[ROWS_CAPACITY];
idx_t rows_num = 0U;
row_t row;

/******************************************************************************/
void line2row(const char *line, row_t *out_row) {
  idx_t i = 0;
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
typedef struct {
  idx_t spring_idx;
  idx_t group_idx;
  idx_t group_size;
  // idx_t damaged_num;
} state_t;
/****************************/
size_t count(void) {
  state_t stack[ARR_CAPACITY];
  idx_t stack_size = 0;
  size_t ans = 0;
  bool backtrack;
  idx_t damaged_target_num = 0;
  for (idx_t i = 0; i < row.groups.size; ++i)
    damaged_target_num += row.groups.data[i];

  idx_t damaged_num = 0;
  for (idx_t i = 0; i < row.springs.size; ++i)
    damaged_num += ('#' == row.springs.data[i]);

  state_t state = {
      .spring_idx = 0, .group_idx = -1, .group_size = 0}; //, .damaged_num = 0};
  while (true) {
    backtrack = false;
    /* Scan the current sping-array until next '?'.
     Backtrack, if it doesn't align with the groups. */
    bool prev_is_damaged = (state.spring_idx > 0) &&
                           ('#' == row.springs.data[state.spring_idx - 1]);
    while ((state.spring_idx < row.springs.size) &&
           '?' != row.springs.data[state.spring_idx]) {
      const bool spring_is_damaged =
          ('#' == row.springs.data[state.spring_idx]);
      if (spring_is_damaged && !prev_is_damaged) {
        ++state.group_idx;
        if (state.group_size || (state.group_idx >= row.groups.size)) {
          backtrack = true;
          break;
        }
        state.group_size = row.groups.data[state.group_idx];
      } /* new group found branch */

      state.group_size -= spring_is_damaged;
      if (state.group_size < 0) {
        backtrack = true;
        break;
      }
      prev_is_damaged = spring_is_damaged;
      ++state.spring_idx;
    } /* '?'-search loop */

    const bool too_many_damaged = (damaged_num > damaged_target_num);
    const bool all_springs_scanned = (state.spring_idx >= row.springs.size);
    if (all_springs_scanned || too_many_damaged) {
      backtrack = true;
    }
    const bool all_groups_fit =
        (state.group_size == 0) && (state.group_idx == (row.groups.size - 1));
    if (all_springs_scanned && all_groups_fit) {
      ++ans;
    }

    do {
      if (backtrack) {
        if (stack_size <= 0)
          return ans;
        state = stack[--stack_size];
      }

      switch (row.springs.data[state.spring_idx]) {
      case '?':
        row.springs.data[state.spring_idx] = '#';
        ++damaged_num;
        stack[stack_size++] = state;
        backtrack = false;
        break;
      case '#':
        row.springs.data[state.spring_idx] = '.';
        --damaged_num;
        stack[stack_size++] = state;
        backtrack = false;
        break;
      case '.':
        row.springs.data[state.spring_idx] = '?';
        backtrack = true;
        break;
      }
      PRINTF("spring_idx=%-6ld spring='%s' ans=%-6lu damaged_num=%-6ld\n",
             state.spring_idx, row.springs.data, ans, state.damaged_num);
      backtrack |= (damaged_num > damaged_target_num);
    } while (backtrack);
  } /* main loop  */
  return ans;
} /* count(..) */
/******************************************************************************/
void solve_part1(void) {
  size_t ans = 0U;
  for (idx_t i = 0; i < rows_num; ++i) {
    row = rows[i];
    const size_t num = count();
    ans += num;
    printf("i=%04lu num=%-10lu ans=%-10lu\n", i, num, ans);
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
  for (idx_t i = 0; i < rows_num; ++i) {
    rows[i].springs.data[rows[i].springs.size++] = '?';
    unfold_array(times, &rows[i].springs);
    rows[i].springs.data[rows[i].springs.size - 1] = '\0';
    unfold_array(times, &rows[i].groups);
  } /* loop over rows */
} /* unfold_rows(.) */
/******************************************************************************/
void solve_part2(void) {
  unfold_rows(5LU);
  size_t ans = 0;
  for (idx_t i = 0; i < rows_num; ++i) {
    row = rows[i];
    const size_t num = count();
    ans += num;
    printf("i=%04lu num=%-10lu ans=%-10lu\n", i, num, ans);
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
