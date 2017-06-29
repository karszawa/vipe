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

const int RECEIVE_DATA_SIZE = 4096;

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

// struct sockaddr_in get_multicast_addr() {
//   struct sockaddr_in multicast_addr;
//
//   memset(&multicast_addr, 0, sizeof(multicast_addr));
//   multicast_addr.sin_family = AF_INET;
//   multicast_addr.sin_port = htons(54321);
//   multicast_addr.sin_addr.s_addr = inet_addr("244.1.1.1");
//
//   return multicast_addr;
// }

int main(int argc, char **argv) {
  int socket = get_socket();
  struct sockaddr_in clients[8];
  int client_size = 0;
  // struct sockaddr_in multicast_addr = get_multicast_addr();

  puts("START");

  while(1) {
    int receive_message_size = 0;
    char buffer[RECEIVE_DATA_SIZE] = { 0 };
    struct sockaddr_in client_address;
    unsigned int client_address_length = sizeof(client_address);

    fprintf(stderr, "RECEIVING ...\n");

    if((receive_message_size = recvfrom(socket, buffer, RECEIVE_DATA_SIZE, 0, (struct sockaddr*)&client_address, &client_address_length)) < 0) {
      fprintf(stderr, "RECEIVE ERROR");
      exit(0);
    }

    int is_new_client = 1;
    char client_address_str[32];
    char *tmp = inet_ntoa(client_address.sin_addr);

    strcpy(client_address_str, tmp);

    fprintf(stderr, "RECEIVED DATA FROM: %s\n", client_address_str);

    for(int i = 0; i < client_size; i++) {
      if(strcmp(inet_ntoa(clients[i].sin_addr), client_address_str) == 0) {
        is_new_client = 0;
      } else {
        fprintf(stderr, "SENDING ...\n");

        if(sendto(socket, buffer, receive_message_size, 0, (struct sockaddr *)&clients[i], sizeof(clients[i])) != receive_message_size) {
        // if(sendto(socket, buffer, receive_message_size, 0, (struct sockaddr *)&multicast_addr, sizeof(multicast_addr)) != receive_message_size) {
          fprintf(stderr, "SEND ERROR\n");
          exit(0);
        }
      }
    }

    if(is_new_client) {
      printf("NEW CLIENT: %s\n", client_address_str);
      sendto(socket, "CONNECTED!", 10, 0, (struct sockaddr *)&client_address, sizeof(client_address));
      clients[client_size++] = client_address;
    }
  }

  return 0;
}
