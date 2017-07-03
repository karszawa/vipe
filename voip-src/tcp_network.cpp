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
#include <future>
#include <vector>

#define MAX_CONNECTION_SIZE 8
#define MAX_MESSAGE_SIZE 1024

typedef void (*WAIT_MESSAGE_FUNC)(const char*, const char*, int);

class Network {
public:

  int sockets[MAX_CONNECTION_SIZE];
  char client_addresses[MAX_CONNECTION_SIZE][32];
  int sockets_size;

  Network() : sockets_size(0) {

  }
};

void _wait_message_server(Network *network, int target_index, WAIT_MESSAGE_FUNC handler) {
  char buffer[MAX_MESSAGE_SIZE];

  while(1) {
    memset(buffer, 0, sizeof(buffer));

    int message_size = read(network->sockets[target_index], buffer, MAX_MESSAGE_SIZE);

    if(message_size < 0) {
      fprintf(stderr, "ERROR: FAILED TO READ");
      break;
    }

    handler(network->client_addresses[target_index], buffer, message_size);
  }
}

void wait_connection(Network *network, int port, WAIT_MESSAGE_FUNC handler) {
  std::vector<std::thread> threads;

  struct sockaddr_in address;

  address.sin_family = AF_INET;
  address.sin_port = htons(port);
  address.sin_addr.s_addr = INADDR_ANY;

  int soc = socket(PF_INET, SOCK_STREAM, 0);

  int yes = 1;
  setsockopt(soc, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes));
  setsockopt(soc, SOL_SOCKET, SO_REUSEPORT, (const char *)&yes, sizeof(yes));

  if(bind(soc, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind");
  }

  listen(soc, MAX_CONNECTION_SIZE);

  fprintf(stderr, "ESTABLISHED: \n");

  for(int i = 0; i < MAX_CONNECTION_SIZE; i++) {
    struct sockaddr_in client;
    socklen_t len = sizeof(struct sockaddr_in);

    network->sockets[i] = accept(soc, (struct sockaddr *) &client, &len);

    if(network->sockets[i] < -1) {
      fprintf(stderr, "ERROR: CONNECTION ERROR\n");
      i--;
      continue;
    }

    strcpy(network->client_addresses[network->sockets_size], inet_ntoa(client.sin_addr));
    network->sockets_size = network->sockets_size + 1;

    fprintf(stderr, "NEW TCP CONNECTION: %d\n", network->sockets[i]);

    threads.push_back(std::thread([=]{ _wait_message_server(network, i, handler); }));
  }

  for(auto &thread : threads) {
    thread.join();
  }
}

void broadcast_message(const Network *network, const char *message, const int message_size) {
  for(int i = 0; i < network->sockets_size; i++) {
    write(network->sockets[i], message, message_size);
  }
}

void _wait_message_client(int socket, const char server_address[32], WAIT_MESSAGE_FUNC handler) {
  char buffer[MAX_MESSAGE_SIZE];

  while(1) {
    memset(buffer, 0, sizeof(buffer));

    int message_size = read(socket, buffer, MAX_MESSAGE_SIZE);

    if(message_size < 0) {
      fprintf(stderr, "ERROR: FAILED TO READ\n");
      break;
    }

    handler(server_address, buffer, message_size);
  }
}

std::thread connect_to_server(const char *server_address, int port, WAIT_MESSAGE_FUNC handler) {
  int soc = socket(PF_INET, SOCK_STREAM, 0);
  struct sockaddr_in address;

  address.sin_family = AF_INET;
  inet_aton(server_address, &address.sin_addr);
  address.sin_port = htons(port);

  if(connect(soc, (struct sockaddr *)&address, sizeof(address)) == 0) {
    fprintf(stderr, "CONNECTED: %s(%d)\n", server_address, soc);
  } else {
    fprintf(stderr, "ERROR: FAIL TO CONNECT: %s\n", server_address);
    exit(1);
  }

  return std::thread([=]{ _wait_message_client(soc, server_address, handler); });
}

void send_message(int soc, const char *message, const int message_size) {
  write(soc, message, message_size);
}
