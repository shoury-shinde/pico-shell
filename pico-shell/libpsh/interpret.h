#ifndef PSH_INTERPRETER_H_
#define PSH_INTERPRETER_H_
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <ctype.h>
#include "psh.h"
#include "concat.h"

#include "variable.h"
#include "readline.h"

int psh_interpret(char **argv);
int psh_exec(char **argv);
void pidhandler(int sig __attribute__((unused)));
int psh_err(const char *err);
int psh_perror(const char *str);

static pid_t pid = 0;
static int exit_status = 0;

// executes built-in commands and calls psh_exec() if a command is not built-in
int psh_interpret(char **argv) {
  signal(SIGINT, pidhandler);
  var_set("null", "(null)");

  if (argv[0][0] == '\\') {
    for (unsigned i = 0; argv[0][i] != '\0'; i++) {
      argv[0][i] = argv[0][i + 1];
    }

    return psh_exec(argv);
  }

  for (int i = 0; argv[i] != NULL; i++) {
    if (argv[i][0] == '$') {
      for (unsigned j = 0; argv[i][j] != '\0'; j++) {
        argv[i][j] = argv[i][j + 1];
      }

      argv[i] = var_get(argv[i])
        ? var_get(argv[i])
        : NULL;
    }
  }

  if (strcmp(argv[0], "status") == 0) {
    printf("%d\n", exit_status);

    return EXIT_SUCCESS;
  }

  if (strcmp(argv[0], "exit") == 0) {
    argv[1] != NULL
      ? exit(atoi(argv[1]))
      : exit(EXIT_SUCCESS);
  }

  if (strcmp(argv[0], "bye") == 0) {
    exit(EXIT_SUCCESS);
  }

  if (strcmp(argv[0], "exec") == 0) {
    for (unsigned i = 0; argv[i] != NULL; i++) {
      argv[i] = argv[i + 1];
    }

    return psh_exec(argv);
  }

  // built-in change-directory command
  if (strcmp(argv[0], "cd") == 0) {
    if (argv[1] == NULL) {
      return psh_err("expected directory");
    } else if (chdir(argv[1]) != 0 && strcmp(argv[1], "~") != 0) {
      return psh_perror(vastrcon(2, "psh: ", argv[1]));
    }

    if (strcmp(argv[1], "~") == 0) {
      uid_t uid = getuid();
      struct passwd *pw = getpwuid(uid);
      chdir(pw->pw_dir);

      return EXIT_SUCCESS;
    }

    chdir(getcwd(argv[1], 0));

    return EXIT_SUCCESS;
  }

  // variables
  // works like this:
  // var set variable_name = variable_value
  // var get variable_name
  if (strcmp(argv[0], "var") == 0) {
    if (argv[1] == NULL) {
      return psh_err("expected variable name");
    }

    if (argv[2] == NULL || strcmp(argv[2], "=") != 0) {
      return psh_err("character '=' missing after variable name");
      // '=' makes it more readable
    }

    for (unsigned i = 0; argv[1][i] != '\0'; i++) {
      switch (tolower(argv[1][i])) {
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '_':
          break;
        default:
          return psh_err(vastrcon(3, "variable name cannot contain character '", &argv[1][i], "'"));
      }
    }

    var_set(argv[1], argv[3]);

    return EXIT_SUCCESS;
  }

  // if-statement
  // works like this:
  // if [condition] : [cmd] [args ...]
  // non-zero = true; 0 = false
  if (strcmp(argv[0], "if") == 0) {
    if (argv[1] == NULL) {
      return psh_err("expected condition");
    }
    
    if (argv[2] == NULL || strcmp(argv[2], ":") != 0) {
      return psh_err("character ':' missing after condition");
      // ':' makes it more readable
    } else if (argv[3] != NULL) {
      // removes the first 3 tokens ('if', condition, ':')
      const char *condition = argv[1];

      for (int i = 0; i < 3; i++) {
        for (unsigned j = 0; argv[j] != NULL; j++) {
          argv[j] = argv[j + 1];
        }
      }

      // recursion
      if (atoi(condition)) {
        psh_interpret(argv);
      }
    }

    return EXIT_SUCCESS;
  }

  // for-loop
  // works like this:
  // for [cnt] : [cmd] [args ...]
  if (strcmp(argv[0], "for") == 0) {
    if (argv[1] == NULL) {
      return psh_err("expected numeric value");
    }
    
    if (argv[2] == NULL || strcmp(argv[2], ":") != 0) {
      return psh_err("character ':' missing after numeric value");
      // ':' makes it more readable
    } else if (argv[3] != NULL) {
      unsigned cnt = (unsigned) atoi(argv[1]);

      // removes the first 3 tokens ('for', count, ':')
      for (int i = 0; i < 3; i++) {
        for (unsigned j = 0; argv[j] != NULL; j++) {
          argv[j] = argv[j + 1];
        }
      }

      // recursion
      for (unsigned i = 0; i < cnt; i++) {
        psh_interpret(argv);
      }
    }

    return EXIT_SUCCESS;
  }

  if (strcmp(argv[0], "while") == 0) {
    if (argv[1] == NULL) {
      return psh_err("expected numeric value");
    }
    
    if (argv[2] == NULL || strcmp(argv[2], ":") != 0) {
      return psh_err("character ':' missing after numeric value");
    } else if (argv[3] != NULL) {
      const char *condition = argv[1];
      // removes the first 3 tokens ('while', condition, ':')
      for (int i = 0; i < 3; i++) {
        for (unsigned j = 0; argv[j] != NULL; j++) {
          argv[j] = argv[j + 1];
        }
      }

      // recursion
      while (atoi(condition)) {
        psh_interpret(argv);
      }
    }

    return EXIT_SUCCESS;
  }

  if (strcmp(argv[0], "read") == 0) {
    if (argv[1] == NULL) {
      argv[1] = "reply";
    }

    var_set(argv[1], psh_readline(stdin));

    return EXIT_SUCCESS;
  }

  return psh_exec(argv);
}

// executes arguments and returns the exit status
int psh_exec(char **argv) {
  int status = 0;
  pid = fork();
  pid_t wpid = 0;

  if (pid == 0) {
    if (execvp(argv[0], argv) == -1) {
      return psh_perror(strcon("psh: ", argv[0]));
    }

  } else if (pid < 0) {
    return psh_perror("psh");
  } else {
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return status;
}

// for child processes
void pidhandler(int sig __attribute__((unused))) {
  putchar('\n');
  kill(pid, SIGKILL);

  return;
}

int psh_err(const char *err) {
  fflush(stdout);
  fprintf(stderr, "psh: %s\n", err);
  fflush(stderr);
  
  exit_status = 1;

  return EXIT_FAILURE;
}

int psh_perror(const char *str) {
  perror(str);

  return (exit_status = errno);
}

#endif
