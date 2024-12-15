#include "server.h"
#include <arpa/inet.h>
#include <stdio.h>

int main() {
  struct Server server =
      server_Constructor(AF_INET, 8080, SOCK_STREAM, 0, 10, INADDR_ANY, launch);

  printf("Listening on: %s:%d\n", inet_ntoa(server.address.sin_addr),
         ntohs(server.address.sin_port));
  printf("--------\n");
  server.launch(&server);
  return 0;
}
