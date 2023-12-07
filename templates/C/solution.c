#include <stdio.h>
#include <stdlib.h>

#define MAX_LINE_LENGTH (1024)
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
    
  } /* loop over lines */
  fclose(file);
  printf("%d\n", ans);
  return EXIT_SUCCESS;
}
