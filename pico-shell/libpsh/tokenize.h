/* use wordexp() */

#ifndef PSH_TOKENIZE_H_
#define PSH_TOKENIZE_H_
#include <string.h>
#include <stdbool.h>
#include <limits.h>

char **psh_tokenize(const char *str);

__attribute__((warn_unused_result)) char **psh_tokenize(const char *str) {
  char *sdup = strdup(str);

  for (unsigned i = 0; sdup[i] != '\0'; i++) {
    if (sdup[i] == '#') {
      if (i == 0) {
        sdup[i] = '\0';
      } else if (i != 0 && sdup[i - 1] != '\\') {
        sdup[i] = '\0';
      }
    }
  }

  

  static char **arr = NULL;
  size_t bufsize = 1024;
  arr = realloc(arr, sizeof (char *) * bufsize);
  char *saveptr = NULL;
  char *tok = strtok_r(sdup, " ", &saveptr);
  arr[0] = tok;
  unsigned pos = 1;

  while (true) {
    tok = strtok_r(NULL, " ", &saveptr);

    if (tok == NULL) {
      break;
    }

    arr[pos++] = strdup(tok);

    if (pos >= bufsize) {
      bufsize *= 2;

      arr = realloc(arr, sizeof (char *) * bufsize);
    }
  }

  return arr;
}

#endif
