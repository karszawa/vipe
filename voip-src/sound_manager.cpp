#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define REC_MAX 2000

class SoundManager {
  FILE *source, *destination;
  int sb, sc, sr, db, dc, dr;

public:

  SoundManager() : sb(8), sc(2), sr(8000), db(8), dc(2), dr(800) {
    this->source = popen("rec -r 44100 -e s -c 1 -b 8 -t raw -q -", "r");
    this->destination = popen("play -r 44100 -e s -c 1 -b 8 -t raw -q -", "w");
  }

  ~SoundManager() {
    pclose(this->source);
    pclose(this->destination);
  }

  size_t record(char *str) {
    return fread(str, sizeof(char), REC_MAX, this->source);
  }

  size_t play(char *str, size_t play_size) {
    return fwrite(str, sizeof(char), play_size, this->destination);
  }
};
