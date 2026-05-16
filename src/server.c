#include "../include/server.h"

/*
 * Objectif: serveur TCP itératif
 * Paramètre: aucun
 * Retour: int
 */

int main(void)
{
    int listenfd;
    int connfd;

    struct sockaddr_in srv;

    int conn_num = 0;
    int opt = 1;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    if (listenfd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    setsockopt(
        listenfd,
        SOL_SOCKET,
        SO_REUSEADDR,
        &opt,
        sizeof(opt)
    );

    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = INADDR_ANY;
    srv.sin_port = htons(PORT);

    if (bind(
            listenfd,
            (struct sockaddr*)&srv, sizeof(srv)
        ) < 0)
    {
        perror("bind"),
        exit(EXIT_FAILURE);
    }

    if (listen(listenfd, BACKLOG) < 0) {
        perror("listen"),
        exit(EXIT_FAILURE);
    }


    printf("Serveur démarré sur port %d\n", PORT);

    while (1)
    {
        connfd = accept(listenfd, NULL, NULL);

        if (connfd < 0) {
            perror("accept");
            continue;
        }

        conn_num++;

        printf("Client #%d connecté\n", conn_num);
        handle_client(connfd, conn_num);
    }
    close(listenfd);
    return 0;
}
