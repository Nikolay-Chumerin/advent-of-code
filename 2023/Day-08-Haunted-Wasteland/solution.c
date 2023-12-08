#define _GNU_SOURCE
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG_
#ifdef DEBUG
#define PUTS puts
#define PRINTF printf
#else
#define PUTS(a) (0)
#define PRINTF(fmt, ...) (0)
#endif // DEBUG

#define STRING_CAPACITY (512)
#define DEFAULT_INPUT_FILE "input.txt"
#define TREE_CAPACITY (1024)
#define TAG_LEN (3)

typedef char tag_t[TAG_LEN];
static const tag_t start_tag = "AAA";
static const tag_t end_tag = "ZZZ";

typedef struct {
  tag_t tag;
  size_t idx;
} kid_node_t;

typedef struct {
  tag_t tag;
  kid_node_t left;
  kid_node_t right;
} node_t;

node_t tree[TREE_CAPACITY];
size_t tree_size = 0U;
char moves[STRING_CAPACITY];
size_t moves_num = 0;
size_t start_idx = 0;
size_t end_idx = 0;

/******************************************************************************/
void trim(char *line) {
  int line_len = strlen(line);
  while (line_len && (line[line_len - 1] <= ' ')) {
    line[--line_len] = 0;
  }
} /* trim(.) */
/******************************************************************************/
#define LOAD_TAG(dst, src, idx)                                                \
  for (int i = 0; i < TAG_LEN; ++i) {                                          \
    dst[i] = src[idx + i];                                                     \
  }
int read_input_data(const char *input_file_path) {
  PRINTF("Reading input data from the '%s' file.\n", input_file_path);
  char line[STRING_CAPACITY];
  FILE *file = fopen(input_file_path, "r");
  if (!file) {
    perror("Error opening the file");
    return (EXIT_FAILURE);
  }
  fgets(moves, sizeof(moves), file);
  trim(moves);
  moves_num = strlen(moves);
  PRINTF("moves='%s'\n", moves);
  PRINTF("moves_num=%lu\n", moves_num);

  while (true) {
    if (!fgets(line, sizeof(line), file))
      break;
    trim(line);
    if (!line[0])
      continue;

    node_t *node = &tree[tree_size++];
    LOAD_TAG(node->tag, line, 0);
    LOAD_TAG(node->left.tag, line, 7);
    LOAD_TAG(node->right.tag, line, 12);
  } /* loop over lines */

  fclose(file);
  PUTS("Input data were read.");

  return EXIT_SUCCESS;
} /* read_input_data(.) */
/******************************************************************************/
size_t tag_to_node_idx(const tag_t tag) {
  for (size_t node_idx = 0; node_idx < tree_size; ++node_idx) {
    bool equal = true;
    for (size_t j = 0; equal && (j < TAG_LEN); ++j)
      equal &= (tree[node_idx].tag[j] == tag[j]);
    if (equal)
      return node_idx;
  } /* loop over tree nodes */
  return UINT64_MAX;
} /* set_node_indices() */
/******************************************************************************/
void print_tree(void) {
  for (size_t node_idx = 0; node_idx < tree_size; ++node_idx) {
    const node_t *node = &tree[node_idx];
    PRINTF("idx=%03lu tag='%s' left=['%s', %03lu] right=['%s', %03lu]\n",
           node_idx, node->tag, node->left.tag, node->left.idx, node->right.tag,
           node->right.idx);
  } /* loop over tree nodes */
} /* print_tree() */
/******************************************************************************/
void set_node_indices(void) {
  for (size_t node_idx = 0; node_idx < tree_size; ++node_idx) {
    tree[node_idx].left.idx = tag_to_node_idx(tree[node_idx].left.tag);
    tree[node_idx].right.idx = tag_to_node_idx(tree[node_idx].right.tag);
  } /* loop over tree nodes */
} /* set_node_indices() */
/******************************************************************************/
void solve_part1(void) {
  /* solution of the part1 */
  size_t steps_num = 0;
  start_idx = tag_to_node_idx(start_tag);
  end_idx = tag_to_node_idx(end_tag);
  PRINTF("start_idx=%lu end_idx=%lu\n", start_idx, end_idx);
  for (size_t idx = start_idx; idx != end_idx; ++steps_num) {
    const size_t move_idx = steps_num % moves_num;
    const char move = moves[move_idx];
    PRINTF("step_num=%lu: idx %lu->", steps_num, idx);
    idx = (move == 'L') ? tree[idx].left.idx : tree[idx].right.idx;
    PRINTF("%lu\n", idx);
  }
  printf("%lu\n", steps_num);
} /* solve_part1() */
/******************************************************************************/
void solve_part2(void) {
  int ans = 0;
  printf("%d\n", ans);
} /* solve_part2() */
/******************************************************************************/
int main(int argc, char *argv[]) {
  const char *input_file_path = (argc > 1) ? argv[1] : DEFAULT_INPUT_FILE;
  if (EXIT_SUCCESS != read_input_data(input_file_path)) {
    perror("Error reading input data from file");
    return (EXIT_FAILURE);
  }
  set_node_indices();
  print_tree();
  solve_part1();
  solve_part2();

  return EXIT_SUCCESS;
}
