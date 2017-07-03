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
#include <time.h>
#include <math.h>
#include <future>
#include <chrono>
#include "voice_communication_network.cpp"
#include "common.cpp"
#include "tcp_network.cpp"

void message_handler(const char* address, const char* message, int message_size) {
  if(start_with(message, "CHANGE NAME: ")) {

  } else if(start_with(message, "CHAT: ")) {

  }
}

void send_connection_list(const Network network, const VoiceCommuniactionNetwork vcn) {
  char message[MAX_MESSAGE_SIZE];

  memset(message, 0, sizeof(message));

  strcat(message, "CONNECTIONS: ");

  for(int i = 0; i < vcn.client_size; i++) {
    strcat(message, inet_ntoa(vcn.clients[i].sin_addr));

    if(i + 1 != vcn.client_size) {
      strcat(message, ",");
    }
  }

  fprintf(stderr, "%s\n", message);
  broadcast_message(&network, message, strlen(message));
}

// argv[1]: tcp port
int main(int argc, char **argv) {
  // fprintf(stderr, "ESTA: \n");
  // return 0;
  const auto DISPATCH_DURATION = std::chrono::milliseconds(50);
  VoiceCommuniactionNetwork vcn(atoi(argv[2]));

  Network network;

  auto wait_connection_thread = std::thread([&]{ wait_connection(&network, atoi(argv[1]), message_handler); });

  auto last_time = std::chrono::system_clock::now();

  puts("START: Waiting for the first connection.");

  while(1) {
    int is_new_client = vcn.receiveFromClient();

    if(is_new_client) {
      send_connection_list(network, vcn);
    }

    if(std::chrono::system_clock::now() - last_time > DISPATCH_DURATION) {
      last_time = std::chrono::system_clock::now();

      vcn.dispatchToClient();
    }
  }

  wait_connection_thread.join();

  return 0;
}

// TODO: fish: './server_udp' terminated by signal SIGBUS (Misaligned address error)
