#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRING_CAPACITY (8192)
#define SEEDS_CAPACITY (256)
#define DEFAULT_INPUT_FILE "input.txt"

#define STAGES_NUM (8)
const char *stage_names[STAGES_NUM] = {"seed",     "soil",    "fertilizer",
                                       "water",    "light",   "temperature",
                                       "humidity", "location"};

#define MAPS_NUM (STAGES_NUM - 1)
#define MAP_CAPACITY (256)
char *s = NULL;
typedef struct {
  int64_t beg; /* The source range first index */
  int64_t end; /* The source range last (inclusive) index */
  int64_t dif; /* Difference (signed): dst_idx = dif + src_idx */
} map_entry_t;

typedef struct {
  map_entry_t map[MAP_CAPACITY];
  size_t size;
} map_t;

map_t maps[MAPS_NUM];
int64_t seeds[SEEDS_CAPACITY];
size_t seeds_num = 0;
FILE *file;

/******************************************************************************/
void trim(char *line) {
  int line_len = strlen(line);
  while (line_len && (line[line_len - 1] <= ' ')) {
    line[--line_len] = 0;
  }
}  /* trim(.) */
/******************************************************************************/
void read_seeds(void) {
  printf("Reading seeds.");
  char line[STRING_CAPACITY];
  fgets(line, sizeof(line), file);
  trim(line);
  seeds_num = 0U;
  int seeds_read = 0;
  int chars_read = 0;
  char *str = line + 6;
  do {
    seeds_read = sscanf(str, "%ld %n", &seeds[seeds_num], &chars_read);
    if (seeds_read != 1)
      break;
    ++seeds_num;
    str += chars_read;
  } while (true);
  printf(" Done: %lu seeds were read.\n", seeds_num);
} /* read_seeds(..) */
/******************************************************************************/
void read_map(const int map_idx) {
  map_t *map = &maps[map_idx];
  char map_header_str[STRING_CAPACITY];
  char line[STRING_CAPACITY];
  sprintf(map_header_str, "%s-to-%s map:", stage_names[map_idx],
          stage_names[map_idx + 1]);

  printf("Reading '%s' ...", map_header_str);
  while (fgets(line, sizeof(line), file)) {
    if (strstr(line, map_header_str)) {
      break;
    }
  } /* loop over file lines */

  while (fgets(line, sizeof(line), file)) {
    trim(line);
    int64_t dst_start_idx, src_start_idx, range_len;
    const int n =
        sscanf(line, "%ld %ld %ld", &dst_start_idx, &src_start_idx, &range_len);
    if (n != 3)
      break;

    const int i = map->size;
    map->map[i].beg = src_start_idx;
    map->map[i].end = src_start_idx + range_len - 1;
    map->map[i].dif = dst_start_idx - src_start_idx;
    ++map->size;
  } /* loop over file lines */
  printf(" Done: %lu entries were read\n", map->size);
} /* read_map(.) */

/******************************************************************************/
int64_t re_map(const int64_t x, const map_t *map) {
  int64_t y = x;
  for (int i = 0; i < map->size; ++i) {
    if (y >= map->map[i].beg && y <= map->map[i].end) {
      y += map->map[i].dif;
      break;
    }
  }
  return y;
} /* re_map */
/******************************************************************************/
int64_t process(const int64_t seed) {
  int64_t x = seed;
  for (int i = 0; i < MAPS_NUM; ++i) {
    x = re_map(x, &maps[i]);
  }
  return x;
} /* process(.) */
/******************************************************************************/
void part1(void) {
  int64_t min_location = __LONG_MAX__;
  for (int i = 0; i < seeds_num; ++i) {
    const int64_t location = process(seeds[i]);
    if (min_location > location) {
      min_location = location;
    }
  }
  printf("min_location = %li\n", min_location);
} 
/******************************************************************************/
int main(int argc, char *argv[]) {
  file = fopen((argc > 1) ? argv[1] : DEFAULT_INPUT_FILE, "r");
  if (!file) {
    perror("Error opening the file");
    return EXIT_FAILURE;
  }
  read_seeds();
  for (int map_idx = 0; map_idx < MAPS_NUM; ++map_idx) {
    read_map(map_idx);
  }
  fclose(file);

  part1();

  return EXIT_SUCCESS;
}
