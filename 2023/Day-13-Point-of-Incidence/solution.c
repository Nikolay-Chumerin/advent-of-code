#define _GNU_SOURCE
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRING_CAPACITY (128)
#define DEFAULT_INPUT_FILE "input.txt"
#define MAX_WIDTH (32)
#define MAX_HEIGHT (32)

static const char *default_input_file = DEFAULT_INPUT_FILE;
FILE *file = NULL;
char input_file_path[2048] = {};
typedef int32_t idx_t;
typedef uint64_t hash_t;

typedef struct {
  char pattern[MAX_HEIGHT][MAX_WIDTH];
  idx_t width;
  idx_t height;
  hash_t row_hashes[MAX_HEIGHT];
  hash_t col_hashes[MAX_WIDTH];
} valley_t;

valley_t valley;

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
void trim(char *line) {
  int line_len = strlen(line);
  while (line_len && (line[line_len - 1] <= ' ')) {
    line[--line_len] = 0;
  }
} /* trim(.) */
/******************************************************************************/
#ifdef DEBUG
void print_valley(void) {
  for (idx_t i = 0; i < valley.height; ++i) {
    PRINTF("%s\n", valley.pattern[i]);
  } /* loop over rows */
} /* print_valley() */
#else
#define print_valley(_)                                                        \
  {}
#endif
/******************************************************************************/
void open_file(const char *file_path) {
  file = fopen(file_path, "r");
  if (!file) {
    perror("Error opening the file");
    exit(EXIT_FAILURE);
  }
} /* open_file() */
/******************************************************************************/
void close_file(void) {
  if (file)
    fclose(file);
  file = NULL;
} /* close_file() */
/******************************************************************************/
void compute_hashes(void) {
  /* row hashes */
  for (idx_t row_idx = 0; row_idx < valley.height; ++row_idx) {
    char *row = valley.pattern[row_idx];
    hash_t row_hash = 0;
    for (idx_t col_idx = valley.width - 1; col_idx >= 0; --col_idx) {
      row_hash = (row_hash << 1) + ('#' == row[col_idx]);
    } /* loop over cols */
    valley.row_hashes[row_idx] = row_hash;
  } /* loop over rows */

  /* col hashes */
  for (idx_t col_idx = 0; col_idx < valley.width; ++col_idx) {
    hash_t col_hash = 0;
    for (idx_t row_idx = valley.height - 1; row_idx >= 0; --row_idx) {
      col_hash = (col_hash << 1) + ('#' == valley.pattern[row_idx][col_idx]);
    } /* loop over rows */
    valley.col_hashes[col_idx] = col_hash;
  } /* loop over cols */
} /* compute_hashes() */
/******************************************************************************/
void read_next_valey(void) {
  char line[STRING_CAPACITY];
  valley.width = valley.height = 0;
  while (true) {
    if (!fgets(line, sizeof(line), file))
      break;
    trim(line);
    const idx_t line_len = strlen(line);
    if (!line_len)
      break;
    if (valley.width <= 0) {
      valley.width = line_len;
    }
    memmove(&valley.pattern[valley.height][0], line, valley.width);
    ++valley.height;
  } /* loop over lines */
} /* read_next_valey() */
/******************************************************************************/
idx_t horizontal_mirror_position(void) {
  for (idx_t row_idx = 1; row_idx < valley.height; ++row_idx) {
    const int max_shift =
        (row_idx <= valley.height / 2) ? row_idx : valley.height - row_idx;
    bool symmetrical = true;
    for (idx_t shift = 0; symmetrical && (shift < max_shift); ++shift) {
      const idx_t idx1 = row_idx - shift - 1;
      const idx_t idx2 = row_idx + shift;
      const hash_t hash1 = valley.row_hashes[idx1];
      const hash_t hash2 = valley.row_hashes[idx2];
      symmetrical &= (hash1 == hash2);
    } /* loop over shifts */
    if (symmetrical)
      return row_idx;
  } /* loop over rows */
  return 0;
} /* horizontal_mirror_position() */
/******************************************************************************/
idx_t vertical_mirror_position(void) {
  for (idx_t col_idx = 1; col_idx < valley.width; ++col_idx) {
    const int max_shift =
        (col_idx <= valley.width / 2) ? col_idx : valley.width - col_idx;
    bool symmetrical = true;
    for (idx_t shift = 0; symmetrical && (shift < max_shift); ++shift) {
      const idx_t idx1 = (col_idx - shift - 1);
      const idx_t idx2 = (col_idx + shift);
      const hash_t hash1 = valley.col_hashes[idx1];
      const hash_t hash2 = valley.col_hashes[idx2];
      symmetrical &= (hash1 == hash2);
    } /* loop over shifts */
    if (symmetrical)
      return col_idx;
  } /* loop over cols */
  return 0;
} /* vertical_mirror_position() */
/******************************************************************************/
void solve_part1(void) {
  open_file(input_file_path);
  idx_t ans = 0;
  while (true) {
    read_next_valey();
    if (!valley.height)
      break;
    PRINTF("-----------------------------\n\n");
    print_valley();
    compute_hashes();
    const idx_t hor_mirror_pos = horizontal_mirror_position();
    const idx_t ver_mirror_pos = vertical_mirror_position();
    ans += 100 * hor_mirror_pos + ver_mirror_pos;
    PRINTF("hor_mirror_pos=%d ver_mirror_pos=%d ans=%d\n", hor_mirror_pos,
           ver_mirror_pos, ans);
  }
  close_file();
  printf("%d\n", ans);
} /* solve_part1() */
/******************************************************************************/
bool fix_horizontal_smudge(void) {
  idx_t smudge_row = -1;
  idx_t smudge_col = -1;

  for (idx_t row_idx = 1; row_idx < valley.height; ++row_idx) {
    const int max_shift =
        (row_idx <= valley.height / 2) ? row_idx : valley.height - row_idx;
    idx_t diff_bits_num = 0;
    for (idx_t shift = 0; shift < max_shift; ++shift) {
      const idx_t idx1 = row_idx - shift - 1;
      const idx_t idx2 = row_idx + shift;
      const hash_t hash1 = valley.row_hashes[idx1];
      const hash_t hash2 = valley.row_hashes[idx2];
      hash_t hash_xor = hash1 ^ hash2;
      for (idx_t bit_idx = 0; bit_idx < valley.width; ++bit_idx) {
        if (hash_xor & 1) {
          ++diff_bits_num;
          smudge_col = bit_idx;
          smudge_row = idx1;
        }
        hash_xor >>= 1;
      } /* loop over bits in hash_xor */
    }   /* loop over shifts */
    if (diff_bits_num == 1) {
      PRINTF("Fixing (horizontal) smudge at [%d,%d]\n", smudge_row, smudge_col);
      valley.pattern[smudge_row][smudge_col] =
          ('.' == valley.pattern[smudge_row][smudge_col]) ? '#' : '.';
      return true;
    }
  } /* loop over candidate-rows */
  return false;
} /* fix_horizontal_smudge() */
/******************************************************************************/
void fix_vertical_smudge(void) {
  idx_t smudge_row = -1;
  idx_t smudge_col = -1;

  for (idx_t col_idx = 1; col_idx < valley.width; ++col_idx) {
    const int max_shift =
        (col_idx <= valley.width / 2) ? col_idx : valley.width - col_idx;
    idx_t diff_bits_num = 0;
    for (idx_t shift = 0; shift < max_shift; ++shift) {
      const idx_t idx1 = (col_idx - shift - 1);
      const idx_t idx2 = (col_idx + shift);
      const hash_t hash1 = valley.col_hashes[idx1];
      const hash_t hash2 = valley.col_hashes[idx2];
      hash_t hash_xor = hash1 ^ hash2;
      for (idx_t bit_idx = 0; bit_idx < valley.width; ++bit_idx) {
        if (hash_xor & 1) {
          ++diff_bits_num;
          smudge_col = idx1;
          smudge_row = bit_idx;
        }
        hash_xor >>= 1;
      } /* loop over bits in hash_xor */
    }   /* loop over shifts */
    if (diff_bits_num == 1) {
      PRINTF("Fixing (vertical) smudge at [%d,%d]\n", smudge_row, smudge_col);
      valley.pattern[smudge_row][smudge_col] =
          ('.' == valley.pattern[smudge_row][smudge_col]) ? '#' : '.';
      return;
    }
  } /* loop over candidate-rows */
} /* fix_vertical_smudge() */
/******************************************************************************/
void solve_part2(void) {
  PUTS("--------------------------------------------------");
  PUTS("Part 2");
  open_file(input_file_path);
  idx_t ans = 0;
  while (true) {
    read_next_valey();
    if (!valley.height)
      break;
    PRINTF("-----------------------------\n\n");
    compute_hashes();
    if (!fix_horizontal_smudge())
      fix_vertical_smudge();
    compute_hashes();
    print_valley();
    const idx_t hor_mirror_pos = horizontal_mirror_position();
    const idx_t ver_mirror_pos =
        (!hor_mirror_pos) ? vertical_mirror_position() : 0;
    ans += 100 * hor_mirror_pos + ver_mirror_pos;
    PRINTF("hor_mirror_pos=%d ver_mirror_pos=%d ans=%d\n", hor_mirror_pos,
           ver_mirror_pos, ans);
  }
  close_file();
  printf("%d\n", ans);
} /* solve_part2() */
/******************************************************************************/
void init(int argc, char *argv[]) {
  strcpy(input_file_path, (argc > 1) ? argv[1] : default_input_file);
  memset(&valley, 0, sizeof(valley_t));
  valley.height = valley.width = -1;
} /* init() */
/******************************************************************************/
void finalize(void) { close_file(); } /* finalize() */
/******************************************************************************/
int main(int argc, char *argv[]) {
  init(argc, argv);
  solve_part1();
  solve_part2();
  finalize();
  return EXIT_SUCCESS;
}
