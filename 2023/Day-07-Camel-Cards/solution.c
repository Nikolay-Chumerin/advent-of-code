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
  hand_value_t value1;
  hand_value_t value2;
} hand_and_bid_t;

hand_and_bid_t sets[SETS_CAPACITY];
size_t sets_num = 0U;

const char CARD_TYPES[CARD_TYPES_NUM] = "23456789TJQKA";
const int CARD_TYPE_TO_INDEX[256] = {
    ['2'] = 0,  ['3'] = 1,  ['4'] = 2, ['5'] = 3, ['6'] = 4,
    ['7'] = 5,  ['8'] = 6,  ['9'] = 7, ['T'] = 8, ['J'] = 9,
    ['Q'] = 10, ['K'] = 11, ['A'] = 12};
#define CARD_TYPE_TO_VALUE(card_type_char) (CARD_TYPE_TO_INDEX[card_type_char])

const int CARD_TYPE_TO_VALUE2[256] = {
    ['2'] = 1,  ['3'] = 2,  ['4'] = 3, ['5'] = 4, ['6'] = 5,
    ['7'] = 6,  ['8'] = 7,  ['9'] = 8, ['T'] = 9, ['J'] = 0,
    ['Q'] = 10, ['K'] = 11, ['A'] = 12};

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
hand_value_t hand_value1(const hand_t hand) {
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
} /* hand_value1(.) */
/******************************************************************************/
hand_value_t hand_value2(const hand_t hand) {
  char hist[CARD_TYPES_NUM];
  memset(hist, 0, sizeof(hist));

  for (int i = 0; i < HAND_LEN; ++i) {
    const char card = hand[i];
    const size_t card_type_idx = CARD_TYPE_TO_INDEX[card];
    ++hist[card_type_idx];
  }

  int max_count1 = 0;
  int idx1 = -1;
  for (int i = 0; i < CARD_TYPES_NUM; ++i) {
    if (max_count1 < hist[i]) {
      max_count1 = hist[i];
      idx1 = i;
    }
  }
  const size_t J_IDX = CARD_TYPE_TO_INDEX['J'];
  int j_count1 = hist[J_IDX];

  hist[idx1] = 0;

  int max_count2 = 0;
  int idx2 = -1;
  for (int i = 0; i < CARD_TYPES_NUM; ++i) {
    if (max_count2 < hist[i]) {
      max_count2 = hist[i];
      idx2 = i;
    }
  }

  if (J_IDX == idx1) {
    max_count1 += max_count2;
    max_count2 = 1;
  } else {
    max_count1 += j_count1;
  }
  switch (max_count1) {
  case 5:
    return FIVE_OF_A_KIND_VALUE;
  case 4:
    return FOUR_OF_A_KIND_VALUE;
  case 3:
    return (2 == max_count2) ? FULL_HOUSE_VALUE : THREE_OF_A_KIND_VALUE;
  case 2:
    return (2 == max_count2) ? TWO_PAIRS_VALUE : ONE_PAIR_VALUE;
  case 1:
    return HIGH_CARD_VALUE;
  default:
    return WRONG_VALUE;
  }
} /* hand_value2(.) */
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
    sets[sets_num].value1 = hand_value1(sets[sets_num].hand);
    sets[sets_num].value2 = hand_value2(sets[sets_num].hand);
    sscanf(s, "%d", &sets[sets_num].bid);

    const char *hand = (char *)&sets[sets_num].hand;
    const int bid = sets[sets_num].bid;
    const hand_value_t value1 = sets[sets_num].value1;
    const char *type = HAND_TYPE_NAMES[value1];
    PRINTF("hand='%s' bid=%-5d value1=%d type='%s'\n", hand, bid, value1, type);

    ++sets_num;
  } /* loop over lines */

  fclose(file);
  PUTS("Input data were read.");
  return EXIT_SUCCESS;
} /* read_input_data(.) */
/******************************************************************************/
int compare_two_sets1(const void *a, const void *b) {
  const hand_and_bid_t *hba = (hand_and_bid_t *)a;
  const hand_and_bid_t *hbb = (hand_and_bid_t *)b;
  if (hba->value1 < hbb->value1)
    return -1;
  if (hba->value1 > hbb->value1)
    return 1;
  for (int i = 0; i < HAND_LEN; ++i) {
    if (CARD_TYPE_TO_VALUE(hba->hand[i]) < CARD_TYPE_TO_VALUE(hbb->hand[i]))
      return -1;
    if (CARD_TYPE_TO_VALUE(hba->hand[i]) > CARD_TYPE_TO_VALUE(hbb->hand[i]))
      return 1;
  }
  return 0;
} /* compare_two_sets1(..) */
/******************************************************************************/
void solve_part1(void) {
  /* solution of the part1 */
  qsort(sets, sets_num, sizeof(hand_and_bid_t), compare_two_sets1);
  int ans = 0;
  for (int i = 0; i < sets_num; ++i) {
    const int rank = i + 1;
    PRINTF("i=%d rank=%d bid=%d value=%d hand='%s'\n", i, rank, sets[i].bid,
           sets[i].value1, sets[i].hand);
    ans += rank * sets[i].bid;
  }
  printf("%d\n", ans);
} /* solve_part1() */
/******************************************************************************/
int compare_two_sets2(const void *a, const void *b) {
  const hand_and_bid_t *hba = (hand_and_bid_t *)a;
  const hand_and_bid_t *hbb = (hand_and_bid_t *)b;
  if (hba->value2 < hbb->value2)
    return -1;
  if (hba->value2 > hbb->value2)
    return 1;
  for (int i = 0; i < HAND_LEN; ++i) {
    if (CARD_TYPE_TO_VALUE2[hba->hand[i]] < CARD_TYPE_TO_VALUE2[hbb->hand[i]])
      return -1;
    if (CARD_TYPE_TO_VALUE2[hba->hand[i]] > CARD_TYPE_TO_VALUE2[hbb->hand[i]])
      return 1;
  }
  return 0;
} /* compare_two_sets2(..) */
/******************************************************************************/
void solve_part2(void) {
  /* solution of the part2 */
  qsort(sets, sets_num, sizeof(hand_and_bid_t), compare_two_sets2);
  int ans = 0;
  for (int i = 0; i < sets_num; ++i) {
    const int rank = i + 1;
    const char *type = HAND_TYPE_NAMES[sets[i].value2];

    PRINTF("i=%d rank=%d bid=%d value2=%d hand='%s' type='%s'\n", i, rank,
           sets[i].bid, sets[i].value2, sets[i].hand, type);
    ans += rank * sets[i].bid;
  }
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
