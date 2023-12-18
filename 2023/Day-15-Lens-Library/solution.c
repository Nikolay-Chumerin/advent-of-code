#define _GNU_SOURCE
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRING_CAPACITY (32768)
#define HASHMAP_SIZE (256)
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

// typedef union {
//   char text[8];
//   uint64_t code;
// } label_t;

typedef struct {
  int len;
  union {
    char text[8];
    uint64_t code;
  } data;
} label_t;
typedef struct {
  label_t label;
  int focal_length;
} lens_t;

typedef struct node {
  lens_t lens;
  struct node *next;
} node_t;

node_t *hashmap[HASHMAP_SIZE] = {NULL};

char s[STRING_CAPACITY];

/******************************************************************************/
#ifdef DEBUG
#define PRINT_LENS                                                             \
  {                                                                            \
    printf(                                                                    \
        "lens: label{.len=%d .data={.code=%lu .text='%s'} focal_length=%d)\n", \
        lens.label.len, lens.label.data.code, lens.label.data.text,            \
        lens.focal_length);                                                    \
  }

void print_hashmap(void) {
  puts("-[hashmap]-----------------------------");
  for (int i = 0; i < HASHMAP_SIZE; ++i) {
    node_t *node = hashmap[i];
    if (!node)
      continue;
    printf("box %03d: ", i);
    while (node) {
      printf("[%s %d] ", node->lens.label.data.text, node->lens.focal_length);
      node = node->next;
    }
    printf("\n");
  }
  puts("---------------------------------------");
}
#else
#define PRINT_LENS                                                             \
  {}
#define print_hashmap(_)                                                       \
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
    h %= HASHMAP_SIZE;
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
  char *p = s;
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
    h %= HASHMAP_SIZE;
  }
  ans += h;
  printf("%d\n", ans);
} /* solve_part1() */
/******************************************************************************/
void build_hashmap(void) {
  int h = 0;
  // label_t l = {.code = 0};
  lens_t lens = {.label.len = 0, .label.data.code = 0, .focal_length = 0};
  node_t *node = NULL;
  node_t *prev_node = NULL;

  for (char *p = s; *p; ++p) {
    const char c = *p;
    switch (c) {
    case 'a' ... 'z':
      PRINTF("Appending '%c' to the current lense label hash=%d->", c, h);
      h += c;
      h *= 17;
      h %= HASHMAP_SIZE;
      PRINTF("%d\n", h);
      PRINT_LENS;
      lens.label.data.text[lens.label.len++] = c;
      PUTS("After appending...");
      PRINT_LENS;
      break;

    case '-':
      PUTS("Remove current lense from hashmap.");
      PRINT_LENS;
      PRINTF("h=%d\n", h);
      node = hashmap[h];
      PRINTF("node=%p\n", node);
      prev_node = NULL;
      while (node && (node->lens.label.data.code != lens.label.data.code)) {
        prev_node = node;
        node = node->next;
        PRINTF("node=%p\n", node);
      }
      PRINTF("node=%p prev_node=%p\n", node, prev_node);
      if (node) { /* found */
        PRINTF("Found something: node=%p node->lens.label='%s'\n", node,
               node->lens.label.data.text);
        PRINTF("prev_node=%p\n", prev_node);
        if (prev_node) {
          PRINTF("prev_node->lens.label='%s'\n",
                 prev_node->lens.label.data.text);
          prev_node->next = node->next;
        } else {
          PRINTF("hashmap[%d] = node->next\n", h);
          hashmap[h] = node->next;
        }
        PUTS("Freeing node...");
        free(node);
        node = NULL;
      }
      break;

    case '=':
      PUTS("Store lens to hashmap.");
      PRINT_LENS;
      /* read lens focal length */
      ++p;
      const char focal_length_char = *p;
      PRINTF("focal_length_char=%c\n", focal_length_char);
      lens.focal_length = focal_length_char - '0';
      PRINT_LENS;

      /* search for lens label in the hashmap */
      PRINTF("search for lens label (%s) in the hashmap\n",
             lens.label.data.text);
      node = hashmap[h];
      prev_node = NULL;
      while (node && node->lens.label.data.code != lens.label.data.code) {
        prev_node = node;
        node = node->next;
      }
      PRINTF("Search finished node=%p  prev_node=%p\n", node, prev_node);
      if (node) {
        PUTS("Updating the lens' focal length...");
        node->lens.focal_length = lens.focal_length;
      } else {
        PUTS("Creating a new node...");
        node = malloc(sizeof(node_t));
        node->lens = lens;
        node->next = NULL;
        PRINTF("Adding the current lense to the end of the list, which is ");
        if (prev_node && hashmap[h]) {
          PUTS("prev_node->next");
          prev_node->next = node;
        } else {
          PRINTF("hashmap[%d]\n", h);
          hashmap[h] = node;
        }
      }
      break;

    case ',':
      PUTS("Move to the next lens.");
      PRINT_LENS;
      h = 0;
      lens = (lens_t){.label.len = 0, .label.data.code = 0, .focal_length = 0};
      PUTS("After clearing...");
      PRINT_LENS;
      break;
    } /* switch */
    print_hashmap();
  } /* loop over input string */
} /* build_hashmap() */
/******************************************************************************/
int focusing_power(void) {
  int ans = 0;
  for (int i = 0; i < HASHMAP_SIZE; ++i) {
    if (!hashmap[i])
      continue;
    const int box_number = i + 1;
    int lense_idx = 1;
    node_t *node = hashmap[i];
    while (node) {
      ans += box_number * lense_idx * node->lens.focal_length;
      node = node->next;
      ++lense_idx;
    }
  }
  return ans;
} /* focusing_power() */
/******************************************************************************/
void solve_part2(void) {
  memset(hashmap, 0, sizeof(hashmap));
  build_hashmap();
  int ans = focusing_power();
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
