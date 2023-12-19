#define _GNU_SOURCE
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_INPUT_FILE "input.txt"
#define MAX_COLS_NUM (128)
#define MAX_ROWS_NUM (128)
#define DONT_USE_COLORS
#ifdef DEBUG
#define PUTS puts
#define PRINTF printf
#else
#define PUTS(a)                                                                \
  {}
#define PRINTF(fmt, ...)                                                       \
  {}
#endif // DEBUG

typedef enum {
  DIR_UP = '^',
  DIR_RIGHT = '>',
  DIR_DOWN = 'v',
  DIR_LEFT = '<'
} direction_t;

typedef struct {
  size_t row;
  size_t col;
  direction_t dir;
} cell_t;

cell_t stack[MAX_ROWS_NUM * MAX_COLS_NUM];
size_t stack_size = 0;
size_t rows_num = 0;
size_t cols_num = 0;
typedef char map_t[MAX_ROWS_NUM][MAX_COLS_NUM];
map_t original_map, map;

bool energized[MAX_ROWS_NUM][MAX_COLS_NUM] = {false};
cell_t beam;

/******************************************************************************/
#ifdef DEBUG
void print_data(void) {
  for (size_t row_idx = 0; row_idx < rows_num; ++row_idx) {
#ifdef USE_COLORS
    for (size_t col_idx = 0; col_idx < cols_num; ++col_idx) {
      if (energized[row_idx][col_idx])
        printf("\033[0;32m");
      if (col_idx == beam.col && row_idx == beam.row)
        printf("\033[0;31m");
      printf("%c\033[0m", map[row_idx][col_idx]);
    }
    printf("\n");
#else
    printf("%s\n", map[row_idx]);
#endif
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
    char *line = original_map[rows_num];
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
bool move(cell_t *cell) {
  if (!cell)
    return false;
  switch (cell->dir) {
  case DIR_UP:
    if (cell->row > 0) {
      --(cell->row);
      return true;
    }
    break;
  case DIR_DOWN:
    if (cell->row < rows_num - 1) {
      ++(cell->row);
      return true;
    }
    break;
  case DIR_RIGHT:
    if (cell->col < cols_num - 1) {
      ++(cell->col);
      return true;
    }
    break;
  case DIR_LEFT:
    if (cell->col > 0) {
      --(cell->col);
      return true;
    }
    break;
  }
  return false;
} /* move(.) */
/******************************************************************************/
void reflect(const char mirror, cell_t *beam) {
  if (!beam)
    return;
  switch (beam->dir) {
  case DIR_UP:
    beam->dir = ('/' == mirror) ? DIR_RIGHT : DIR_LEFT;
    break;
  case DIR_DOWN:
    beam->dir = ('/' == mirror) ? DIR_LEFT : DIR_RIGHT;
    break;
  case DIR_RIGHT:
    beam->dir = ('/' == mirror) ? DIR_UP : DIR_DOWN;
    break;
  case DIR_LEFT:
    beam->dir = ('/' == mirror) ? DIR_DOWN : DIR_UP;
    break;
  } /* switch (beam->dir) */
}
/******************************************************************************/
int count_energized(void) {
  int ans = 0;
  for (size_t row_idx = 0; row_idx < rows_num; ++row_idx) {
    for (size_t col_idx = 0; col_idx < cols_num; ++col_idx) {
      ans += energized[row_idx][col_idx] ? 1 : 0;
    }
  } /* loop over rows */
  return ans;
} /* count_energized() */
/******************************************************************************/
int solve_for_beam(const cell_t start_beam) {
  beam = start_beam;
  memset(energized, false, sizeof(energized));
  memcpy(map, original_map, sizeof(map));
  bool moved = false;
  PRINTF("beam: row=%lu col=%lu dir=%c map[beam]='%c'\n", beam.row, beam.col,
         beam.dir, map[beam.row][beam.col]);
  print_data();
  do {
    const char c = map[beam.row][beam.col];
    switch (c) {
    case '.':
      map[beam.row][beam.col] = beam.dir;
      break;
    case '\\':
    case '/':
      reflect(c, &beam);
      break;
    case '|':
      if ((DIR_DOWN == beam.dir) || (DIR_UP == beam.dir)) { /* ^ v */
      } else {                                              /* >|  or |< */
        if (beam.row > 0) {
          stack[stack_size++] =
              (cell_t){.row = beam.row - 1, .col = beam.col, .dir = DIR_UP};
        }
        beam.dir = DIR_DOWN;
      }
      break;
    case '-':
      if ((DIR_LEFT == beam.dir) || (DIR_RIGHT == beam.dir)) { /* >- or -< */
      } else {                                                 /* ^- or v- */
        if (beam.col > 0) {
          stack[stack_size++] =
              (cell_t){.row = beam.row, .col = beam.col - 1, .dir = DIR_LEFT};
        }
        beam.dir = DIR_RIGHT;
      }
    } /* switch() */
    energized[beam.row][beam.col] = true;
    moved = move(&beam);
    const char dir = (char)beam.dir;
    if (!moved || dir == map[beam.row][beam.col]) {
      /* stop the current beam and try to pop from stack */
      if (stack_size) {
        beam = stack[--stack_size];
        moved = true;
      }
    }
    PRINTF("beam: row=%lu col=%lu dir=%c map[beam]='%c'\n", beam.row, beam.col,
           beam.dir, map[beam.row][beam.col]);
    PRINTF("energized cells num = %d\n", count_energized());
    PUTS("--------------------------");
  } while (moved); /* while */
  print_data();
  PRINTF("energized cells num = %d\n", count_energized());
  return count_energized();
}
/******************************************************************************/
void solve_part1(void) {
  beam = (cell_t){.row = 0, .col = 0, .dir = DIR_RIGHT};
  int ans = solve_for_beam(beam);
  printf("%d\n", ans);
} /* solve_part1() */
/******************************************************************************/
void solve_part2(void) {
  int ans, max_ans = -1;
  for (size_t row_idx = 0; row_idx < rows_num; ++row_idx) {
    beam = (cell_t){.row = row_idx, .col = 0, .dir = DIR_RIGHT};
    ans = solve_for_beam(beam);
    if (ans > max_ans) {
      max_ans = ans;
      printf("%d\n", max_ans);
    }
    beam = (cell_t){.row = row_idx, .col = cols_num - 1, .dir = DIR_LEFT};
    ans = solve_for_beam(beam);
    if (ans > max_ans) {
      max_ans = ans;
      printf("%d\n", max_ans);
    }
  }

  for (size_t col_idx = 0; col_idx < cols_num; ++col_idx) {
    beam = (cell_t){.col = col_idx, .row = 0, .dir = DIR_DOWN};
    ans = solve_for_beam(beam);
    if (ans > max_ans) {
      max_ans = ans;
      printf("%d\n", max_ans);
    }
    beam = (cell_t){.col = col_idx, .row = rows_num - 1, .dir = DIR_UP};
    ans = solve_for_beam(beam);
    if (ans > max_ans) {
      max_ans = ans;
      printf("%d\n", max_ans);
    }
  }
  // printf("%d\n", max_ans);
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
