#define _GNU_SOURCE
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tgmath.h>

#define STRING_CAPACITY (1024)
#define SEGMENTS_CAPACITY (1024)
#define DEFAULT_INPUT_FILE "input.txt"
#define EPS (1e-4)

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
#ifdef DEBUG
void print_field(const bool *field, const size_t w, const size_t h) {
  for (size_t row_idx = 0; row_idx < h; ++row_idx) {
    const bool *row = field + row_idx * w;
    for (size_t col_idx = 0; col_idx < w; ++col_idx) {
      printf("%c", row[col_idx] ? '#' : '.');
    } /* loop over cols */
    puts("");
  } /* loop over rows */
} /* print_field(...) */
#else
#define print_field(a, b, c)                                                   \
  {}
#endif /* DEBUG */
/******************************************************************************/
typedef struct {
  char dir;
  int len;
  unsigned int r, g, b;
} segment_t;
typedef struct {
  int x, y;
} point_t;

segment_t segments[SEGMENTS_CAPACITY];
point_t polygone[SEGMENTS_CAPACITY];
size_t segments_num = 0;
int min_x = 0, max_x = 0, min_y = 0, max_y = 0;

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
  char line[STRING_CAPACITY];
  FILE *file = fopen(input_file_path, "r");
  if (!file) {
    perror("Error opening the file");
    return (EXIT_FAILURE);
  }
  size_t n = 0;
  while (true) {
    if (!fgets(line, sizeof(line), file))
      break;
    trim(line);
    sscanf(line, "%c %d (#%02x%02x%02x)", &segments[n].dir, &segments[n].len,
           &segments[n].r, &segments[n].g, &segments[n].b);

    PRINTF("'%s' -> %c %d (#%02x%02x%02x)\n", line, segments[n].dir,
           segments[n].len, segments[n].r, segments[n].g, segments[n].b);
    ++n;
  } /* loop over lines */
  fclose(file);
  segments_num = n;
  PUTS("Input data were read.");
  return EXIT_SUCCESS;
} /* read_input_data(.) */
/******************************************************************************/
void make_polygone(void) {
  polygone[0] = (point_t){.x = 0, .y = 0};
  min_x = min_y = INT32_MAX;
  max_x = max_y = -INT32_MAX;
  for (size_t i = 0; i < segments_num; ++i) {
    point_t p = polygone[i];
    const int len = segments[i].len;
    switch (segments[i].dir) {
    case 'L':
      p.x -= len;
      break;
    case 'R':
      p.x += len;
      break;
    case 'U':
      p.y -= len;
      break;
    case 'D':
      p.y += len;
      break;
    } /* switch */
    polygone[i + 1] = p;
    min_x = (p.x < min_x) ? p.x : min_x;
    max_x = (max_x < p.x) ? p.x : max_x;
    min_y = (p.y < min_y) ? p.y : min_y;
    max_y = (max_y < p.y) ? p.y : max_y;
  } /* loop */
  PRINTF("last point: (%d, %d)\n", polygone[segments_num].x,
         polygone[segments_num].y);
  PRINTF("min_x=%d max_x=%d  min_y=%d max_y=%d\n", min_x, max_x, min_y, max_y);
} /* make_polygone() */
/******************************************************************************/
double angle_between_two_vectors(const double x1, const double y1,
                                 const double x2, const double y2) {
  const double dot = x1 * x2 + y1 * y2;
  const double det = x1 * y2 - y1 * x2;
  return atan2(det, dot);
} /* angle_between_two_vectors(....) */
/******************************************************************************/
void shift_by(const int dx, const int dy) {
  for (size_t i = 0; i <= segments_num; ++i) {
    polygone[i].x += dx;
    polygone[i].y += dy;
  }
} /* shift_by(..) */
/******************************************************************************/
bool inside(const int x, const int y) {
  double total_angle = 0.0;
  for (size_t i = 1; i <= segments_num; ++i) {
    const double prev_vx = polygone[i - 1].x - x;
    const double prev_vy = polygone[i - 1].y - y;
    const double curr_vx = polygone[i].x - x;
    const double curr_vy = polygone[i].y - y;
    const double angle =
        angle_between_two_vectors(prev_vx, prev_vy, curr_vx, curr_vy);
    total_angle += angle;
  }
  const bool ans = fabs(total_angle - 2.0 * M_PI) < EPS;
  PRINTF("(%d,%d) total_anlge=%f  %c\n", x, y, total_angle, ans ? '#' : '.');
  return ans;
} /* inside(..) */
/******************************************************************************/
void solve_part1(void) {
  make_polygone();
  const size_t field_width = max_x - min_x + 1;
  const size_t field_height = max_y - min_y + 1;

  bool field[field_height][field_width];
  memset(field, false, sizeof(field));
  int x = 0, y = 0;
  for (size_t i = 0; i < segments_num; ++i) {
    int dx = 0, dy = 0;
    const int len = segments[i].len;
    switch (segments[i].dir) {
    case 'L':
      dx = -1;
      break;
    case 'R':
      dx = +1;
      break;
    case 'U':
      dy = -1;
      break;
    case 'D':
      dy = +1;
      break;
    } /* switch */
    for (int j = 0; j < len; ++j) {
      field[y - min_y][x - min_x] = true;
      x += dx;
      y += dy;
    }
  } /* loop */
  print_field(&field[0][0], field_width, field_height);
  PUTS("__________________________________________________");

  int sum = 0;
  for (int y = min_y; y <= max_y; ++y) {
    for (int x = min_x; x <= max_x; ++x) {
      bool is_inside = field[y - min_y][x - min_x] || inside(x, y);
      field[y - min_y][x - min_x] = is_inside;
      sum += is_inside;
    } /* x-loop */
    PRINTF("\n");
  } /* y-loop */
  print_field(&field[0][0], field_width, field_height);
  printf("%d\n", sum);
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
