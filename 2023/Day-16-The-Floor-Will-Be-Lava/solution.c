#define _GNU_SOURCE
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_INPUT_FILE "input.txt"
#define MAX_COLS_NUM (128)
#define MAX_ROWS_NUM (128)

#ifdef DEBUG
#define PUTS puts
#define PRINTF printf
#else
#define PUTS(a)                                                                \
  {}
#define PRINTF(fmt, ...)                                                       \
  {}
#endif // DEBUG

size_t rows_num = 0;
size_t cols_num = 0;
char contraption[MAX_ROWS_NUM][MAX_COLS_NUM] = {0};
int beams[MAX_ROWS_NUM][MAX_COLS_NUM] = {0};
/******************************************************************************/
#ifdef DEBUG
void print_data(void) {
  for (size_t row_idx = 0; row_idx < rows_num; ++row_idx) {
    printf("%s\n", contraption[row_idx]);
  }
} /* print_data() */
#else
#define print_data(_)                                                          \
  {}
#endif /* DEBUG */
/******************************************************************************/
size_t trim(char *s) {
  size_t i = 0;
  while (s[i] && (s[i] <= ' '))
    ++i;
  size_t j = 0;
  while (s[i])
    s[j++] = s[i++];
  s[j] = 0;
  while (j && (s[j] <= ' '))
    --j;
  s[++j] = 0;
  return j;
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
  while (true) {
    char *line = contraption[rows_num];
    if (!fgets(line, MAX_COLS_NUM, file))
      break;
    const size_t len = trim(line);
    if (!cols_num)
      cols_num = len;
    ++rows_num;
    PRINTF("'%s'\n", line);
  } /* loop over lines */
  fclose(file);
  PUTS("Input data were read.");
  return EXIT_SUCCESS;
} /* read_input_data(.) */
/******************************************************************************/
void solve_part1(void) {
  /* solution of the part1 */
  print_data();
  printf("%d\n", 0);
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
