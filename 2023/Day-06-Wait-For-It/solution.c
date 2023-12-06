#define _GNU_SOURCE
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRING_CAPACITY (8192)
#define MAX_RACES (16)
#define EPS (1E-8)

#define NDEBUG
#ifdef DEBUG
#define PUTS puts
#define PRINTF printf
#else
#define PUTS(a) (0)
#define PRINTF(fmt, ...) (0)
#endif // DEBUG

#define DEFAULT_INPUT_FILE "input.txt"

char *s = NULL;
int times[MAX_RACES];
int distances[MAX_RACES];
size_t races_num = 0U;
FILE *file;

/******************************************************************************/
void trim(char *line) {
  int line_len = strlen(line);
  while (line_len && (line[line_len - 1] <= ' ')) {
    line[--line_len] = 0;
  }
} /* trim(.) */
/******************************************************************************/
size_t read_row_to_array(int *out_arr) {
  PRINTF("Reading row...");
  char line[STRING_CAPACITY];
  fgets(line, sizeof(line), file);
  trim(line);
  size_t values_num = 0U;
  int values_read = 0;
  int chars_read = 0;
  char *s = line;
  while (*s > ' ')
    ++s; /* skip to the first space */
  do {
    values_read = sscanf(s, "%d %n", &out_arr[values_num], &chars_read);
    if (values_read != 1)
      break;
    ++values_num;
    s += chars_read;
  } while (true);
  PRINTF(" Done: %lu values were read.\n", values_num);
  return values_num;
} /* read_row_to_array(.) */
/******************************************************************************/
void part1(void) {
  long ans = 1;
  for (size_t race_idx = 0; race_idx < races_num; ++race_idx) {
    const int t = times[race_idx];
    const int d = distances[race_idx];
    const double D = t * t - 4.0F * d;
    PRINTF("race_idx=%ld t=%d d=%d D=%f ", race_idx, t, d, D);
    if (D < 0.0F) {
      ans = 0;
      break;
    }
    const double sqrtD = sqrt(D);
    const double x1 = (t - sqrtD) / 2.0F;
    const double x2 = (t + sqrtD) / 2.0F;
    const int integer_solutions_num = (floor(x2 - EPS) - ceil(x1 + EPS) + 1);
    ans *= integer_solutions_num;
    PRINTF("floor(x2-EPS)=%f ceil(x1+EPS)=%-8.4f\n", floor(x2 - EPS),
           ceil(x1 + EPS));
    PRINTF("sqrtD=%-8.4f x1=%-8.4f x2=%-8.4f num=%d ans=%li\n", sqrtD, x1, x2,
           integer_solutions_num, ans);
  } /*  loop over races */
  printf("%ld\n", ans);
} /* part1() */
/******************************************************************************/
void part2(void) {
  typedef long double REAL;
  typedef long long int INT;
  INT ans = 1;
  REAL t = 0.0;
  REAL d = 0.0;

  for (size_t race_idx = 0; race_idx < races_num; ++race_idx) {
    const int t_ = times[race_idx];
    t *= exp10l(ceil(log10(t_)));
    t += t_;

    const int d_ = distances[race_idx];
    d *= exp10l(ceil(log10(d_)));
    d += d_;
  } /*  loop over races */
  PRINTF("t=%Lf d=%Lf\n", t, d);
  const REAL D = t * t - 4.0 * d;
  if (D < 0.0) {
    ans = 0;
  } else {
    const REAL sqrtD = sqrtl(D);
    const REAL x1 = (t - sqrtD) / 2.0;
    const REAL x2 = (t + sqrtD) / 2.0;
    ans = (floorl(x2 - EPS) - ceill(x1 + EPS) + 1);
  }
  printf("%lld\n", ans);
} /* part2() */
/******************************************************************************/
int main(int argc, char *argv[]) {
  file = fopen((argc > 1) ? argv[1] : DEFAULT_INPUT_FILE, "r");
  if (!file) {
    perror("Error opening the file");
    return EXIT_FAILURE;
  }
  races_num = read_row_to_array(times);
  if (races_num != read_row_to_array(distances)) {
    perror("Inconsistent number of columns in the input data file");
    return EXIT_FAILURE;
  }
  fclose(file);

  part1();
  part2();

  return EXIT_SUCCESS;
}
