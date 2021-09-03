#ifndef PSH_VARIABLE_H_
#define PSH_VARIABLE_H_
#define BUFSIZE 5

static unsigned var_cnt = 0;
static char **var_key = NULL;
static char **var_val = NULL;

bool var_set(char *key, char *value);
char *var_get(char *key);
int var_get_pos(char *key);

bool var_set(char *key, char *value) {
  if (var_get_pos(key) != -1) {
    var_val[var_get_pos(key)] = value;

    return false; // variable does exist
  }

  var_key = realloc(var_key, sizeof (char *) * BUFSIZE * var_cnt);
  var_val = realloc(var_val, sizeof (char *) * BUFSIZE * var_cnt);

  var_key[var_cnt] = key;
  var_val[var_cnt++] = value;

  return true; // variable doesn't exist
}

__attribute__((warn_unused_result)) int var_get_pos(char *key) {
  for (unsigned i = 0; i < var_cnt; i++) {
    if (strcmp(var_key[i], key) == 0) {
      return (int) i;
    }
  }

  return -1;
}

__attribute__((warn_unused_result)) char *var_get(char *key) {
  for (unsigned i = 0; i < var_cnt; i++) {
    if (strcmp(var_key[i], key) == 0) {
      return var_val[i];
    }
  }

  return NULL;
}

#endif
