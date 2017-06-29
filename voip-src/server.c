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

// Thread 1: waiting connection request from clients and establish connection
// Thread 2: receive sound from clients
// Thread 3: broadcast sound

typedef void (*FUNCTION)(void*);

const int MAX_CONNECTION_SIZE = 8;
const int RECEIVE_DATA_SIZE = 1024;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct Network {
  int sockets[MAX_CONNECTION_SIZE];
  int sockets_size;
  char message_queue[MAX_CONNECTION_SIZE][RECEIVE_DATA_SIZE];
};

struct CenteredNetwork {
  struct Network *network;
  int target_index;
};

void lock() {
  int ret = pthread_mutex_lock(&mutex);

  if(ret != 0) {
    errc(EXIT_FAILURE, ret, "can not lock");
  }
}

void unlock() {
  int ret = pthread_mutex_unlock(&mutex);

  if(ret != 0) {
    errc(EXIT_FAILURE, ret, "can not unlock");
  }
}

void receive_sound_from_clients(void *p) {
  struct CenteredNetwork *c = (struct CenteredNetwork*)p;
  struct Network *network = c->network;
  int target_index = c->target_index;

  while(1) {
    char tmp[RECEIVE_DATA_SIZE] = { 0 };

    int n = read(network->sockets[target_index], tmp, RECEIVE_DATA_SIZE);
    fprintf(stderr, "\x1b[33m%s\x1b[37m", tmp);

    if(n <= 0) {
      fprintf(stderr, "FAILED TO READ");
      break;
    }

    const int sockets_size = network->sockets_size;

    for(int i = 0; i < sockets_size; i++) {
      if(i == target_index) {
        continue;
      }

      if(network->message_queue[i][0] != '\0') {
        continue;
      }

      strcpy(network->message_queue[i], tmp);
    }
  }
}

void broadcast(void *p) {
  struct CenteredNetwork *c = (struct CenteredNetwork*)p;
  struct Network *network = c->network;
  int target_index = c->target_index;

  while(1) {
    if(network->message_queue[target_index][0] != '\0') {
      fprintf(stderr, "\x1b[31m%s\x1b[37m", network->message_queue[target_index]);
      write(network->sockets[target_index], network->message_queue[target_index], strlen(network->message_queue[target_index]));
      network->message_queue[target_index][0] = '\0';
    }
  }
}

void establish_thread(pthread_t* thread, void *(*start_routine)(void*), void* args) {
  int ret = pthread_create(thread, NULL, start_routine, args);

  if(ret != 0) {
    err(EXIT_FAILURE, "%s", strerror(ret));
  }
}

void wait_connection() {
  struct Network network;
  pthread_t threads[MAX_CONNECTION_SIZE * 2];
  struct CenteredNetwork centered_networks[MAX_CONNECTION_SIZE];

  int soc = socket(PF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr;

  addr.sin_family = AF_INET;
  addr.sin_port = htons(54321);
  addr.sin_addr.s_addr = INADDR_ANY;

  if(bind(soc, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
  }

  listen(soc, MAX_CONNECTION_SIZE);

  for(int i = 0; i < MAX_CONNECTION_SIZE; i++) {
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(struct sockaddr_in);

    network.sockets[i] = accept(soc, (struct sockaddr *) &client_addr, &len);

    if(network.sockets[i] < -1) {
      fprintf(stderr, "CONNECTION ERROR");
      i--;
      continue;
    }

    network.sockets_size = network.sockets_size + 1;
    network.message_queue[i][0] = '\0';

    fprintf(stderr, "New socket(%d): %d\n", network.sockets_size, network.sockets[i]);

    centered_networks[i].network = &network;
    centered_networks[i].target_index = i;

    establish_thread(threads + i * 2 + 1, (void*)broadcast, &centered_networks[i]);
    establish_thread(threads + i * 2, (void*)receive_sound_from_clients, &centered_networks[i]);
  }

  fprintf(stderr, "MAX CONNECTION\n");
}

int main(int argc, char **argv) {
  wait_connection();

  return 0;
}
