#ifndef PSH_READLINE_H_
#define PSH_READLINE_H_

char *psh_readline(FILE *stream);

char *psh_readline(FILE *stream) {
  size_t bufsize = 1024;
  unsigned pos = 0;
  char *buf = malloc(sizeof (char) * bufsize);
  char ch = '\0';

  // allocation error
  if (buf == NULL) {
    perror("psh");
    // psh_err("allocation error");
  }

  while (true) {
    // read a character
    ch = (char) getc(stream);

    // if we hit EOF, replace it with a null character and return.
    if (ch == EOF || ch == '\n') {
      buf[pos] = '\0';

      return buf;
    } else {
      buf[pos] = ch;
    }

    pos++;

    // if we have exceeded the buf, reallocate.
    if (pos >= bufsize) {
      bufsize *= 2;
      buf = realloc(buf, bufsize);

      // allocation error
      if (buf == NULL) {
        perror("psh");
        // psh_err("allocation error");
      }
    }
  }
}

#endif
