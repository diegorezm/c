#include "server.h"
#include "bundler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct item {
  char *name;
  int price;
};

char *items_to_char(struct item *items, int size) {
  int total_length = 1;
  for (int i = 0; i < size; i++) {
    total_length += strlen(items[i].name) + 15;
  }
  char *response = malloc(total_length);
  if (response == NULL) {
    perror("Failed to allocate memory for items string");
    return NULL;
  }

  response[0] = '\0';

  for (int i = 0; i < size; i++) {
    char temp[256];
    sprintf(temp, "<li>%s: $%d</li>", items[i].name, items[i].price);
    strcat(response, temp);
  }
  return response;
}

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

      char *html = fileToChar("./templates/index.html");
      char *headers = "HTTP/1.1 200 OK\r\n"
                      "Content-Type: text/html; charset=UTF-8\r\n\r\n";

      char *response;

      response = malloc(strlen(html) + strlen(headers) + 1);

      strcpy(response, headers);
      strcat(response, html);

      struct item items[] = {{"Laptop", 1000},
                             {"Smartphone", 700},
                             {"Headphones", 150},
                             {"Smartwatch", 200},
                             {"Keyboard", 50}};
      int size = sizeof(items) / sizeof(items[0]);

      char *html_items = items_to_char(items, size);

      response = replace_with(response, "{content}", html_items);

      write(new_socket, response, strlen(response));

      free(response);
      if (html_items) {
        free(html_items);
      }
    } else if (strcmp(path, "/styles.css") == 0) {
      char *css = fileToChar("./templates/styles.css");
      char *headers = "HTTP/1.1 200 OK\r\n"
                      "Content-Type: text/css; charset=UTF-8\r\n\r\n";
      char *response;
      response = malloc(strlen(css) + strlen(headers) + 1);
      strcpy(response, headers);
      strcat(response, css);
      write(new_socket, response, strlen(response));
      free(response);
    } else if (strcmp(path, "/about") == 0) {
      char *html = fileToChar("./templates/about.html");
      char *headers = "HTTP/1.1 200 OK\r\n"
                      "Content-Type: text/html; charset=UTF-8\r\n\r\n";
      char *response;
      response = malloc(strlen(html) + strlen(headers) + 1);

      strcpy(response, headers);
      strcat(response, html);

      response = replace_with(response, "{content}",
                              "This message came from the server!");

      write(new_socket, response, strlen(response));
      free(response);
    } else {
      char *html = fileToChar("./templates/404.html");
      char *headers = "HTTP/1.1 200 OK\r\n"
                      "Content-Type: text/html; charset=UTF-8\r\n\r\n";
      char *response;
      response = malloc(strlen(html) + strlen(headers) + 1);
      strcpy(response, headers);
      strcat(response, html);
      write(new_socket, response, strlen(response));
      free(response);
    }

    close(new_socket);
  }
}
