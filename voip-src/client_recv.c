#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int establish_connect(char* ip_address, int port) {
  int s = socket(PF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr;

  addr.sin_family = AF_INET;
  inet_aton(ip_address, &addr.sin_addr);
  addr.sin_port = htons(port);

  if(connect(s, (struct sockaddr *)&addr, sizeof(addr)) == 0) {
    printf("SUCCESSFULL CONNECTION: %s\n", ip_address);
  } else {
    printf("FAILED CONNECTION(ERRORNO=%d): %s\n", errno, ip_address);
    exit(1);
  }

  return s;
}

int main(int argc, char** argv) {
  int soc = establish_connect(argv[1], atoi(argv[2]));

  while(1) {
    const size_t N = 256;
    char data[N];

    int n = read(soc, data, N);

    if(n > 0) {
      write(1, data, n);
    } else if(n == 0) {
      shutdown(soc, SHUT_WR);
      break;
    } else {
      printf("READ ERROR\n");
      exit(1);
    }
  }

  close(soc);

  return 0;
}
