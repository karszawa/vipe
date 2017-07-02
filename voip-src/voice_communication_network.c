#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include "stack.c"

#define RECEIVE_DATA_SIZE 2200
#define MAX_NODE_SIZE 8

struct VoiceCommuniactionNetwork {
  int socket;
  size_t client_size;
  struct Stack *stacks;
  struct sockaddr_in *clients;
};

int get_socket() {
  const int multicast_ttl = 5;
  int soc = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  struct sockaddr_in addr;

  addr.sin_family = AF_INET;
  addr.sin_port = htons(54321);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if(bind(soc, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
  }

  if(setsockopt(soc, IPPROTO_IP, IP_MULTICAST_TTL, (void*)&multicast_ttl, sizeof(multicast_ttl)) < 0) {
    perror("setsockopt() failed");
  }

  return soc;
}

void initVCN(struct VoiceCommuniactionNetwork *vcn) {
  vcn->socket = get_socket();
  vcn->client_size = 0;
  vcn->stacks = (struct Stack*)malloc(sizeof(struct Stack) * MAX_NODE_SIZE);
  vcn->clients = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in) * MAX_NODE_SIZE);
}

// Return whether it was received from a new client
int receiveFromClient(struct VoiceCommuniactionNetwork *vcn) {
  static char buffer[RECEIVE_DATA_SIZE];

  memset(buffer, 0, sizeof(buffer));

  /// 1. RECEIVING DATA

  int received_data_size = 0;
  struct sockaddr_in client_address;
  unsigned int client_address_length = sizeof(client_address);

  if((received_data_size = recvfrom(vcn->socket, buffer, RECEIVE_DATA_SIZE, 0, (struct sockaddr*)&client_address, &client_address_length)) < 0) {
    fprintf(stderr, "RECEIVE ERROR");
    exit(0);
  }

  /// 2. REGIST CLIENT

  int is_new_client = 1;
  char client_address_str[32];
  char *tmp = inet_ntoa(client_address.sin_addr);

  strcpy(client_address_str, tmp);

  for(int i = 0; i < vcn->client_size; i++) {
    if(strcmp(inet_ntoa(vcn->clients[i].sin_addr), client_address_str) == 0) {
      is_new_client = 0;
    }
  }

  if(is_new_client) {
    fprintf(stderr, "NEW CONNECTION: %s\n", client_address_str);
    sendto(vcn->socket, "CONNECTED!", 10, 0, (struct sockaddr *)&client_address, sizeof(client_address));
    vcn->clients[vcn->client_size++] = client_address;
  }

  /// 3. PUSH DATA TO A STACK

  for(int i = 0; i < vcn->client_size; i++) {
    if(strcmp(inet_ntoa(vcn->clients[i].sin_addr), client_address_str) == 0) {
      pushDataToStack(&vcn->stacks[i], buffer, received_data_size);
    }
  }

  return is_new_client;
}

void dispatchToClient(struct VoiceCommuniactionNetwork *vcn) {
  static char buffer[RECEIVE_DATA_SIZE * 8];

  for(int i = 0; i < vcn->client_size; i++) {
    /// 1. SYNTHESIS RECEIVED SOUNDS

    memset(buffer, 0, sizeof(buffer));
    int buffer_size = 0;

    for(int j = 0; j < vcn->client_size; j++) {
      if(i == j) {
        continue;
      }

      buffer_size += vcn->stacks[j].stack_size;

      for(int k = 0; k < vcn->stacks[j].stack_size; k++) {
        buffer[k] += vcn->stacks[j].pile[k];
      }
    }

    /// 2. SEND SYHTHESISED SOUNDS

    if(buffer_size > 0) {
      sendto(vcn->socket, buffer, buffer_size, 0, (struct sockaddr *)&vcn->clients[i], sizeof(vcn->clients[i]));
    }
  }

  /// 3. RESET STACKS

  for(int i = 0; i < vcn->client_size; i++) {
    resetStack(&vcn->stacks[i]);
  }
}
