#include "../include/server.h"

/*
 * Objectif: gérer un client TCP
 * Paramètre:
 * connfd: socket client
 * conn_num:numéro connexion
 * Retour: void
 */

void handle_client(int connfd, int conn_num)
{
    char buffer[BUF_SIZE];
    char response[BUF_SIZE];

    int n = read(connfd, buffer, BUF_SIZE - 1);

    if (n < 0) {
        perror("read");
        close(connfd);
        return;
    }

    if (n == 0) {
        close(connfd);
        return;
    }

    buffer[n] = '\0';

    snprintf(response, sizeof(response),
        "[Connexion #%d] Echo : %.900s",
        conn_num,buffer);

    if (write(connfd, response, strlen(response)) < 0)
    {
        perror("write");
    }

    close(connfd);
}
