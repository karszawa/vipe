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
#include <time.h>
#include <math.h>
#include "my_time.c"
#include "voice_communication_network.c"
#include "common.c"

int main(int argc, char **argv) {
  struct VoiceCommuniactionNetwork vcn;

  initVCN(&vcn);

  // static struct Stack stacks[MAX_NODE_SIZE];
  //
  // int socket = get_socket();
  // struct sockaddr_in clients[MAX_NODE_SIZE];
  // int client_size = 0;

  puts("START: Waiting for the first connection.");

  struct timespec begin, end;
  clock_gettime(CLOCK_MONOTONIC_RAW, &begin);

  // char buffer[RECEIVE_DATA_SIZE] = { 0 };

  while(1) {
    // memset(buffer, 0, sizeof(buffer));

    // int receive_message_size = 0;
    // struct sockaddr_in client_address;
    // unsigned int client_address_length = sizeof(client_address);
    //
    // if((receive_message_size = recvfrom(socket, buffer, RECEIVE_DATA_SIZE, 0, (struct sockaddr*)&client_address, &client_address_length)) < 0) {
    //   fprintf(stderr, "RECEIVE ERROR");
    //   exit(0);
    // }
    //
    // int is_new_client = 1;
    // char client_address_str[32];
    // char *tmp = inet_ntoa(client_address.sin_addr);
    //
    // strcpy(client_address_str, tmp);
    //
    // for(int i = 0; i < client_size; i++) {
    //   if(strcmp(inet_ntoa(clients[i].sin_addr), client_address_str) == 0) {
    //     is_new_client = 0;
    //   }
    // }
    //
    // // regist new client
    // if(is_new_client) {
    //   printf("NEW CONNECTION: %s\n", client_address_str);
    //   sendto(socket, "CONNECTED!", 10, 0, (struct sockaddr *)&client_address, sizeof(client_address));
    //   clients[client_size++] = client_address;
    // }
    //
    // // piling data
    // for(int i = 0; i < client_size; i++) {
    //   if(strcmp(inet_ntoa(clients[i].sin_addr), client_address_str) == 0) {
    //     pushDataToStack(&stacks[i], buffer, receive_message_size);
    //   }
    // }

    int is_new_client = receiveFromClient(&vcn);

    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    // dispatch messages
    if(-get_difference_of_time(end, begin) > 50) {
      clock_gettime(CLOCK_MONOTONIC_RAW, &begin);

      // dispatch(stacks, socket, clients, client_size);
      dispatchToClient(&vcn);
    }
  }

  return 0;
}
