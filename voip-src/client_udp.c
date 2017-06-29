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

struct SocketAddress {
  int socket;
  struct sockaddr_in *address;
};

void send_sounds(void *p) {
  struct SocketAddress *socket_address = p;
  int socket = socket_address->socket;
  struct sockaddr_in *address = socket_address->address;

  char buffer[RECEIVE_DATA_SIZE];

  memset(buffer, 0, sizeof(buffer));

  while(1) {
    size_t read_length = read(0, buffer, RECEIVE_DATA_SIZE);

    if(read_length == 0) {
			break;
		} else if (read_length == -1){
			perror("send error");
			break;
		}

    if(sendto(socket, buffer, read_length, 0, (struct sockaddr *)address, sizeof(*address)) < 0) {
      fprintf(stderr, "SENDTO ERROR\n");
      break;
    }
  }

  shutdown(socket, SHUT_WR);
}

void receive_sounds(void *p) {
  int socket = *(int*)p;

  char buffer[RECEIVE_DATA_SIZE];

  memset(buffer, 0, sizeof(buffer));

  struct sockaddr_in address;
  socklen_t address_size;

  while(1) {
    int receive_message_size = recvfrom(socket, buffer, RECEIVE_DATA_SIZE, 0, (struct sockaddr *)&address, &address_size);

    if(receive_message_size == 0) {
      break;
    } else if(receive_message_size == -1) {
      perror("write error");
      break;
    }

    write(1, buffer, receive_message_size);
  }

  shutdown(socket, SHUT_WR);
}

// argv[1]:IPaddr, argv[2]:port num
int main(int argc, char** argv) {
	int send_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	int receive_socket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

  if(send_socket == -1 || receive_socket == -1){
		perror("error:socket");
		return 1;
	}

	struct sockaddr_in server_address;

  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(atoi(argv[2]));
	server_address.sin_addr.s_addr = inet_addr(argv[1]);

  struct SocketAddress socket_address;

  socket_address.socket = send_socket;
  socket_address.address = &server_address;

	// FILE* wp = popen("play -r 44100 -e s -c 1 -b 16 -t raw -", "w");
	// FILE* fp = popen("rec -r 44100 -e s -c 1 -b 16 -t raw -", "r");

  pthread_t send_thread, receive_thread;

  pthread_create(&send_thread, NULL, (void*)send_sounds, &socket_address);
  pthread_create(&receive_thread, NULL, (void*)receive_sounds, &receive_socket);

  pthread_join(send_thread, NULL);
  pthread_join(receive_thread, NULL);

	// pclose(wp);
	// pclose(fp);

	return 0;
}
