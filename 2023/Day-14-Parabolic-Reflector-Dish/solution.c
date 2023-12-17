#define _GNU_SOURCE
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRING_CAPACITY (128)
#define DEFAULT_INPUT_FILE "input.txt"
#define ROWS_MAX_NUM (128)
#define COLS_MAX_NUM (128)

#ifdef DEBUG
#define PUTS puts
#define PRINTF printf
#else
#define PUTS(a)                                                                \
  {}
#define PRINTF(fmt, ...)                                                       \
  {}
#endif // DEBUG

char dish[ROWS_MAX_NUM][COLS_MAX_NUM] = {0};
char dish1[ROWS_MAX_NUM][COLS_MAX_NUM] = {0};
size_t rows_num = 0;
size_t cols_num = 0;

/******************************************************************************/
#ifdef DEBUG
void print_dish(void) {
  for (size_t i = 0; i < rows_num; ++i) {
    puts(dish[i]);
  } /* Loop over rows */
} /* print_dish() */
#else
#define print_dish(_)                                                          \
  {}
#endif /* DEBUG */
/******************************************************************************/
void trim(char *line) {
  int line_len = strlen(line);
  while (line_len && (line[line_len - 1] <= ' ')) {
    line[--line_len] = 0;
  }
} /* trim(.) */
/******************************************************************************/
int read_input_data(const char *input_file_path) {
  PRINTF("Reading input data from the '%s' file.\n", input_file_path);
  FILE *file = fopen(input_file_path, "r");
  if (!file) {
    perror("Error opening the file");
    return (EXIT_FAILURE);
  }
  rows_num = 0;
  cols_num = 0;
  while (true) {
    char *line = &dish[rows_num][0];
    if (!fgets(line, COLS_MAX_NUM, file))
      break;
    trim(line);
    ++rows_num;
    PRINTF("'%s'\n", line);
  } /* loop over lines */
  cols_num = strlen(&dish[0][0]);
  fclose(file);
  PUTS("Input data were read.");
  return EXIT_SUCCESS;
} /* read_input_data(.) */
/******************************************************************************/
void tilt_dish_north(void) {
  for (size_t col_idx = 0; col_idx < cols_num; ++col_idx) {
    size_t j = 0;
    for (size_t row_idx = 0; row_idx < rows_num; ++row_idx) {
      const char c = dish[row_idx][col_idx];
      if ('.' == c)
        continue;
      if ('O' == c) {
        dish[row_idx][col_idx] = '.';
        while ('.' != dish[j][col_idx])
          ++j;
        dish[j++][col_idx] = c;
      } else { /* '#' == c */
        j = row_idx + 1;
      }
    } /* loop over rows */
  }   /* loop over columns */
} /* tilt_dish_north() */
/******************************************************************************/
void tilt_dish_south(void) {
  for (size_t col_idx = 0; col_idx < cols_num; ++col_idx) {
    size_t j = rows_num - 1;
    for (int row_idx = j; row_idx >= 0; --row_idx) {
      const char c = dish[row_idx][col_idx];
      if ('.' == c)
        continue;
      if ('O' == c) {
        dish[row_idx][col_idx] = '.';
        while (j && '.' != dish[j][col_idx])
          --j;
        dish[j--][col_idx] = c;
      } else { /* '#' == c */
        j = row_idx - 1;
      }
    } /* loop over rows */
  }   /* loop over columns */
} /* tilt_dish_south() */
/******************************************************************************/
void tilt_dish_west(void) {
  for (size_t row_idx = 0; row_idx < rows_num; ++row_idx) {
    size_t j = 0;
    for (size_t col_idx = 0; col_idx < cols_num; ++col_idx) {
      const char c = dish[row_idx][col_idx];
      if ('.' == c)
        continue;
      if ('O' == c) {
        dish[row_idx][col_idx] = '.';
        while ('.' != dish[row_idx][j])
          ++j;
        dish[row_idx][j++] = c;
      } else { /* '#' == c */
        j = col_idx + 1;
      }
    } /* loop over columns */
  }   /* loop over rows */
} /* tilt_dish_west() */
/******************************************************************************/
void tilt_dish_east(void) {
  for (size_t row_idx = 0; row_idx < rows_num; ++row_idx) {
    size_t j = cols_num - 1;
    for (int col_idx = j; col_idx >= 0; --col_idx) {
      const char c = dish[row_idx][col_idx];
      if ('.' == c)
        continue;
      if ('O' == c) {
        dish[row_idx][col_idx] = '.';
        while (j && '.' != dish[row_idx][j])
          ++j;
        dish[row_idx][j--] = c;
      } else { /* '#' == c */
        j = col_idx - 1;
      }
    } /* loop over columns */
  }   /* loop over rows */
} /* tilt_dish_east() */
/******************************************************************************/
int calculate_north_beam_load(void) {
  int ans = 0;
  for (size_t row_idx = 0; row_idx < rows_num; ++row_idx) {
    int row_sum = 0;
    const char *row = dish[row_idx];
    for (size_t col_idx = 0; col_idx < cols_num; ++col_idx) {
      row_sum += ('O' == row[col_idx]);
    } /* loop over columns */
    ans += (rows_num - row_idx) * row_sum;
  } /* loop over rows */
  return ans;
} /* calculate_north_beam_load() */
/******************************************************************************/
void make_cycle(void) {
  tilt_dish_north();
  tilt_dish_west();
  tilt_dish_south();
  tilt_dish_east();
}
/******************************************************************************/
void solve_part1(void) {
  /* solution of the part1 */
  PUTS("--------------------");
  tilt_dish_north();
  print_dish();
  int ans = calculate_north_beam_load();
  printf("%d\n", ans);
} /* solve_part1() */
/******************************************************************************/
void solve_part2(void) {
  size_t it;

  for (it = 0; it < 100; ++it) {
    make_cycle();
  }
  size_t it1 = it;
  memcpy(dish1, dish, sizeof(dish));
  do {
    make_cycle();
    ++it;
    PRINTF("\e[1;1H\e[2J");
    print_dish();
    PRINTF("it=%lu\n", it);
  } while (memcmp(dish, dish1, sizeof(dish)));
  const size_t period = it - it1;
  PRINTF("Cycle detected: %lu\n", period);
  const size_t target = 1000000000;
  const size_t iterations_remains = (target - it) % period;
  for (it = 0; it < iterations_remains; ++it) {
    make_cycle();
  }
  int ans = calculate_north_beam_load();
  printf("%d\n", ans);
} /* solve_part2() */
/******************************************************************************/
int main(int argc, char *argv[]) {
  memset(dish, 0, sizeof(dish));
  memset(dish1, 0, sizeof(dish1));
  const char *input_file_path = (argc > 1) ? argv[1] : DEFAULT_INPUT_FILE;
  if (EXIT_SUCCESS != read_input_data(input_file_path)) {
    perror("Error reading input data from file");
    return (EXIT_FAILURE);
  }

  solve_part1();
  solve_part2();

  return EXIT_SUCCESS;
}
