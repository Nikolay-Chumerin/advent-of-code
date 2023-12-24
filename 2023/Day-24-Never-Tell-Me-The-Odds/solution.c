#define _GNU_SOURCE
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRING_CAPACITY (1024)
#define LINES_CAPACITY (512)
#define EPS (1E-8)
#define ABS(x) (x < 0 ? -x : x)
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

#define TEST_AREA_MIN_COORD_EXAMPLE (7)
#define TEST_AREA_MAX_COORD_EXAMPLE (27)
#define TEST_AREA_MIN_COORD (200000000000000)
#define TEST_AREA_MAX_COORD (400000000000000)

typedef long double coord_t;
#define COORD_FMT "%Lf"
typedef struct {
  coord_t x;
  coord_t y;
  coord_t z;
} point_t;

typedef point_t dir_t;
typedef struct {
  point_t point;
  dir_t dir;
} line_t;

typedef struct {
  point_t corner0;
  point_t corner1;
} test_area_t;

test_area_t test_area = {
    .corner0 = {.x = TEST_AREA_MIN_COORD, .y = TEST_AREA_MIN_COORD},
    .corner1 = {.x = TEST_AREA_MAX_COORD, .y = TEST_AREA_MAX_COORD}};

line_t lines[LINES_CAPACITY];
size_t lines_num = 0;
/******************************************************************************/
#ifdef DEBUG
void print_data(void) { /* */
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
bool inside_test_area(const point_t *p) {
  return (test_area.corner0.x <= p->x) && (p->x <= test_area.corner1.x) &&
         (test_area.corner0.y <= p->y) && (p->y <= test_area.corner1.y);
} /* inside_test_area */
/******************************************************************************/
bool intersect_lines(const line_t *l1, /* [in] line 1 */
                     const line_t *l2, /* [in] line 2 */
                     point_t *p        /* [out] intersection point */
) {
  /* l1 || l2 <=> d1.x / d1.y == d2.x / d2.y <=> d1.x * d2.y == d2.x * d1.y */
  const coord_t d = l1->dir.x * l2->dir.y - l2->dir.x * l1->dir.y;
  bool lines_intersect = ABS(d) > EPS;
  if (!lines_intersect)
    return false;
  /*
    x = x1 + vx1*t1 == x2 + vx2*t2
    y = y1 + vy1*t1 == y2 + vy2*t2

    d = vx1 * vy2 - vy1 * vx2
    t1 = ((x1 - x2)*(-vy2) - (y1 - y2)*(-vx2)) / ((-vx1)*(-vy2) - (-vy1)*(-vx2))
       = (vy2*(x2 - x1) + vx2*(y1 - y2)) / d

    t2 = ((x1 - x2)*(-vy1) - (y1 - y2)*(-vx1)) / d
       = (vy1*(x2 - x1) + vx1*(y1 - y2)) / d

  */
  const coord_t t1 = (l2->dir.y * (l2->point.x - l1->point.x) +
                      l2->dir.x * (l1->point.y - l2->point.y)) /
                     d;

  if (t1 < 0)
    return false;

  const coord_t t2 = (l1->dir.y * (l2->point.x - l1->point.x) +
                      l1->dir.x * (l1->point.y - l2->point.y)) /
                     d;
  if (t2 < 0)
    return false;
  p->x = l1->point.x + t1 * l1->dir.x;
  p->y = l1->point.y + t1 * l1->dir.y;
  return true;
} /* intersect_lines */
/******************************************************************************/
int read_input_data(const char *input_file_path) {
  PRINTF("Reading input data from the '%s' file.\n", input_file_path);
  char text_line[STRING_CAPACITY];
  FILE *file = fopen(input_file_path, "r");
  if (!file) {
    perror("Error opening the file");
    return (EXIT_FAILURE);
  }
  while (true) {
    if (!fgets(text_line, sizeof(text_line), file))
      break;
    trim(text_line);
    line_t *l = &lines[lines_num++];
    sscanf(text_line,
           COORD_FMT ", " COORD_FMT ", " COORD_FMT " @ " COORD_FMT
                     ", " COORD_FMT ", " COORD_FMT,
           &(l->point.x), &(l->point.y), &(l->point.z), &(l->dir.x),
           &(l->dir.y), &(l->dir.z));
    PRINTF("'%s' -> " COORD_FMT ", " COORD_FMT ", " COORD_FMT " @ " COORD_FMT
           ", " COORD_FMT ", " COORD_FMT "\n",
           text_line, l->point.x, l->point.y, l->point.z, l->dir.x, l->dir.y,
           l->dir.z);
    /* process line ... */
  } /* loop over lines */
  fclose(file);
  PUTS("Input data were read.");
  return EXIT_SUCCESS;
} /* read_input_data(.) */
/******************************************************************************/
void solve_part1(void) {
  int ans = 0;
  point_t intersection_point;
  for (size_t i = 0; i < lines_num - 1; ++i) {
    const line_t *li = &lines[i];
    for (size_t j = i + 1; j < lines_num; ++j) {
      const line_t *lj = &lines[j];
      if (intersect_lines(li, lj, &intersection_point)) {
        const bool inside = inside_test_area(&intersection_point);
        ans += inside;
        PRINTF("lines %lu and %lu intersect in point (" COORD_FMT ", " COORD_FMT
               ", " COORD_FMT ") which is %s of the test aread\n",
               i, j, intersection_point.x, intersection_point.y,
               intersection_point.z, inside ? "inside" : "outside");
      }
    } /* j-loop */
  }   /* i-loop */

  printf("%d\n", ans);
} /* solve_part1() */
/******************************************************************************/
void solve_part2(void) {
  /* solution of the part2 */
  printf("%d\n", 0);
} /* solve_part2() */
/******************************************************************************/
int main(int argc, char *argv[]) {
  const char *input_file_path = (argc > 1) ? argv[1] : DEFAULT_INPUT_FILE;
  if (strcmp(input_file_path, "input.txt")) {
    test_area = (test_area_t){.corner0 = {.x = TEST_AREA_MIN_COORD_EXAMPLE,
                                          .y = TEST_AREA_MIN_COORD_EXAMPLE},
                              .corner1 = {.x = TEST_AREA_MAX_COORD_EXAMPLE,
                                          .y = TEST_AREA_MAX_COORD_EXAMPLE}};
  }

  PRINTF("test_area = [(" COORD_FMT ", " COORD_FMT ")-(" COORD_FMT
         ", " COORD_FMT ")]",
         test_area.corner0.x, test_area.corner0.y, test_area.corner1.x,
         test_area.corner1.y);
  if (EXIT_SUCCESS != read_input_data(input_file_path)) {
    perror("Error reading input data from file");
    return (EXIT_FAILURE);
  }

  solve_part1();
  solve_part2();

  return EXIT_SUCCESS;
}
