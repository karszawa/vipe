#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <vector>
#include "stack.cpp"

#define RECEIVE_DATA_SIZE 2200
#define STACK_BUFFER_SIZE RECEIVE_DATA_SIZE * 32
#define MAX_NODE_SIZE 8

class VoiceCommuniactionNetwork {
public:

  int soc;
  size_t client_size;
  std::vector<Stack> stacks;
  std::vector<struct sockaddr_in> clients;

  VoiceCommuniactionNetwork(int port) : client_size(0) {
    this->soc = get_socket(port);
    this->stacks = std::vector<Stack>(MAX_NODE_SIZE);
    this->clients = std::vector<struct sockaddr_in>(MAX_NODE_SIZE);
  }

  // Return whether it was received from a new client
  int receiveFromClient() {
    static char buffer[RECEIVE_DATA_SIZE];

    memset(buffer, 0, sizeof(buffer));

    /// 1. RECEIVING DATA

    int received_data_size = 0;
    struct sockaddr_in client_address;
    unsigned int client_address_length = sizeof(client_address);

    if((received_data_size = recvfrom(this->soc, buffer, RECEIVE_DATA_SIZE, 0, (struct sockaddr*)&client_address, &client_address_length)) < 0) {
      fprintf(stderr, "RECEIVE ERROR");
      exit(0);
    }

    /// 2. REGIST CLIENT

    int is_new_client = 1;
    char client_address_str[32];
    char *tmp = inet_ntoa(client_address.sin_addr);

    strcpy(client_address_str, tmp);

    for(int i = 0; i < this->client_size; i++) {
      if(strcmp(inet_ntoa(this->clients[i].sin_addr), client_address_str) == 0) {
        is_new_client = 0;
      }
    }

    if(is_new_client) {
      fprintf(stderr, "NEW CONNECTION: %s\n", client_address_str);
      sendto(this->soc, "CONNECTED!", 10, 0, (struct sockaddr *)&client_address, sizeof(client_address));
      this->clients[this->client_size++] = client_address;
    }

    /// 3. PUSH DATA TO A STACK

    for(int i = 0; i < this->client_size; i++) {
      if(strcmp(inet_ntoa(this->clients[i].sin_addr), client_address_str) == 0) {
        this->stacks[i].push_data(buffer, received_data_size);
      }
    }

    return is_new_client;
  }

  void dispatchToClient() {
    static char buffer[RECEIVE_DATA_SIZE * 32];

    int min_stack_size = 1 << 30;

    for(int i = 0; i < this->client_size; i++) {
      min_stack_size = std::min(min_stack_size, this->stacks[i].stack_size);
    }

    for(int i = 0; i < this->client_size; i++) {
      /// 1. SYNTHESIS RECEIVED SOUNDS

      memset(buffer, 0, sizeof(buffer));

      for(int j = 0; j < this->client_size; j++) {
        if(i == j) {
          continue;
        }

        for(int k = 0; k < min_stack_size; k++) {
          buffer[k] += ((int)this->stacks[j].pile[k]) / 2;
        }
      }

      /// 2. SEND SYHTHESISED SOUNDS

      if(min_stack_size > 0) {
        sendto(this->soc, buffer, min_stack_size, 0, (struct sockaddr *)&this->clients[i], sizeof(this->clients[i]));
      }
    }

    /// 3. RESET STACKS
    for(int i = 0; i < this->client_size; i++) {
      this->stacks[i].shift(min_stack_size);
    }
  }

private:

  int get_socket(int port) {
    const int multicast_ttl = 5, yes1 = 1, yes2 = 1;
    int soc = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    setsockopt(soc, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes1, sizeof(yes1));
    setsockopt(soc, SOL_SOCKET, SO_REUSEPORT, (const char *)&yes2, sizeof(yes2));
    setsockopt(soc, IPPROTO_IP, IP_MULTICAST_TTL, (void*)&multicast_ttl, sizeof(multicast_ttl));

    if(bind(soc, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
      perror("bind");
      exit(1);
    }

    return soc;
  }
};
