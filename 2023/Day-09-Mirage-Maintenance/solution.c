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

typedef struct {
  int seq[SEQUENCE_CAPACITY];
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
    seq->len = 0U;
    do {
      vals_read = sscanf(str, "%d %n", &seq->seq[seq->len], &chars_read);
      if (vals_read != 1) break;
      ++seq->len;
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
void solve_part1(void) {
  /* solution of the part1 */
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
