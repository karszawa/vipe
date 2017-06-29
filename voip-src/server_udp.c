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

const int RECEIVE_DATA_SIZE = 128;

int get_socket() {
  int soc = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  struct sockaddr_in addr;

  addr.sin_family = AF_INET;
  addr.sin_port = htons(54321);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if(bind(soc, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("bind");
  }

  return soc;
}

int main(int argc, char **argv) {
  int socket = get_socket();
  struct sockaddr_in clients[8];
  int client_size = 0;

  while(1) {
    int receive_message_size = 0;
    char buffer[RECEIVE_DATA_SIZE] = { 0 };
    struct sockaddr_in client_address;
    unsigned int client_address_length = sizeof(client_address);

    fprintf(stderr, "RECEIVING ...\n");
    if((receive_message_size = recvfrom(socket, buffer, RECEIVE_DATA_SIZE, 0, (struct sockaddr*)&client_address, &client_address_length)) < 0) {
      fprintf(stderr, "RECVFROM ERROR");
      exit(0);
    }

    fprintf(stderr, "RECEIVED DATA FROM: %s\n", inet_ntoa(client_address.sin_addr));

    char *client_address_str = inet_ntoa(client_address.sin_addr);
    int is_new_client = 1;

    for(int i = 0; i < client_size; i++) {
      if(strcmp(inet_ntoa(clients[i].sin_addr), client_address_str) == 0) {
        is_new_client = 0;
      } else {
        fprintf(stderr, "SENDING ...\n");

        if(sendto(socket, buffer, receive_message_size, 0, (struct sockaddr *)&clients[i], sizeof(clients[i])) != receive_message_size) {
          fprintf(stderr, "SENDTO ERROR\n");
          exit(0);
        }
      }
    }

    if(is_new_client) {
      fprintf(stderr, "NEW CLIENT: %s\n", inet_ntoa(client_address.sin_addr));
      sendto(socket, "CONNECTED!", 10, 0, (struct sockaddr *)&client_address, sizeof(client_address));
      clients[client_size++] = client_address;
    }
  }

  return 0;
}
