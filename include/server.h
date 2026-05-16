
#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 9999
#define BACKLOG 10
#define BUF_SIZE 1024

void handle_client(int connfd, int conn_num);

void daemonize();

void log_msg(const char *msg);

#endif
