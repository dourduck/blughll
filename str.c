#include <stdbool.h>
#include <string.h>

#include "str.h"

bool StrWrite(char *s, str *out) {
  if (s == NULL) {
    return false;
  }

  out->len = strlen(s);
  out->value = s;
  return true;
}
