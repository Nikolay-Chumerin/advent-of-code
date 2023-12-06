#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRING_CAPACITY (8192)
#define RANGES_CAPACITY (16384)
#define SEEDS_CAPACITY (256)
#ifdef DEBUG
#define PUTS puts
#define PRINTF printf
#else
#define PUTS(a) (0)
#define PRINTF(fmt, ...) (0) 
#endif // DEBUG

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

typedef struct {
  int64_t beg; /* The source range first index */
  int64_t end; /* The source range last (inclusive) index */
} range_t;
typedef struct {
  range_t list[RANGES_CAPACITY];
  size_t size;
} ranges_list_t;
ranges_list_t range_lists[2];

FILE *file;

/******************************************************************************/
void trim(char *line) {
  int line_len = strlen(line);
  while (line_len && (line[line_len - 1] <= ' ')) {
    line[--line_len] = 0;
  }
} /* trim(.) */
/******************************************************************************/
void read_seeds(void) {
  PRINTF("Reading seeds.");
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
  PRINTF(" Done: %lu seeds were read.\n", seeds_num);
} /* read_seeds(..) */
/******************************************************************************/
void read_map(const int map_idx) {
  map_t *map = &maps[map_idx];
  char map_header_str[STRING_CAPACITY];
  char line[STRING_CAPACITY];
  sprintf(map_header_str, "%s-to-%s map:", stage_names[map_idx],
          stage_names[map_idx + 1]);

  PRINTF("Reading '%s' ...", map_header_str);
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
  PRINTF(" Done: %lu entries were read\n", map->size);
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
int64_t propagate(const int64_t seed) {
  int64_t x = seed;
  for (int i = 0; i < MAPS_NUM; ++i) {
    x = re_map(x, &maps[i]);
  }
  return x;
} /* propagate(.) */
/******************************************************************************/
void part1(void) {
  int64_t min_location = __LONG_MAX__;
  for (int i = 0; i < seeds_num; ++i) {
    const int64_t location = propagate(seeds[i]);
    if (min_location > location) {
      min_location = location;
    }
  }
  printf("min_location = %li\n", min_location);
}
/******************************************************************************/
void print_range_list(const ranges_list_t *rl) {
  PRINTF("size=%lu: ", rl->size);
  for (size_t i = 0; i < rl->size; ++i) {
    PRINTF("[%li..%li] ", rl->list[i].beg, rl->list[i].end);
  }
  PRINTF("\n");
} /* print_range_list(.) */
/******************************************************************************/
int compare_ranges(const void *a, const void *b) {
  const range_t ra = *(range_t *)a;
  const range_t rb = *(range_t *)b;
  if (ra.beg == rb.beg) {
    if (ra.end == rb.end)
      return 0;
    if (ra.end < rb.end)
      return -1;
    return 1;
  } else if (ra.beg < rb.beg) {
    return -1;
  }
  return 1;
} /* compare_ranges(..) */
/******************************************************************************/
void sort_range_list(ranges_list_t *ranges_list) {
  qsort(ranges_list->list, ranges_list->size, sizeof(range_t), compare_ranges);
} /* sort_range_list(.) */
/******************************************************************************/
void merge_ranges(ranges_list_t *ranges_list) {
  sort_range_list(ranges_list);
  // PUTS("After sorting...");
  // print_range_list(ranges_list);
  for (size_t i = 0; i < ranges_list->size - 1; ++i) {
    range_t ri = ranges_list->list[i];
    if (ri.beg < 0)
      continue;
    for (size_t j = i + 1; j < ranges_list->size; ++j) {
      range_t rj = ranges_list->list[j];
      if (rj.beg < 0)
        continue;
      if (rj.beg <= ri.end) {
        if (rj.beg < ri.beg) {
          ri.beg = rj.beg;
        }
        if (ri.end < rj.end) {
          ri.end = rj.end;
        }
        ranges_list->list[j].beg = -1;
      }
    } /* j-loop */
    ranges_list->list[i] = ri;
  } /* i-loop */

  // PUTS("After merging, but not prunning...");
  // print_range_list(ranges_list);

  size_t j = 0;
  for (size_t i = 0; i < ranges_list->size; ++i) {
    if (ranges_list->list[i].beg >= 0) {
      ranges_list->list[j++] = ranges_list->list[i];
    }
  } /* i-loop */
  PRINTF("j=%lu\n", j);
  ranges_list->size = j;

  PUTS("After merging and prunning...");
  print_range_list(ranges_list);

} /* merge_ranges */
/******************************************************************************/
void split_ranges(const size_t src_idx, const map_t *map) {
  PUTS("Splitting ranges...");
  const size_t dst_idx = src_idx ^ 1;
  const size_t src_ranges_num = range_lists[src_idx].size;

  ranges_list_t *src_ranges_list = &range_lists[src_idx];
  range_t *src_ranges = src_ranges_list->list;

  ranges_list_t *dst_ranges_list = &range_lists[dst_idx];
  range_t *dst_ranges = dst_ranges_list->list;
  size_t dst_range_idx = 0;

  for (size_t src_range_idx = 0; src_range_idx < src_ranges_list->size;
       ++src_range_idx) {
    int64_t beg = src_ranges[src_range_idx].beg;
    int64_t end = src_ranges[src_range_idx].end;
    bool store_original_range = true;
    for (size_t map_range_idx = 0; map_range_idx < map->size; ++map_range_idx) {
      const int64_t mbeg = map->map[map_range_idx].beg;
      const int64_t mend = map->map[map_range_idx].end;
      const int64_t mdif = map->map[map_range_idx].dif;

      /* Notations: (-mbeg )-mend  [-beg ]-end */

      if ((mend < beg || end < mbeg)) { /* (1)[2] or [3](4) */
        /* All ranges (1), [2], [3] and (4) can be dropped for now.
        The original range [2] or [3] should be stored only if no other
        remapping happens to it. */
        continue;
      }

      if (mbeg <= beg && end <= mend) { /* or (1[2]3) */
        /* ranges (1[ and ]3) can be dropped */
        /* range [2] should be remapped */
        PRINTF("store original [%lu..%lu]\n", beg, end);
        dst_ranges[dst_range_idx].beg = beg;
        dst_ranges[dst_range_idx].end = end;
        ++dst_range_idx;
        store_original_range = false;
        // continue;
        break;
      }

      if (mbeg <= beg && mend <= end) { /* (1[2)3] */
        PRINTF("(%li [%li %li) %li]: ", mbeg, beg, mend, end);
        /* range (1[ can be dropped */
        /* range [2) should be remapped */
        PRINTF("[%lu..%lu]-to2mapped ", beg, mend);
        dst_ranges[dst_range_idx].beg = beg;
        dst_ranges[dst_range_idx].end = mend;
        ++dst_range_idx;
        if (mend < end) {
          /* range )3] should be stored as it is*/
          PRINTF("[%lu..%lu]-to2stored\n", mend + 1, end);
          dst_ranges[dst_range_idx].beg = mend + 1;
          dst_ranges[dst_range_idx].end = end;
          ++dst_range_idx;
        }
        beg = mend + 1;               /* [2)3] -> [2)[3] -> [3] */
        store_original_range = false; /* Don't store original range later */
        continue;
      } /* (1[2)3] branch */

      if (beg <= mbeg && end <= mend) { /* [1(2]3) */
        PRINTF("[%li (%li %li] %li): ", beg, mbeg, end, mend);

        /* range [1(, if it exists, should be stored as it is (no remapping) */
        if (beg < mbeg) {
          PRINTF("[%lu..%lu] ", beg, mbeg - 1);
          dst_ranges[dst_range_idx].beg = beg;
          dst_ranges[dst_range_idx].end = mbeg - 1;
          ++dst_range_idx;
        }

        /* range (2] should be remapped later */
        PRINTF("[%lu..%lu]\n", mbeg, end);
        dst_ranges[dst_range_idx].beg = mbeg;
        dst_ranges[dst_range_idx].end = end;
        ++dst_range_idx;
        store_original_range = false; /* Don't store original range later */

        end = mbeg - 1; /* [1(2] -> [1](2] -> [1] */
        continue;
      } /* [1(2]3) branch */

      if (beg <= mbeg && mend <= end) { /* [1(2)3] */
        PRINTF("[%li (%li %li) %li]: ", beg, mbeg, mend, end);
        if (beg < mbeg) {
          PRINTF("[%lu..%lu] ", beg, mbeg - 1);
          dst_ranges[dst_range_idx].beg = beg;
          dst_ranges[dst_range_idx].end = mbeg - 1;
          ++dst_range_idx;
        }
        PRINTF("[%lu..%lu]\n", mbeg, mend);
        dst_ranges[dst_range_idx].beg = mbeg;
        dst_ranges[dst_range_idx].end = mend;
        ++dst_range_idx;
        if (mend < end) {
          PRINTF("[%lu..%lu]\n", mend + 1, end);
          dst_ranges[dst_range_idx].beg = mend + 1;
          dst_ranges[dst_range_idx].end = end;
          ++dst_range_idx;
        }
        if ((beg < mbeg) && (mend < end)) {
          /* Both ranges [1( and )3] exist... */
          /* Append  )3] to the list of source ranges */
          const size_t new_range_idx = src_ranges_list->size++;
          src_ranges[new_range_idx].beg = mend + 1;
          src_ranges[new_range_idx].end = end;

          /* Shrink the current range [1(2)3] to [1] */
          end = mbeg - 1; /* [1(2] -> [1](2] -> [1] */
        } else if (beg < mbeg) {
          /* Only range [1( exists... */
          /* Shrink the current range [1(2)3] to [1] */
          end = mbeg - 1; /* [1(2] -> [1](2] -> [1] */
        } else {
          /* Only range )3] exists... */
          /* Shrink the current range [1(2)3] to [3] */
          beg = mend + 1;
        }
        store_original_range = false;
      } /* [1(2)3] branch */
    }   /* Loop over map ranges */

    if (store_original_range) {
      /* no overlaps of original range with map-ranges found */
      PRINTF("store original range [%lu..%lu].\n", beg, end);
      dst_ranges[dst_range_idx].beg = beg;
      dst_ranges[dst_range_idx].end = end;
      ++dst_range_idx;
    } /* store_original_range branch */
  }   /* Loop over src ranges */
  range_lists[dst_idx].size = dst_range_idx;
  PRINTF("split into %lu ranges.\n", dst_range_idx);
} /* split_ranges()  */
/******************************************************************************/
void map_ranges(const size_t src_idx, const map_t *map) {
  PUTS("Re-mapping ranges...");
  const size_t dst_idx = src_idx ^ 1;
  const size_t src_ranges_num = range_lists[src_idx].size;

  const ranges_list_t *src_ranges_list = &range_lists[src_idx];
  const range_t *src_ranges = src_ranges_list->list;

  ranges_list_t *dst_ranges_list = &range_lists[dst_idx];
  range_t *dst_ranges = dst_ranges_list->list;
  size_t dst_range_idx = 0;

  for (size_t range_idx = 0; range_idx < src_ranges_num; ++range_idx) {
    int64_t beg = src_ranges[range_idx].beg;
    int64_t end = src_ranges[range_idx].end;
    bool remapping_happened = false;
    for (size_t map_range_idx = 0; map_range_idx < map->size; ++map_range_idx) {
      const int64_t mbeg = map->map[map_range_idx].beg;
      const int64_t mend = map->map[map_range_idx].end;
      const int64_t mdif = map->map[map_range_idx].dif;
      // PRINTF("Intersecting source range [%lu, %lu] with map range (%lu, %lu):
      // ",
      //        beg, end, mbeg, mend);

      /* (-mbeg )-mend  [-beg ]-end */
      if ((mend < beg || end < mbeg)) { /* ()[]  or []() */
        // PRINTF("skipping.\n");
        continue;
      }

      if (mbeg <= beg && end <= mend) { /* or ([]) */
        PRINTF("map source range [%lu..%lu]->[%lu..%lu]\n", beg, end,
               beg + mdif, end + mdif);
        dst_ranges[dst_range_idx].beg = beg + mdif;
        dst_ranges[dst_range_idx].end = end + mdif;
        remapping_happened = true;
        ++dst_range_idx;
        break;
      }

      if (mbeg <= beg && mend <= end) { /* ([)] */
        PRINTF("!!!(%li [%li %li) %li]: ", mbeg, beg, mend, end);
        PRINTF("[%lu..%lu]->[%lu..%lu] ", beg, mend, beg + mdif, mend + mdif);
        dst_ranges[dst_range_idx].beg = beg + mdif;
        dst_ranges[dst_range_idx].end = mend + mdif;
        remapping_happened = true;
        ++dst_range_idx;

        PRINTF("[%lu..%lu]->[%lu..%lu]\n", mend + 1, end, mend + 1, end);
        dst_ranges[dst_range_idx].beg = mend + 1;
        dst_ranges[dst_range_idx].end = end;
        beg = mend + 1;
        ++dst_range_idx;
        continue;
      }

      if (beg <= mbeg && end <= mend) { /* [(]) */
        PRINTF("!!![%li (%li %li] %li): ", beg, mbeg, end, mend);
        if (beg < mbeg) {
          PRINTF("[%lu..%lu]->[%lu..%lu] ", beg, mbeg - 1, beg, mbeg - 1);
          dst_ranges[dst_range_idx].beg = beg;
          dst_ranges[dst_range_idx].end = mbeg - 1;
          ++dst_range_idx;
        }
        PRINTF("[%lu..%lu]->[%lu..%lu]\n", mbeg, end, mbeg + mdif, end + mdif);
        dst_ranges[dst_range_idx].beg = mbeg + mdif;
        dst_ranges[dst_range_idx].end = end + mdif;
        remapping_happened = true;
        ++dst_range_idx;
        end = mbeg - 1;
        continue;
      }

      if (beg <= mbeg && mend <= end) { /* [()]  */
        PRINTF("!!!!![%li (%li %li) %li]: ", beg, mbeg, mend, end);
        if (beg < mbeg) {
          PRINTF("[%lu..%lu]->[%lu..%lu] ", beg, mbeg - 1, beg, mbeg - 1);
          dst_ranges[dst_range_idx].beg = beg;
          dst_ranges[dst_range_idx].end = mbeg - 1;
          ++dst_range_idx;
        }
        PRINTF("[%lu..%lu]->[%lu..%lu]\n", mbeg, mend, mbeg + mdif,
               mend + mdif);
        dst_ranges[dst_range_idx].beg = mbeg + mdif;
        dst_ranges[dst_range_idx].end = mend + mdif;
        remapping_happened = true;
        ++dst_range_idx;
        if (mend < end) {
          PRINTF("[%lu..%lu]->[%lu..%lu]\n", mend + 1, end, mend + 1, end);
          dst_ranges[dst_range_idx].beg = mend + 1;
          dst_ranges[dst_range_idx].end = end;
          ++dst_range_idx;
        }
      }
    } /* Loop over map ranges */
    if (!remapping_happened) {
      PRINTF("store source range [%lu..%lu].\n", beg, end);
      dst_ranges[dst_range_idx].beg = beg;
      dst_ranges[dst_range_idx].end = end;
      ++dst_range_idx;
    }
  } /* Loop over src ranges */
  range_lists[dst_idx].size = dst_range_idx;
  PRINTF("split into %lu ranges.\n", dst_range_idx);
} /* map_ranges()  */
/******************************************************************************/
void part2(void) {
  int64_t min_location = __LONG_MAX__;
  memset(range_lists, 0, sizeof(range_lists));
  int curr = 1;
  int prev = 0;

  for (size_t i = 0; i < seeds_num; i += 2) {
    const size_t idx = range_lists[prev].size;
    range_lists[prev].list[idx].beg = seeds[i];
    range_lists[prev].list[idx].end = seeds[i] + seeds[i + 1] - 1;
    ++range_lists[prev].size;
  }
  PRINTF("before merging: curr=%d (%lu intervals)\n", curr,
         range_lists[prev].size);
  merge_ranges(&range_lists[prev]);
  PRINTF("after merging: curr=%d (%lu intervals)\n", curr,
         range_lists[prev].size);

  for (size_t i = 0; i < MAPS_NUM; ++i) {
    PUTS("-------------------------------------------------------");
    PRINTF("mapping from %s to %s\n", stage_names[i], stage_names[i + 1]);
    PRINTF("i=%lu prev=%d (%lu intervals) curr=%d (%lu intervals)\n", i, prev,
           range_lists[prev].size, curr, range_lists[curr].size);
    print_range_list(&range_lists[prev]);
    split_ranges(prev, &maps[i]); /* prev -> curr */
    sort_range_list(&range_lists[curr]);
    PUTS("Ranges after splitting...");
    print_range_list(&range_lists[curr]);

    map_ranges(curr, &maps[i]); /* curr -> prev */
    merge_ranges(&range_lists[prev]);
    PRINTF("after merging: prev=%d (%lu intervals)\n", prev,
           range_lists[prev].size);
    // prev = curr;
    // curr ^= 1;
  }
  min_location = range_lists[prev].list[0].beg;
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
  part2();

  return EXIT_SUCCESS;
}
