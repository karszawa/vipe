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
#include "sound_manager.cpp"
#include "tcp_network.cpp"

#define SEND_DATA_SIZE 2200
#define RECEIVE_DATA_SIZE 40000

void message_handler(const char* address, const char* message, int message_size) {
  // CHANGE NAME new name
  // CHAT content
  printf("%s", message);
}

void send_sounds(int socket, struct sockaddr_in target_address, SoundManager sound_manager) {
  char buffer[SEND_DATA_SIZE];

  memset(buffer, 0, sizeof(buffer));

  while(1) {
    size_t read_length = sound_manager.record(buffer);

    if(read_length == 0) {
			break;
		} else if (read_length == -1) {
			perror("send error");
			break;
		}

    if(sendto(socket, buffer, read_length, 0, (struct sockaddr *)&target_address, sizeof(target_address)) < 0) {
      fprintf(stderr, "SENDTO ERROR\n");
      break;
    }
  }

  shutdown(socket, SHUT_WR);
}

void receive_sounds(int socket, SoundManager sound_manager) {
  char buffer[RECEIVE_DATA_SIZE];

  memset(buffer, 0, sizeof(buffer));

  struct sockaddr_in sender_address;
  socklen_t sender_address_size;

  while(1) {
    int receive_message_size = recvfrom(socket, buffer, RECEIVE_DATA_SIZE, 0, (struct sockaddr *)&sender_address, &sender_address_size);

    if(receive_message_size == -1) {
      perror("write error");
      break;
    }

    sound_manager.play(buffer, receive_message_size);
  }

  shutdown(socket, SHUT_WR);
}

// argv[1]: ip address (like 192.168.100.4)
int main(int argc, char** argv) {
  SoundManager sound_manager;

  int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

  if(sock == -1) {
		perror("error:socket");
	}

	struct sockaddr_in server_address;

  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(54321);
	server_address.sin_addr.s_addr = inet_addr(argv[1]);

  auto send_thread = std::thread([=, &sound_manager]{ send_sounds(sock, server_address, sound_manager); });
  auto recv_thread = std::thread([=, &sound_manager]{ receive_sounds(sock, sound_manager); });

  auto message_thread = connect_to_server(argv[1], message_handler);

  send_thread.join();
  recv_thread.join();
  message_thread.join();

	return 0;
}
