#define _GNU_SOURCE
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRING_CAPACITY (32768)
#define DEFAULT_INPUT_FILE "input.txt"

#ifdef DEBUG
#define PUTS puts
#define PRINTF printf
#else
#define PUTS(a)                                                                \
  {}
#define PRINTF(fmt, ...)                                                       \
  {}
#endif // DEBUG

char s[STRING_CAPACITY];

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
int hash(char *s) {
  int h = 0;
  while (*s) {
    h += *s++;
    h *= 17;
    h %= 256;
  }
  return h;
} /* hash(.) */
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
    if (!fgets(s, sizeof(s), file))
      break;
    trim(s);
  } /* loop over lines */
  // cols_num = strlen(&dish[0][0]);
  fclose(file);
  PUTS("Input data were read.");
  return EXIT_SUCCESS;
} /* read_input_data(.) */
/******************************************************************************/
void solve_part1(void) {
  int ans = 0;
  char* p = s;
  int h = 0;
  while (*p) {
    if (',' == *p) {
      ans += h;
      h = 0;
      ++p;
      continue;
    }
    h += *p++;
    h *= 17;
    h %= 256;
  }
  ans += h;
  printf("%d\n", ans);
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
