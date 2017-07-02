#include <string.h>

#ifndef max
    #define max(a,b) ((a) > (b) ? (a) : (b))
#endif

#ifndef min
    #define min(a,b) ((a) < (b) ? (a) : (b))
#endif

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
