#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH (16384)
#define DEFAULT_INPUT_FILE "input.txt"

int main(int argc, char *argv[]) {
  FILE *file = fopen((argc > 1) ? argv[1] : DEFAULT_INPUT_FILE, "r");
  if (!file) {
    perror("Error opening the file");
    return EXIT_FAILURE;
  }
  char line[MAX_LINE_LENGTH];
  int ans = 0;
  while (fgets(line, sizeof(line), file)) {
    char *p = line;
    int first = -1;
    int last = -1;
    while (*p) {
      if (*p >= '0' && *p <= '9') {
        last = *p - '0';
        if (first < 0) {
          first = last;
        }
      }
      ++p;
    } /* loop over line chars */
    if (first >= 0 && last >= 0) {
      ans += first * 10 + last;
    }
  } /* loop over lines */
  fclose(file);
  printf("%d\n", ans);
  return EXIT_SUCCESS;
}
