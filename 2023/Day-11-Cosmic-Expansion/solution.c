#define _GNU_SOURCE
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAP_MAX_WIDTH (512)
#define MAP_MAX_HEIGHT (512)
#define STRING_CAPACITY (MAP_MAX_WIDTH)
#define DEFAULT_INPUT_FILE "input.txt"

#ifdef DEBUG
#define PUTS puts
#define PRINTF printf
#else
#define PUTS(a) \
  {}
#define PRINTF(fmt, ...) \
  {}
#endif  // DEBUG

#define GALAXY_CHAR '#'
typedef struct {
  int row;
  int col;
} pos_t;
#define GALAXIES_MAX_NUM (512)
pos_t galaxy_positions[GALAXIES_MAX_NUM] = {};
int galaxies_num = 0;
int map_cols_num = 0;
int map_rows_num = 0;
bool empty_cols_mask[MAP_MAX_WIDTH] = {true};
bool empty_rows_mask[MAP_MAX_WIDTH] = {true};
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
  memset(empty_cols_mask, true, sizeof(empty_cols_mask));
  memset(empty_rows_mask, true, sizeof(empty_rows_mask));
  map_rows_num = 0;
  galaxies_num = 0;
  while (true) {
    if (!fgets(line, sizeof(line), file)) break;
    trim(line);
    if (!map_cols_num) map_cols_num = strlen(line);
    bool maps_row_is_empty = true;
    for (int col_idx = 0; col_idx < map_cols_num; ++col_idx) {
      if (GALAXY_CHAR == line[col_idx]) {
        galaxy_positions[galaxies_num].row = map_rows_num;
        galaxy_positions[galaxies_num].col = col_idx;
        ++galaxies_num;
        maps_row_is_empty = false;
        empty_cols_mask[col_idx] = false;
      }
    }
    empty_rows_mask[map_rows_num] = maps_row_is_empty;
    ++map_rows_num;
  } /* loop over lines */
  fclose(file);
  PRINTF("Read map of size [%d x %d]\n", map_rows_num, map_cols_num);
  PRINTF("Found %d galaxies\n", galaxies_num);
  return EXIT_SUCCESS;
} /* read_input_data(.) */
/******************************************************************************/
void expand_the_universe(void) {
  int cols_new_indices[MAP_MAX_WIDTH];
  int rows_new_indices[MAP_MAX_HEIGHT];
  int j = 0;
  for (int i = 0; i < map_cols_num; ++i) {
    cols_new_indices[i] = j++;
    if (empty_cols_mask[i]) ++j;
  }
  j = 0;
  for (int i = 0; i < map_rows_num; ++i) {
    rows_new_indices[i] = j++;
    if (empty_rows_mask[i]) ++j;
  }

  for (int i = 0; i < galaxies_num; ++i) {
    const pos_t old_pos = galaxy_positions[i];
    const pos_t new_pos = {.col = cols_new_indices[old_pos.col],
                           .row = rows_new_indices[old_pos.row]};
    galaxy_positions[i] = new_pos;
    PRINTF("Galaxy#%03d: (%03d, %03d) -> (%03d, %03d)\n", i, old_pos.row,
           old_pos.col, new_pos.row, new_pos.col);
  } /* loop over galaxies */

} /* expand_the_universe() */
/******************************************************************************/
#define ABS(x) ((x) < 0 ? -(x) : (x))
void solve_part1(void) {
  long sum = 0;
  for (int i = 0; i < galaxies_num - 1; ++i) {
    const pos_t pi = galaxy_positions[i];
    for (int j = i + 1; j < galaxies_num; ++j) {
      const pos_t pj = galaxy_positions[j];
      const long distance = ABS(pi.row - pj.row) + ABS(pi.col - pj.col);
      sum += distance;
    } /* j-loop */
  }   /* i-loop */
  printf("%ld\n", sum);
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
  expand_the_universe();
  solve_part1();
  solve_part2();
  return EXIT_SUCCESS;
}
