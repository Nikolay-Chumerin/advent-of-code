#define _GNU_SOURCE
#define DEBUG_

#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRING_CAPACITY (256)
#define ROWS_MAX_NUM (256)
#define COLS_MAX_NUM (STRING_CAPACITY)
#define DEFAULT_INPUT_FILE "test2.txt"

#ifdef DEBUG
#define PUTS puts
#define PRINTF printf
#else
#define PUTS(a) (0)
#define PRINTF(fmt, ...) (0)
#endif  // DEBUG

#define START_CHAR ('S')

char maze[ROWS_MAX_NUM][COLS_MAX_NUM] = {0};
int path[ROWS_MAX_NUM][COLS_MAX_NUM] = {0};
size_t rows_num, cols_num;
typedef struct {
  int row;
  int col;
} pos_t;

pos_t start = {-1, -1};
pos_t a = {-1, -1}, b = {-1, -1};
pos_t prev_a = {-1, -1}, prev_b = {-1, -1};

/******************************************************************************/
void trim(char *line) {
  int line_len = strlen(line);
  while (line_len && (line[line_len - 1] <= ' ')) line[--line_len] = 0;
} /* trim(.) */
/******************************************************************************/
#ifdef DEBUG
void print_maze(void) {
  for (size_t row_idx = 0; row_idx < rows_num; ++row_idx) {
    // PRINTF("%s\n", maze[row_idx]);
    for (size_t col_idx = 0; col_idx < cols_num; ++col_idx) {
      if (a.col == col_idx && a.row == row_idx) PRINTF("\033[0;31m");
      if (b.col == col_idx && b.row == row_idx) PRINTF("\033[0;32m");
      if (start.col == col_idx && start.row == row_idx) PRINTF("\033[0;34m");
      PRINTF("%c\033[0m", maze[row_idx][col_idx]);
    }
    PRINTF("\n");
  }
} /* print_maze() */
/***********************************/
void print_pos(const pos_t p) {
  PRINTF("[%d,%d]", p.row, p.col);
} /* print_pos(.) */
#else
#define print_maze(_) (0)
#define print_pos(p) (0)
#endif  // DEBUG
/******************************************************************************/
int read_input_data(const char *input_file_path) {
  PRINTF("Reading input data from the '%s' file.\n", input_file_path);
  char line[STRING_CAPACITY];
  FILE *file = fopen(input_file_path, "r");
  if (!file) {
    perror("Error opening the file");
    return (EXIT_FAILURE);
  }

  memset(maze, '.', sizeof(maze));
  rows_num = 1;
  cols_num = 0;
  while (true) {
    if (!fgets(line, sizeof(line), file)) break;
    trim(line);
    cols_num = strlen(line);
    memmove(&maze[rows_num++][1], line, cols_num);
  } /* loop over lines */
  fclose(file);

  cols_num += 2;
  ++rows_num;
  for (size_t row_idx = 0; row_idx < rows_num; ++row_idx)
    maze[row_idx][cols_num] = 0;

  PRINTF("Input data [%lu x %lu] -> [%lu x %lu] were read.\n", rows_num - 2,
         cols_num - 2, rows_num, cols_num);
  return EXIT_SUCCESS;
} /* read_input_data(.) */
/******************************************************************************/
#define DIFFERENT(pos, pos_ptr) \
  ((pos.row != pos_ptr->row) || (pos.col != pos_ptr->col))
void propagate(pos_t *curr, pos_t *prev) {
  const char c = maze[curr->row][curr->col];
  char n;
  pos_t t = *curr;

  /* try north/up */
  t.row = curr->row - 1;
  if (DIFFERENT(t, prev)) {
    n = maze[t.row][t.col];
    if (('.' != n) &&
        (('|' == c) || ('J' == c) || ('L' == c) || ('S' == c)) &&
        (('|' == n) || ('7' == n) || ('F' == n))) {
      *prev = *curr;
      *curr = t;
      return;
    }
  }

  /* try south/down */
  t.row = curr->row + 1;
  if (DIFFERENT(t, prev)) {
    n = maze[t.row][t.col];
    if (('.' != n) &&
        ((('|' == c) || ('7' == c) || ('F' == c) || ('S' == c))) &&
        (('|' == n) || ('J' == n) || ('L' == n))) {
      *prev = *curr;
      *curr = t;
      return;
    }
  }

  /* try west/left */
  t.row = curr->row;
  t.col = curr->col - 1;
  if (DIFFERENT(t, prev)) {
    n = maze[t.row][t.col];
    if (('.' != n) &&
        (('-' == c) || ('J' == c) || ('7' == c) || ('S' == c)) &&
        (('-' == n) || ('L' == n) || ('F' == n))) {
      *prev = *curr;
      *curr = t;
      return;
    }
  }

  /* try east/right */
  t.col = curr->col + 1;
  if (DIFFERENT(t, prev)) {
    n = maze[t.row][t.col];
    if (('.' != n) &&
        (('-' == c) || ('L' == c) || ('F' == c) || ('S' == c)) &&
        (('-' == n) || ('J' == n) || ('7' == n))) {
      *prev = *curr;
      *curr = t;
      return;
    }
  }
} /* propagate(.) */
/******************************************************************************/
void solve_part1(void) {
  int steps_num = 0;
  prev_a = a = prev_b = b = start;
  print_maze();
  char c;
  do {
    ++steps_num;
    PUTS("a moves...");
    propagate(&a, &prev_a);
    if ((a.row == b.row) && (a.col == b.col)) break;
    if (1 == steps_num) {
      c = maze[a.row][a.col];
      maze[a.row][a.col] = '.';
    };

    PUTS("b moves...");
    propagate(&b, &prev_b);
    if (1 == steps_num) {
      maze[a.row][a.col] = c;
    };
    // maze[b.row][b.col] = 'b';
    // maze[a.row][a.col] = 'a';
    maze[start.row][start.col] = START_CHAR;
    print_maze();
    print_pos(prev_a); PRINTF(" -> "); print_pos(a); PRINTF("\n");
    print_pos(prev_b); PRINTF(" -> "); print_pos(b); PRINTF("\n\n");
  } while ((a.row != b.row) || (a.col != b.col));
  printf("%d\n", steps_num);
} /* solve_part1() */
/******************************************************************************/
void solve_part2(void) {
  int ans = 0;
  printf("%d\n", ans);
} /* solve_part2() */
/******************************************************************************/
void find_start(void) {
  for (start.row = 0; start.row < rows_num; ++start.row) {
    for (start.col = 0; start.col < cols_num; ++start.col) {
      if (START_CHAR == maze[start.row][start.col]) return;
    } /* col-loop */
  }   /* row-loop */
} /* find_start() */
/******************************************************************************/
int main(int argc, char *argv[]) {
  const char *input_file_path = (argc > 1) ? argv[1] : DEFAULT_INPUT_FILE;
  if (EXIT_SUCCESS != read_input_data(input_file_path)) {
    perror("Error reading input data from file");
    return (EXIT_FAILURE);
  }
  print_maze();
  find_start();
  PRINTF("start = [%d, %d]\n", start.row, start.col);
  solve_part1();
  solve_part2();
  return EXIT_SUCCESS;
}
