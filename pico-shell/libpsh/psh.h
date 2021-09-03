#ifndef PSH_H_
#define PSH_H_
#include <stdlib.h>
#include <stdio.h>
#include "tokenize.h"
#include "interpret.h"

// I don't know why I have to do this
int psh_interpret(char **argv);

int psh(const char *cmd);

int psh(const char *cmd) {
  char *cmddup = strdup(cmd);

  for (unsigned i = 0; cmddup[i] != '\0'; i++) {
    if (cmddup[i] == '#') {
      if (i == 0) {
        cmddup[i] = '\0';
      } else if (i != 0 && cmddup[i - 1] != '\\') {
        cmddup[i] = '\0';
      }
    }
  }

  // empty command
  if (psh_tokenize(cmd)[0] == NULL) {
    return EXIT_SUCCESS;
  }

  char **argv = psh_tokenize(cmddup);
  int status = psh_interpret(argv);

  for (int i = 0; argv[i] != NULL; i++) {
    argv[i] = NULL;
  }

  free(cmddup);

  return status;
}

#endif
