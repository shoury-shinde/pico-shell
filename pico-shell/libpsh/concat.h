#ifndef PSH_CONCAT_H_
#define PSH_CONCAT_H_
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

// concatenates two string, returns concatenated string
char *strcon(const char *s1, const char *s2);
// uses variadic arguments to concatenate multiple string, returns concatenated string
char *vastrcon(const unsigned count, ...);

__attribute__((warn_unused_result)) char *strcon(const char *s1, const char *s2) {
  // check if they're null
  if (s1 == NULL) {
    return strdup(s2);
  } else if (s2 == NULL) {
    return strdup(s1);
  }

  char *s = malloc(strlen(s1) + strlen(s2) - 1);
  // minus 1 because we don't want '\0' twice, just once
  strcpy(s, s1);
  strcat(s, s2);

  return s;
}

// uses variadic arguments to concatenate multiple string, returns concatenated string
__attribute__((warn_unused_result)) char *vastrcon(const unsigned count, ...) {
  char *s = NULL;
  va_list ap;

  va_start(ap, count);

  for (unsigned i = 0; i < count; i++) {
    s = strcon(s, va_arg(ap, char *));
  }

  va_end(ap);

  return s;
}

#endif
