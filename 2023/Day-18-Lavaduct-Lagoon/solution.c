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
  unsigned int rgb;
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
    sscanf(line, "%c %d (#%06x)", &segments[n].dir, &segments[n].len,
           &segments[n].rgb);

    PRINTF("'%s' -> %c %d (#%06x)\n", line, segments[n].dir, segments[n].len,
           segments[n].rgb);
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
void convert_rgb_to_segments() {
  const char idx2dir[4] = "RDLU";
  for (size_t i = 0; i < segments_num; ++i) {
    unsigned int rgb = segments[i].rgb;
    segments[i].len = rgb >> 4;
    segments[i].dir = idx2dir[rgb & 0x03];
    PRINTF("'%06x' -> %c %d\n", segments[i].rgb, segments[i].dir,
           segments[i].len);
  }
} /* convert_rgb_to_segments(..) */
/******************************************************************************/
bool inside(const double x, const double y) {
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
  PRINTF("(%f,%f) total_anlge=%f  %c\n", x, y, total_angle, ans ? '#' : '.');
  return ans;
} /* inside(..) */
/******************************************************************************/
void solve_part1(void) {
  make_polygone();
  const size_t field_width = max_x - min_x + 1;
  const size_t field_height = max_y - min_y + 1;
  PRINTF("field_width=%lu  field_height=%lu\n", field_width, field_height);
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
int comp(const void *a, const void *b) { return *(int *)a - *(int *)b; }
/******************************************************************************/
size_t get_unique(int *arr, const size_t len) {
  qsort(arr, len, sizeof(int), comp);
  size_t out_len = 1;
  for (size_t i = 1; i < len; ++i) {
    if (arr[i] != arr[out_len - 1]) {
      arr[out_len++] = arr[i];
    }
  }
  return out_len;
} /* get_unique(..) */
/******************************************************************************/
void solve_part2(void) {
  convert_rgb_to_segments();
  make_polygone();
  PRINTF("segments_num=%lu\n", segments_num);
  int unique_x[segments_num + 1];
  int unique_y[segments_num + 1];
  for (size_t i = 0; i < segments_num; ++i) {
    unique_x[i] = polygone[i].x;
    unique_y[i] = polygone[i].y;
  }
  const size_t unique_x_num = get_unique(unique_x, segments_num);
  const size_t unique_y_num = get_unique(unique_y, segments_num);
  PRINTF("unique_x_num=%lu\n", unique_x_num);
  PRINTF("unique_y_num=%lu\n", unique_y_num);

  bool in_tile[unique_y_num + 1][unique_x_num + 1];
  memset(in_tile, false, sizeof(in_tile));

  unsigned long ans = 0;
  for (size_t r = 0; r < unique_y_num - 1; ++r) {
    const long dy = unique_y[r + 1] - unique_y[r] + 1;
    const double cy = (double)(unique_y[r + 1] + unique_y[r]) / 2.0;
    for (size_t c = 0; c < unique_x_num - 1; ++c) {
      const long dx = unique_x[c + 1] - unique_x[c] + 1;
      const double cx = (double)(unique_x[c] + unique_x[c + 1]) / 2.0;
      in_tile[r][c] = inside(cx, cy);
      ans += in_tile[r][c] * dx * dy;
    } /* c-loop*/
  }   /* r-loop */

  /* check doubled horizontal segments (interior) */
  for (size_t c = 0; c < unique_x_num; ++c) {
    const long dx = unique_x[c + 1] - unique_x[c] - 1;
    for (size_t r = 1; r < unique_y_num; ++r) {
      if (in_tile[r][c] && in_tile[r - 1][c]) {
        ans -= dx;
      }
    } /* r-loop */
  }   /* c-loop*/

  /* check doubled vertical segments (interior) */
  for (size_t r = 0; r < unique_y_num; ++r) {
    const long dy = unique_y[r + 1] - unique_y[r] - 1;
    for (size_t c = 0; c < unique_x_num; ++c) {
      if (in_tile[r][c] && in_tile[r][c + 1]) {
        ans -= dy;
      }
    } /* c-loop*/
  }   /* r-loop */

  /* check doubled vertical segments (interior) */
  for (size_t r = 0; r <= unique_y_num; ++r) {
    for (size_t c = 0; c <= unique_x_num; ++c) {
      int adj_tiles_num = in_tile[r][c] + ((r > 0) && in_tile[r - 1][c]) +
                          ((c > 0) && in_tile[r][c - 1]) +
                          ((r > 0) && (c > 0) && in_tile[r - 1][c - 1]);
      // printf("r=%lu  c=%lu  adj_tiles_num=%d\n", r, c, adj_tiles_num);
      ans -= (adj_tiles_num > 1) * (adj_tiles_num - 1);
    } /* c-loop*/
  }   /* r-loop */

  printf("%lu\n", ans);
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
