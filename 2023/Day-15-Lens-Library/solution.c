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

/******************************************************************************/
#ifdef DEBUG
void print_data(void) {} /* print_data() */
#else
#define print_data(_)                                                          \
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
  while (true) {
    // // char *line = &dish[rows_num][0];
    // if (!fgets(line, COLS_MAX_NUM, file))
    //   break;
    // trim(line);
    // ++rows_num;
    // PRINTF("'%s'\n", line);
  } /* loop over lines */
  // cols_num = strlen(&dish[0][0]);
  fclose(file);
  PUTS("Input data were read.");
  return EXIT_SUCCESS;
} /* read_input_data(.) */
/******************************************************************************/
void solve_part1(void) {
  /* solution of the part1 */
  printf("%d\n", 0);
} /* solve_part1() */
/******************************************************************************/
void solve_part2(void) { printf("%d\n", 0); } /* solve_part2() */
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
