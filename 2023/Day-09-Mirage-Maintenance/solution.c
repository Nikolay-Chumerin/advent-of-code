#define _GNU_SOURCE
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRING_CAPACITY (8192)
#define DEFAULT_INPUT_FILE "input.txt"
#define EPS (1E-8)
#define DEBUG

#ifdef DEBUG
#define PUTS puts
#define PRINTF printf
#else
#define PUTS(a) (0)
#define PRINTF(fmt, ...) (0)
#endif  // DEBUG

#define SEQUENCE_CAPACITY (1024)
#define SEQUENCES_MAX_NUM (256)

typedef int seq_data_t[SEQUENCE_CAPACITY];
typedef struct {
  seq_data_t data;
  size_t len;
} seq_t;

seq_t seqs[SEQUENCES_MAX_NUM];
size_t seqs_num;

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

  seqs_num = 0U;
  while (true) {
    if (!fgets(line, sizeof(line), file)) break;
    trim(line);
    PRINTF("'%s'\n", line);
    /* process line ... */
    int vals_read = 0;
    int chars_read = 0;
    char *str = line;
    seq_t *seq = &seqs[seqs_num];
    int *data = (int *)seq->data;
    seq->len = 0U;
    do {
      vals_read = sscanf(str, "%i %n", data, &chars_read);
      if (vals_read != 1) break;
      PRINTF("%d ", seqs[seqs_num].data[seqs[seqs_num].len]);
      ++seq->len;
      ++data;
      str += chars_read;
    } while (true);
    ++seqs_num;
    PRINTF(" Done: %lu values were read.\n", seq->len);
  } /* loop over lines */
  fclose(file);
  PRINTF("Input data (%lu sequences) were read.\n", seqs_num);
  return EXIT_SUCCESS;
} /* read_input_data(.) */
/******************************************************************************/
void print_seq(const seq_t *seq) {
  PRINTF("len=%lu [ ", seq->len);
  const int *d = seq->data;
  for (size_t i = 0U; (i < seq->len); ++i) PRINTF("%d ", *d++);
  PRINTF("]\n");
}
/******************************************************************************/
bool all_zeros(const int* data, const size_t len) {
  bool ans = true;
  for (size_t i = 0U; ans && (i < len); ++i) ans &= (data[i] == 0);
  return ans;
} /* all_zeros(.) */
/******************************************************************************/
void solve_part1(void) {
  int ans = 0;
  seq_t diffs;
  for (size_t seq_idx = 0U; seq_idx < seqs_num; ++seq_idx) {
    const seq_t *seq = &seqs[seq_idx];
    const int *data = (int *)seq->data;
    const size_t seq_len = seq->len;
    diffs = seqs[seq_idx];  // memmove(&diffs, seq, sizeof(seq_t));
    print_seq(&diffs);
    do {
      for (size_t i = 0; i < diffs.len - 1; ++i) {
        diffs.data[i] = diffs.data[i + 1] - diffs.data[i];
      }
      --diffs.len;
      print_seq(&diffs);
    } while (!all_zeros(diffs.data, diffs.len));

    int seq_next_element = 0;
    for (size_t i = diffs.len - 1; i < seq_len; ++i) {
      seq_next_element += diffs.data[i];
    }
    PRINTF("seq_next_element=%d\n", seq_next_element);
    ans += seq_next_element;
  } /* loop over sequences */

  printf("%d\n", ans);
} /* solve_part1() */
/******************************************************************************/
void solve_part2(void) {
  int ans = 0;
  seq_t diffs;
  for (size_t seq_idx = 0U; seq_idx < seqs_num; ++seq_idx) {
    const seq_t *seq = &seqs[seq_idx];
    const int *data = (int *)seq->data;
    const size_t seq_len = seq->len;
    diffs = seqs[seq_idx];
    print_seq(&diffs);
    bool diffs_are_all_zeros = false;
    size_t step = 1;
    do {
      for (size_t i = seq_len - 1; i >= step; --i) {
        diffs.data[i] -= diffs.data[i - 1];
      }
      ++step;
      print_seq(&diffs);
      diffs_are_all_zeros = all_zeros(diffs.data + step, diffs.len - step);
      PRINTF("diffs_are_all_zeros=%d\n", diffs_are_all_zeros);
    } while (!diffs_are_all_zeros);

    PRINTF("step=%lu\n", step);
    int seq_prev_element = 0;
    int sign = 1;
    for (size_t i = 0; i <= seq_len - step + 1; ++i) {
      seq_prev_element += sign * diffs.data[i];
      sign = -sign;
    }
    PRINTF("seq_prev_element=%d\n", seq_prev_element);
    ans += seq_prev_element;
  } /* loop over sequences */
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
