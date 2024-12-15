#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct Server server_Constructor(int domain, int port, int service,
                                 int protocol, int backlog, u_long interface,
                                 void (*launch)(struct Server *server)) {
  struct Server server;

  server.domain = domain;
  server.service = service;
  server.port = port;
  server.protocol = protocol;
  server.backlog = backlog;

  server.address.sin_family = domain;
  server.address.sin_port = htons(port);
  server.address.sin_addr.s_addr = htonl(interface);

  server.socket = socket(domain, service, protocol);

  if (server.socket < 0) {
    perror("Failed to initialize/connect to socket...\n");
    exit(EXIT_FAILURE);
  }

  // i dont care if the socket is not fully closed yet just let me use the
  // fucking thing
  int opt = 1;
  if (setsockopt(server.socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) <
      0) {
    perror("Failed to set SO_REUSEADDR...\n");
    exit(EXIT_FAILURE);
  }

  if (bind(server.socket, (struct sockaddr *)&server.address,
           sizeof(server.address)) < 0) {
    perror("Failed to bind socket...\n");
    exit(EXIT_FAILURE);
  }

  if (listen(server.socket, server.backlog) < 0) {
    perror("Failed to start listening...\n");
    exit(EXIT_FAILURE);
  }

  server.launch = launch;
  return server;
}

void printMethod(char *str) {
  if (str != NULL && *str != '\0') {
    putchar(*str);
    putchar('\n');
  } else {
    printf("String is empty");
  }
}

void launch(struct Server *server) {
  char buffer[BUFFER_SIZE];
  while (1) {

    char method[8];
    char path[256];

    int addrlen = sizeof(server->address);
    int new_socket = accept(server->socket, (struct sockaddr *)&server->address,
                            (socklen_t *)&addrlen);
    if (new_socket < 0) {
      perror("Failed to accept connection...\n");
      continue;
    }

    ssize_t bytesRead = read(new_socket, buffer, BUFFER_SIZE - 1);

    if (bytesRead < 0) {
      perror("Error reading buffer...\n");
      close(new_socket);
      continue;
    }

    buffer[bytesRead] = '\0';

    if (sscanf(buffer, "%7s %255s", method, path) == 2) {
      printf("Method: %s\n", method);
      printf("Path: %s\n\n", path);
    } else {
      printf("Invalid connection.");
      close(new_socket);
      continue;
    }

    if (strcmp(path, "/") == 0) {
      char *response = "HTTP/1.1 200 OK\r\n"
                       "Content-Type: text/html; charset=UTF-8\r\n\r\n"
                       "<!DOCTYPE html>\r\n"
                       "<html>\r\n"
                       "<head>\r\n"
                       "<title>Testing Basic HTTP-SERVER</title>\r\n"
                       "</head>\r\n"
                       "<body>\r\n"
                       "Iam testing\r\n"
                       "</body>\r\n"
                       "</html>\r\n";

      write(new_socket, response, strlen(response));
    } else {
      char *response = "HTTP/1.1 404 Not Found\r\n"
                       "Content-Type: text/html; charset=UTF-8\r\n\r\n"
                       "<!DOCTYPE html>\r\n"
                       "<html>\r\n"
                       "<head>\r\n"
                       "<title>Testing Basic HTTP-SERVER</title>\r\n"
                       "</head>\r\n"
                       "<body>\r\n"
                       "Not found\r\n"
                       "</body>\r\n"
                       "</html>\r\n";
      write(new_socket, response, strlen(response));
    }

    close(new_socket);
  }
}
