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
#include "sound_manager.cpp"
#include "my_time.cpp"
#include "tcp_network.cpp"

#define SEND_DATA_SIZE 2200
#define RECEIVE_DATA_SIZE 40000

void message_handler(const char* address, const char* message, int message_size) {
  // CHANGE NAME new name
  // CHAT content
  printf("%s", message);
}

struct SendSoundParams {
  int socket;
  struct sockaddr_in *address;
  struct SoundManager sound_manager;
};

struct ReceiveSoundParams {
  int socket;
  struct SoundManager sound_manager;
};

void send_sounds(void *p) {
  struct SendSoundParams *params = p;
  int socket = params->socket;
  struct sockaddr_in *address = params->address;
  struct SoundManager sound_manager = params->sound_manager;

  char buffer[SEND_DATA_SIZE];

  memset(buffer, 0, sizeof(buffer));

  while(1) {
    size_t read_length = recSound(sound_manager, buffer);

    if(read_length == 0) {
			break;
		} else if (read_length == -1) {
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
  struct ReceiveSoundParams *params = p;
  int socket = params->socket;
  struct SoundManager sound_manager = params->sound_manager;

  char buffer[RECEIVE_DATA_SIZE];

  memset(buffer, 0, sizeof(buffer));

  struct sockaddr_in address;
  socklen_t address_size;

  while(1) {
    int receive_message_size = recvfrom(socket, buffer, RECEIVE_DATA_SIZE, 0, (struct sockaddr *)&address, &address_size);

    if(receive_message_size == -1) {
      perror("write error");
      break;
    }

    // write(1, buffer, receive_message_size);
    playSound(sound_manager, buffer, receive_message_size);
  }

  shutdown(socket, SHUT_WR);
}

// argv[1]:IPaddr
int main(int argc, char** argv) {
	int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

  if(sock == -1) {
		perror("error:socket");
		return 1;
	}

  struct SoundManager sound_manager;

  openSoundManager(&sound_manager);

	struct sockaddr_in server_address;

  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(54321);
	server_address.sin_addr.s_addr = inet_addr(argv[1]);

  struct SendSoundParams send_sound_params;

  send_sound_params.socket = sock;
  send_sound_params.address = &server_address;
  send_sound_params.sound_manager = sound_manager;

  struct ReceiveSoundParams receive_sound_params;

  receive_sound_params.socket = sock;
  receive_sound_params.sound_manager = sound_manager;

  pthread_t send_thread, receive_thread;

  pthread_create(&send_thread, NULL, (void*)send_sounds, &send_sound_params);
  pthread_create(&receive_thread, NULL, (void*)receive_sounds, &receive_sound_params);

  int tcp_socket = 0;

  connect_to_server(&tcp_socket, argv[1], message_handler);

  pthread_join(send_thread, NULL);
  pthread_join(receive_thread, NULL);

	closeSoundManager(&sound_manager);

	return 0;
}
