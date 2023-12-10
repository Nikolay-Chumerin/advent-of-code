#define _GNU_SOURCE
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRING_CAPACITY (256)
#define ROWS_MAX_NUM (256)
#define COLS_MAX_NUM (STRING_CAPACITY)
#define DEFAULT_INPUT_FILE "input.txt"

#define DEBUG
#ifdef DEBUG
#define PUTS puts
#define PRINTF printf
#else
#define PUTS(a) (0)
#define PRINTF(fmt, ...) (0)
#endif  // DEBUG

char maze[ROWS_MAX_NUM][COLS_MAX_NUM] = {0};
int path[ROWS_MAX_NUM][COLS_MAX_NUM] = {0};
size_t rows_num, cols_num;

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
  char line[STRING_CAPACITY];
  FILE *file = fopen(input_file_path, "r");
  if (!file) {
    perror("Error opening the file");
    return (EXIT_FAILURE);
  }
  memset(maze, 0, sizeof(maze));
  rows_num = 0;
  cols_num = 0;
  while (true) {
    if (!fgets(line, sizeof(line), file)) break;
    trim(line);
    cols_num = strlen(line);
    memmove(&maze[rows_num++][0], line, cols_num);
  } /* loop over lines */
  fclose(file);
  PRINTF("Input data (%lu x %lu) were read.\n", rows_num, cols_num);
  return EXIT_SUCCESS;
} /* read_input_data(.) */
/******************************************************************************/
void solve_part1(void) {
  int ans = 0;
  printf("%d\n", ans);
} /* solve_part1() */
/******************************************************************************/
void solve_part2(void) {
  int ans = 0;
  printf("%d\n", ans);
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
