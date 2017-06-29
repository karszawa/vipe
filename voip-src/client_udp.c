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

struct SendSoundParams {
  int socket;
  struct sockaddr_in *address;
  FILE *source;
};

struct ReceiveSoundParams {
  int socket;
  FILE *destination;
};

void send_sounds(void *p) {
  struct SendSoundParams *params = p;
  int socket = params->socket;
  struct sockaddr_in *address = params->address;
  FILE *source = params->source;

  char buffer[RECEIVE_DATA_SIZE];

  memset(buffer, 0, sizeof(buffer));

  while(1) {
    // size_t read_length = read(0, buffer, RECEIVE_DATA_SIZE);
    size_t read_length = fread(buffer, sizeof(char), RECEIVE_DATA_SIZE, source);

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
  struct ReceiveSoundParams *params = p;
  int socket = params->socket;
  FILE *destination = params->destination;

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

    // write(1, buffer, receive_message_size);
    fwrite(buffer, sizeof(char), receive_message_size, destination);
  }

  shutdown(socket, SHUT_WR);
}

// argv[1]:IPaddr, argv[2]:port num
int main(int argc, char** argv) {
	int sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

  if(sock == -1) {
		perror("error:socket");
		return 1;
	}

  FILE *source = popen("rec -r 44100 -e s -c 1 -b 16 -t raw -", "r");
  FILE *destination = popen("play -r 44100 -e s -c 1 -b 16 -t raw -", "w");

	struct sockaddr_in server_address;

  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(atoi(argv[2]));
	server_address.sin_addr.s_addr = inet_addr(argv[1]);

  struct SendSoundParams send_sound_params;

  send_sound_params.socket = sock;
  send_sound_params.address = &server_address;
  send_sound_params.source = source;

  struct ReceiveSoundParams receive_sound_params;

  receive_sound_params.socket = sock;
  receive_sound_params.destination = destination;

  pthread_t send_thread, receive_thread;

  pthread_create(&send_thread, NULL, (void*)send_sounds, &send_sound_params);
  pthread_create(&receive_thread, NULL, (void*)receive_sounds, &receive_sound_params);

  pthread_join(send_thread, NULL);
  pthread_join(receive_thread, NULL);

	pclose(source);
	pclose(destination);

	return 0;
}
