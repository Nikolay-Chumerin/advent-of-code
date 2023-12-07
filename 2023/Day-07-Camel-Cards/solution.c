#define _GNU_SOURCE
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NO_DEBUG

#define STRING_CAPACITY (8192)
#define DEFAULT_INPUT_FILE "input.txt"
#define SETS_CAPACITY (1024)
#define HAND_LEN (5)
#define CARD_TYPES_NUM (13)
#define EPS (1E-8)

typedef enum {
  WRONG_VALUE = 0,
  HIGH_CARD_VALUE,
  ONE_PAIR_VALUE,
  TWO_PAIRS_VALUE,
  THREE_OF_A_KIND_VALUE,
  FULL_HOUSE_VALUE,
  FOUR_OF_A_KIND_VALUE,
  FIVE_OF_A_KIND_VALUE
} hand_value_t;

static const char *const HAND_TYPE_NAMES[] = {
    [WRONG_VALUE] = "wrong value",
    [HIGH_CARD_VALUE] = "High card",
    [ONE_PAIR_VALUE] = "One pair",
    [TWO_PAIRS_VALUE] = "Two pairs",
    [THREE_OF_A_KIND_VALUE] = "Three of a kind",
    [FULL_HOUSE_VALUE] = "Full house",
    [FOUR_OF_A_KIND_VALUE] = "Four of a kind",
    [FIVE_OF_A_KIND_VALUE] = "Five of a kind"};


typedef char hand_t[HAND_LEN];
typedef struct {
  hand_t hand;
  int bid;
  hand_value_t value;
} hand_and_bid_t;

hand_and_bid_t sets[SETS_CAPACITY];
size_t sets_num = 0U;

const char CARD_TYPES[CARD_TYPES_NUM] = "23456789TJQKA";
const int CARD_TYPE_TO_INDEX[256] = {
    ['2'] = 0,  ['3'] = 1,  ['4'] = 2, ['5'] = 3, ['6'] = 4,
    ['7'] = 5,  ['8'] = 6,  ['9'] = 7, ['T'] = 8, ['J'] = 9,
    ['Q'] = 10, ['K'] = 11, ['A'] = 12};
#define CARD_TYPE_TO_VALUE(card_type_char) (CARD_TYPE_TO_INDEX[card_type_char])

#ifdef DEBUG
#define PUTS puts
#define PRINTF printf
#else
#define PUTS(a) (0)
#define PRINTF(fmt, ...) (0)
#endif // DEBUG

/******************************************************************************/
void trim(char *line) {
  int line_len = strlen(line);
  while (line_len && (line[line_len - 1] <= ' ')) {
    line[--line_len] = 0;
  }
} /* trim(.) */
/******************************************************************************/
hand_value_t hand_value(const hand_t hand) {
  char hist[CARD_TYPES_NUM];
  memset(hist, 0, sizeof(hist));

  for (int i = 0; i < HAND_LEN; ++i) {
    const char card = hand[i];
    const size_t card_type_idx = CARD_TYPE_TO_INDEX[card];
    ++hist[card_type_idx];
  }

  int max_count1 = 0;
  int idx = -1;
  for (int i = 0; i < CARD_TYPES_NUM; ++i) {
    if (max_count1 < hist[i]) {
      max_count1 = hist[i];
      idx = i;
    }
  }
  if (5 == max_count1)
    return FIVE_OF_A_KIND_VALUE;
  if (4 == max_count1)
    return FOUR_OF_A_KIND_VALUE;
  if (1 == max_count1)
    return HIGH_CARD_VALUE;

  hist[idx] = 0;
  int max_count2 = 0;
  for (int i = 0; i < CARD_TYPES_NUM; ++i) {
    if (max_count2 < hist[i])
      max_count2 = hist[i];
  }

  if (3 == max_count1)
    return (2 == max_count2) ? FULL_HOUSE_VALUE : THREE_OF_A_KIND_VALUE;

  if (2 == max_count1)
    return (2 == max_count2) ? TWO_PAIRS_VALUE : ONE_PAIR_VALUE;

  return WRONG_VALUE;
} /* hand_value() */
/******************************************************************************/
int read_input_data(const char *input_file_path) {
  PRINTF("Reading input data from the '%s' file.\n", input_file_path);
  char line[STRING_CAPACITY];
  FILE *file = fopen(input_file_path, "r");
  if (!file) {
    perror("Error opening the file");
    return (EXIT_FAILURE);
  }

  while (fgets(line, sizeof(line), file)) {
    trim(line);
    PRINTF("'%s': ", line);
    /* process line ... */
    char *s = line;
    for (int i = 0; i < HAND_LEN; ++i) {
      sets[sets_num].hand[i] = *s++;
    }
    sets[sets_num].hand[HAND_LEN] = 0;
    sets[sets_num].value = hand_value(sets[sets_num].hand);
    sscanf(s, "%d", &sets[sets_num].bid);

    const char *hand = (char *)&sets[sets_num].hand;
    const int bid = sets[sets_num].bid;
    const hand_value_t value = sets[sets_num].value;
    const char *type = HAND_TYPE_NAMES[value];
    PRINTF("hand='%s' bid=%-5d value=%d type='%s'\n", hand, bid, value, type);

    ++sets_num;
  } /* loop over lines */

  fclose(file);
  PUTS("Input data were read.");
  return EXIT_SUCCESS;
} /* read_input_data(.) */
/******************************************************************************/
int compare_two_sets(const void *a, const void *b) {
  const hand_and_bid_t *hba = (hand_and_bid_t *)a;
  const hand_and_bid_t *hbb = (hand_and_bid_t *)b;
  if (hba->value < hbb->value)
    return -1;
  if (hba->value > hbb->value)
    return 1;
  for (int i = 0; i < HAND_LEN; ++i) {
    if (CARD_TYPE_TO_VALUE(hba->hand[i]) < CARD_TYPE_TO_VALUE(hbb->hand[i]))
      return -1;
    if (CARD_TYPE_TO_VALUE(hba->hand[i]) > CARD_TYPE_TO_VALUE(hbb->hand[i]))
      return 1;
  }
  return 0;
} /* compare_two_sets(..) */
/******************************************************************************/
void solve_part1(void) {
  /* solution of the part1 */
  qsort(sets, sets_num, sizeof(hand_and_bid_t), compare_two_sets);
  int ans = 0;
  for (int i = 0; i < sets_num; ++i) {
    const int coeff = i + 1;
    PRINTF("i=%d coeff=%d bid=%d value=%d hand='%s'\n", i, coeff, sets[i].bid,
           sets[i].value, sets[i].hand);
    ans += coeff * sets[i].bid;
  }
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
  if (EXIT_SUCCESS != read_input_data(input_file_path)) {
    perror("Error reading input data from file");
    return (EXIT_FAILURE);
  }

  solve_part1();
  solve_part2();

  return EXIT_SUCCESS;
}
