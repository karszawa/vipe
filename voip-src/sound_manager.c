#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define REC_MAX 2200

struct SoundManager {
  FILE *source, *destination;
  int sb, sc, sr, db, dc, dr;
};

void openSoundManager(struct SoundManager *sound_manager) {
  sound_manager->source = popen("rec -r 44100 -e s -c 2 -b 8 -t raw -q -", "r");
  sound_manager->destination = popen("play -r 44100 -e s -c 2 -b 8 -t raw -q -", "w");
  sound_manager->sb = 8;
  sound_manager->sc = 1;
  sound_manager->sr = 44100;
}

void closeSoundManager(struct SoundManager *sound_manager) {
  pclose(sound_manager->source);
  pclose(sound_manager->destination);
}

size_t recSound(const struct SoundManager sound_manager, char *str) {
  return fread(str, sizeof(char), REC_MAX, sound_manager.source);
}

size_t playSound(const struct SoundManager sound_manager, char *str, int play_size) {
  return fwrite(str, sizeof(char), play_size, sound_manager.destination);
}
