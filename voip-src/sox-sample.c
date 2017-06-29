#include <assert.h>
#include <sox.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char ** argv) {
  (void)argc;
  /* 5秒分 (48000 sample/sec x 2チャンネル x 5秒 */
  size_t n = 48000 * 2 * 5;

  sox_sample_t * buf = malloc(sizeof(sox_sample_t) * n);
  sox_init();

  fprintf(stderr, "使い方: %s | play -t raw -b 16 -c 1 -e s -r 48000 -\n", argv[0]);

  fprintf(stderr, "リターンキーを打つと同時に何かしゃべってください．5秒間録音します>>>");
  fflush(stderr);
  fgetc(stdin);

  sox_format_t * ft = sox_open_read("default", 0, 0, "pulseaudio");
  size_t m = sox_read(ft, buf, n);
  assert(m == n);
  sox_close(ft);
  sox_quit();

  fprintf(stderr, "録音終わりました. リターンキーを押したら, 符号付き16 bit, モノラル, 48000Hzのraw形式にして標準出力に出力します >>>");
  fflush(stderr);
  fgetc(stdin);

  {
    /* 32 bit, 2 channel -> 16 bit, 1 channel */
    size_t i;
    short * sbuf = malloc(sizeof(short) * n / 2);
    for (i = 0; i < n/2; i++) {
      sbuf[i] = (buf[2 * i] >> 16);
    }
    fwrite(sbuf, sizeof(short), n / 2, stdout);
  }

  return 0;
}
