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
// #include <future>

#define MAX_CONNECTION_SIZE 8
#define MAX_MESSAGE_SIZE 1024

typedef void (*WAIT_MESSAGE_FUNC)(const char*, const char*, int);

void establish_thread(pthread_t* thread, void *(*start_routine)(void*), void* args) {
  int ret = pthread_create(thread, NULL, start_routine, args);

  if(ret != 0) {
    err(EXIT_FAILURE, "%s", strerror(ret));
  }
}

struct Network {
  int sockets[MAX_CONNECTION_SIZE];
  char client_addresses[MAX_CONNECTION_SIZE][32];
  int sockets_size;
};

void initNetwork(struct Network *network) {
  network->sockets_size = 0;
}

struct WaitMessageServerParams {
  struct Network *network;
  int target_index;
  WAIT_MESSAGE_FUNC handler;
};

void _wait_message_server(void *p) {
  struct WaitMessageServerParams *params = (struct WaitMessageServerParams*)p;
  struct Network *network = params->network;
  int target_index = params->target_index;
  WAIT_MESSAGE_FUNC handler = params->handler;

  char buffer[MAX_MESSAGE_SIZE];

  while(1) {
    memset(buffer, 0, sizeof(buffer));

    int message_size = read(network->sockets[target_index], buffer, MAX_MESSAGE_SIZE);

    if(message_size < 0) {
      fprintf(stderr, "FAILED TO READ");
      break;
    }

    handler(network->client_addresses[target_index], buffer, message_size);
  }
}

void wait_connection(struct Network *network, WAIT_MESSAGE_FUNC handler) {
  pthread_t threads[MAX_CONNECTION_SIZE];

  initNetwork(network);

  struct sockaddr_in address;

  address.sin_family = AF_INET;
  address.sin_port = htons(51515);
  address.sin_addr.s_addr = INADDR_ANY;

  int soc = socket(PF_INET, SOCK_STREAM, 0);

  if(bind(soc, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind");
  }

  listen(soc, MAX_CONNECTION_SIZE);

  struct WaitMessageServerParams params[MAX_CONNECTION_SIZE];

  for(int i = 0; i < MAX_CONNECTION_SIZE; i++) {
    struct sockaddr_in client;
    socklen_t len = sizeof(struct sockaddr_in);

    network->sockets[i] = accept(soc, (struct sockaddr *) &client, &len);

    if(network->sockets[i] < -1) {
      fprintf(stderr, "CONNECTION ERROR");
      i--;
      continue;
    }

    strcpy(network->client_addresses[network->sockets_size], inet_ntoa(client.sin_addr));
    network->sockets_size = network->sockets_size + 1;

    fprintf(stderr, "NEW TCP CONNECTION: %d\n", network->sockets[i]);

    params[i].network = network;
    params[i].target_index = i;
    params[i].handler = handler;

    establish_thread(threads + i, (void*)_wait_message_server, params + i);
  }
}

struct WaitConnectionWrapperParams {
  struct Network *network;
  WAIT_MESSAGE_FUNC handler;
};

void wait_connection_wrapper(void *p) {
  struct WaitConnectionWrapperParams *params = (struct WaitConnectionWrapperParams*)p;
  struct Network *network = params->network;
  WAIT_MESSAGE_FUNC handler = params->handler;

  wait_connection(network, handler);
}

void broadcast_message(const struct Network *network, const char *message, const int message_size) {
  for(int i = 0; i < network->sockets_size; i++) {
    write(network->sockets[i], message, message_size);
  }
}

struct WaitMessageClientParams {
  int socket;
  char server_address[32];
  WAIT_MESSAGE_FUNC handler;
};

void _wait_message_client(void *p) {
  struct WaitMessageClientParams *params = (struct WaitMessageClientParams*)p;
  WAIT_MESSAGE_FUNC handler = params->handler;
  int soc = params->socket;

  char buffer[MAX_MESSAGE_SIZE];

  while(1) {
    memset(buffer, 0, sizeof(buffer));

    int message_size = read(soc, buffer, MAX_MESSAGE_SIZE);
    printf("_wait_message_client: %s\n", buffer);

    if(message_size < 0) {
      fprintf(stderr, "FAILED TO READ");
      break;
    }

    handler(params->server_address, buffer, message_size);
  }
}

void connect_to_server(int *soc, const char *server_address, WAIT_MESSAGE_FUNC handler) {
  *soc = socket(PF_INET, SOCK_STREAM, 0);
  struct sockaddr_in address;

  address.sin_family = AF_INET;
  inet_aton(server_address, &address.sin_addr);
  address.sin_port = htons(51515);

  if(connect(*soc, (struct sockaddr *)&address, sizeof(address)) == 0) {
    fprintf(stderr, "NEW TCP CONNECTION: %s(%d)\n", server_address, *soc);
  } else {
    fprintf(stderr, "FAIL TO CONNECT: %s\n", server_address);
    exit(1);
  }

  pthread_t *thread = (pthread_t*)malloc(sizeof(pthread_t));
  struct WaitMessageClientParams *params = (struct WaitMessageClientParams*)malloc(sizeof(struct WaitMessageClientParams));

  params->socket = *soc;
  params->handler = handler;
  strcpy(params->server_address, server_address);

  pthread_create(thread, NULL, (void*)_wait_message_client, params);
}

void send_message(int soc, const char *message, const int message_size) {
  write(soc, message, message_size);
}
