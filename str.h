#ifndef STR_H
#define STR_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
  char *value;
  size_t len;
} str;

bool StrWrite(char *s, str *out);

#endif
