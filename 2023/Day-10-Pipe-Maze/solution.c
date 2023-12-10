#define _GNU_SOURCE
#define _DEBUG

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
#define PUTS(a) \
  {}
#define PRINTF(fmt, ...) \
  {}
#endif  // DEBUG

#define START_CHAR ('S')

char maze[ROWS_MAX_NUM][COLS_MAX_NUM] = {0};
bool path_mask[ROWS_MAX_NUM][COLS_MAX_NUM] = {false};
int rows_num, cols_num;
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
  for (int row_idx = 0; row_idx < rows_num; ++row_idx) {
    // PRINTF("%s\n", maze[row_idx]);
    for (int col_idx = 0; col_idx < cols_num; ++col_idx) {
      if (path_mask[row_idx][col_idx]) PRINTF("\033[0;33m");
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
#define print_maze(_) \
  {}
#define print_pos(_) \
  {}
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
  for (int row_idx = 0; row_idx < rows_num; ++row_idx)
    maze[row_idx][cols_num] = 0;

  PRINTF("Input data [%d x %d] -> [%d x %d] were read.\n", rows_num - 2,
         cols_num - 2, rows_num, cols_num);
  return EXIT_SUCCESS;
} /* read_input_data(.) */
/******************************************************************************/
#define DIFFERENT(pos, pos_ptr) \
  ((pos.row != pos_ptr->row) || (pos.col != pos_ptr->col))
/********************************************************/
void propagate(pos_t *curr, pos_t *prev) {
  const char c = maze[curr->row][curr->col];
  char n;
  pos_t t = *curr;
  path_mask[curr->row][curr->col] = true;
  /* try north/up */
  t.row = curr->row - 1;
  if (!path_mask[t.row][t.col] && DIFFERENT(t, prev)) {
    n = maze[t.row][t.col];
    if (('.' != n) && (('|' == c) || ('J' == c) || ('L' == c) || ('S' == c)) &&
        (('|' == n) || ('7' == n) || ('F' == n))) {
      *prev = *curr;
      *curr = t;
      return;
    }
  }

  /* try south/down */
  t.row = curr->row + 1;
  if (!path_mask[t.row][t.col] && DIFFERENT(t, prev)) {
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
  if (!path_mask[t.row][t.col] && DIFFERENT(t, prev)) {
    n = maze[t.row][t.col];
    if (('.' != n) && (('-' == c) || ('J' == c) || ('7' == c) || ('S' == c)) &&
        (('-' == n) || ('L' == n) || ('F' == n))) {
      *prev = *curr;
      *curr = t;
      return;
    }
  }

  /* try east/right */
  t.col = curr->col + 1;
  if (!path_mask[t.row][t.col] && DIFFERENT(t, prev)) {
    n = maze[t.row][t.col];
    if (('.' != n) && (('-' == c) || ('L' == c) || ('F' == c) || ('S' == c)) &&
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
    maze[start.row][start.col] = START_CHAR;
#ifdef DEBUG
    print_maze();
    print_pos(prev_a);
    PRINTF(" -> ");
    print_pos(a);
    PRINTF("\n");
    print_pos(prev_b);
    PRINTF(" -> ");
    print_pos(b);
    PRINTF("\n\n");
#endif /* DEBUG */
  } while ((a.row != b.row) || (a.col != b.col));
  path_mask[a.row][a.col] = true;
  printf("%d\n", steps_num);
} /* solve_part1() */
/******************************************************************************/
void fix_start(void) {
  char t, b, l, r;
  /* top */
  t = maze[start.row - 1][start.col];
  t = ('|' == t) || ('F' == t) || ('7' == t) ? '|' : '.';

  /* botom */
  b = maze[start.row + 1][start.col];
  b = ('|' == b) || ('L' == b) || ('J' == b) ? '|' : '.';

  /* left */
  l = maze[start.row][start.col - 1];
  l = ('-' == l) || ('L' == l) || ('F' == l) ? '-' : '.';

  /* right */
  r = maze[start.row][start.col + 1];
  r = ('-' == r) || ('7' == r) || ('J' == r) ? '-' : '.';
  char c = 'S';
  if ('-' == l) {
    if ('|' == t) {
      c = 'J';
    } else if ('|' == b) {
      c = '7';
    } else if ('-' == r) {
      c = '-';
    }
  } else {
    if ('-' == r) {
      if ('|' == t) {
        c = 'L';
      } else if ('|' == b) {
        c = 'F';
      }
    } else if (('|' == t) && ('|' == b)) {
      c = '|';
    }
  }
  maze[start.row][start.col] = c;
}
/******************************************************************************/
#define IS_CORNER(c) (('F' == c) || ('7' == c) || ('L' == c) || ('J' == c))
void solve_part2(void) {
  int ans = 0;
  fix_start();
  bool inside;
  for (int row_idx = 1; row_idx < rows_num - 1; ++row_idx) {
    char last_corner = '.';
    bool is_corner = false;

    inside = false;
    for (int col_idx = 1; col_idx < cols_num - 1; ++col_idx) {
      const char c = maze[row_idx][col_idx];
      if (path_mask[row_idx][col_idx]) {
        is_corner = IS_CORNER(c);
        const char prev_c = maze[row_idx][col_idx - 1];
        if (path_mask[row_idx][col_idx - 1] && (('-' == c) || is_corner) &&
            IS_CORNER(prev_c)) {
          last_corner = prev_c;
        }
        if ('-' == c) {
          continue;
        }
        if ('|' == c) {
          inside ^= 1;
          last_corner = '.';
          continue;
        }
        if (is_corner) {
          if ((last_corner == 'F' && 'J' == c) ||
              (last_corner == 'L' && '7' == c)) {
            inside ^= 1;
          }
        }
        // if (inside) {
        //   if (('J' == c) || ('7' == c)) {
        //     inside = false;
        //   }
        // } else {
        //   if (('L' == c) || ('F' == c)) {
        //     inside = false;
        //   }
        // }
        last_corner = '.';
        continue;
      } else {
        maze[row_idx][col_idx] = inside ? 'I' : 'O';
      }
      ans += inside;
    } /* loop over cols */
  }   /* loop over rows */
  print_maze();
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
void prepare_path_mask(void) {
  memset(path_mask, false, sizeof(path_mask));
  path_mask[start.row][start.col] = true;
} /* prepare_path_mask */
/******************************************************************************/
int main(int argc, char *argv[]) {
  const char *input_file_path = (argc > 1) ? argv[1] : DEFAULT_INPUT_FILE;
  if (EXIT_SUCCESS != read_input_data(input_file_path)) {
    perror("Error reading input data from file");
    return (EXIT_FAILURE);
  }
  print_maze();
  find_start();
  prepare_path_mask();
  PRINTF("start = [%d, %d]\n", start.row, start.col);
  solve_part1();
  solve_part2();
  return EXIT_SUCCESS;
}
