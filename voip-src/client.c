#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <pthread.h>

// #define DEBUG

typedef void (*FUNCTION)(void*);

const int RECEIVE_DATA_SIZE = 1024;

int establish_connect(char* ip_address, int port) {
  int s = socket(PF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr;

  addr.sin_family = AF_INET;
  inet_aton(ip_address, &addr.sin_addr);
  addr.sin_port = htons(port);

  if(connect(s, (struct sockaddr *)&addr, sizeof(addr)) == 0) {
    fprintf(stderr, "SUCCESSFULL CONNECTION: %s\n", ip_address);
  } else {
    fprintf(stderr, "FAILED CONNECTION(ERRORNO=%d): %s\n", errno, ip_address);
    exit(1);
  }

  return s;
}

// Thread 1: send sounds
// Thread 2: receive sounds

void send_sounds(void *p) {
  int soc = *(int*)p;

  while(1) {
    char tmp[RECEIVE_DATA_SIZE] = { 0 };

    int n = read(0, tmp, RECEIVE_DATA_SIZE);

#ifdef DEBUG
    fprintf(stderr, "\x1b[31m%s\x1b[37m", tmp);
#endif

    if(n > 0) {
      write(soc, tmp, n);
    } else if(n == 0) {
      close(soc);
      break;
    } else {
      fprintf(stderr, "ERROR OCCURS WHILE READING FROM STANDARD INPUT\n");
      exit(1);
    }
  }

  fprintf(stderr, "FINISH SENDING SOUNDS\n");
}

void receive_sounds(void *p) {
  int soc = *(int*)p;

  while(1) {
    char tmp[RECEIVE_DATA_SIZE] = { 0 };

    int n = read(soc, tmp, RECEIVE_DATA_SIZE);

#ifdef DEBUG
    fprintf(stderr, "\x1b[33m%s\x1b[37m", tmp);
#endif

    if(n > 0) {
      write(1, tmp, strlen(tmp));
    } else if(n == 0) {
      break;
    } else {
      fprintf(stderr, "ERROR OCCURS WHILE READING FROM SOCKET\n");
      exit(1);
    }
  }

  fprintf(stderr, "FINISH RECEIVING SOUNDS\n");
}

// ./client ip_address port
int main(int argc, char **argv) {
  int soc = establish_connect(argv[1], atoi(argv[2]));

  FUNCTION functions[2] = { send_sounds, receive_sounds };
  pthread_t threads[2];

  fprintf(stderr, "===CREATING THREADS===\n");

  for(int i = 0; i < 2; i++) {
    int ret = pthread_create(threads + i, NULL, (void*)functions[i], &soc);

    if(ret != 0) {
      err(EXIT_FAILURE, "Can not create thread %d: %s", i, strerror(ret));
    }
  }

  fprintf(stderr, "===EXECUTE JOINING THREADS===\n");

  for(int i = 0; i < 2; i++) {
    int ret = pthread_join(threads[i], NULL);

    if(ret != 0) {
      err(EXIT_FAILURE, "Can not create thread %d: %s", i, strerror(ret));
    }
  }

  return 0;
}
