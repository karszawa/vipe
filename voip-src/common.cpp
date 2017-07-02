#include <string.h>

int start_with(const char *s, const char *prefix) {
  if(strlen(s) < strlen(prefix)) {
    return 0;
  }

  for(int i = 0; i < strlen(prefix); i++) {
    if(s[i] != prefix[i]) {
      return 0;
    }
  }

  return 1;
}
