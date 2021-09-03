#define _POSIX_C_SOURCE 200809L
#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <libgen.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <wordexp.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "libpsh/tokenize.h"
#include "libpsh/interpret.h"
#include "libpsh/psh.h"
#include "libpsh/concat.h"
#include "libpsh/readline.h"
#include "libpsh/variable.h"

int main(const int argc __attribute__((unused)), const char **argv) {
  if (argv[1] != NULL) {
    if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) {
      printf("%s\n", "pico-shell, version 0.9 alpha - in development");

      return EXIT_SUCCESS;
    } else if (strcmp(argv[1], "-f") == 0 || strcmp(argv[1], "--file") == 0) {
      if (argv[2] == NULL) {
        psh_err("expected a file name");
      } else {
        FILE *fptr = fopen(argv[2], "r");

        if (fptr == NULL) {
          perror(strcon("psh: ", argv[2]));
          
          return EXIT_FAILURE;
        }

        for (unsigned i = 1; !feof(fptr); i++) {
          psh(psh_readline(fptr));
        }

        fclose(fptr);
      }
    }

    return EXIT_SUCCESS;
  }

  FILE *pshrc = fopen(".pshrc", "r");

  if (pshrc == NULL) {
    pshrc = fopen(".pshrc", "w"); // create file
    pshrc = freopen(".pshrc", "r", pshrc);

    if (pshrc == NULL) {
      perror("psh: creating .pshrc");
      // error creating .pshrc
    }
  }

  for (unsigned i = 1; !feof(pshrc); i++) {
    psh(psh_readline(pshrc));
  }

  fclose(pshrc);

  time_t rawtime = time(NULL); // getting local time
  struct tm *timeinfo = localtime(&rawtime);
  char hostname[_SC_HOST_NAME_MAX] = {'\0'};
  gethostname(hostname, _SC_HOST_NAME_MAX);
  uid_t uid = getuid();
  struct passwd *pw = getpwuid(uid);

  if (!pw) {
    return EXIT_FAILURE;
  }

  var_set("hostname", hostname);
  var_set("user", pw->pw_name);
  var_set("term", getenv("TERM"));
  printf("%s\n", asctime(timeinfo));

  do {
    char cwd[FILENAME_MAX] = {'\0'}; // directory
    
    getcwd(cwd, FILENAME_MAX);
    var_set("pwd", cwd);

    const char *cmd = readline(vastrcon(6, pw->pw_name, "@", hostname, " ", basename(cwd), " % "));
    // vastrcon() from libpsh/concat.h (a function that concatenates strings, uses variadic arguments)

    if (cmd == NULL) {
      printf("%s\n", "bye");

      return EXIT_SUCCESS;
    }

    add_history(cmd);
    psh(cmd);
  } while (!feof(stdin));

  return EXIT_SUCCESS;
}
