#include "../include/server.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define MAX_CLIENTS FD_SETSIZE
#define TIMEOUT_SEC 5

int main(void)
{
    int listenfd, connfd, maxfd, activity, i;
    int clients[MAX_CLIENTS];
    struct sockaddr_in srv, cli;
    socklen_t len = sizeof(cli);

    fd_set readfds;
    struct timeval timeout;

    for (i = 0; i < MAX_CLIENTS; i++)
	clients[i] = -1;

    /* ====== Création socket ======= */
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0)
    {
	perror("socket");
        return 1;
}

    int opt = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    /* Configuration serveur */
    srv.sin_family = AF_INET;
    srv.sin_addr.s_addr = INADDR_ANY;
    srv.sin_port = htons(PORT);

    if (bind(listenfd, (struct sockaddr*)&srv, sizeof(srv)) < 0)
    {
	perror("bind");
	return 1;
    }

    if (listen(listenfd, BACKLOG) < 0)
    {
        perror("listen");
        return 1;
    }

    printf("Serveur SELECT démarré sur port %d\n", PORT);

    /* ======== BOUCLE ======== */
    while (1)
    {
	FD_ZERO(&readfds);
	FD_SET(listenfd, &readfds);
	maxfd = listenfd;

        for (i = 0; i < MAX_CLIENTS; i++)
	{
	    if (clients[i] != -1)
	    {
		FD_SET(clients[i], &readfds);
		if (clients[i] > maxfd)
		    maxfd = clients[i];
	    }
	}

	timeout.tv_sec = TIMEOUT_SEC;
	timeout.tv_usec = 0;

	activity = select(maxfd + 1, &readfds, NULL, NULL, &timeout);

        if (activity < 0) {
            perror("select");
            continue;
        }

        if (FD_ISSET(listenfd, &readfds))
	{
            connfd = accept(listenfd, (struct sockaddr*)&cli, &len);
	    if (connfd < 0)
	    {
		perror("accept");
            	continue;
	    }

	    for (i = 0; i < MAX_CLIENTS; i++)
	    {
		if (clients[i] == -1)
		{
		    clients[i] = connfd;
		    break;
		}
	    }
	}

	for (i = 0; i < MAX_CLIENTS; i++)
        {
            if (clients[i] != -1 && FD_ISSET(clients[i], &readfds))
            {
                char buffer[1024];
		int n = read(clients[i], buffer, sizeof(buffer) - 1);

                if (n <= 0)
                {
		    close(clients[i]);
		    clients[i] = -1;
		}
                else
                {
                    buffer[n] = '\0';
		    printf("Client: %s\n", buffer);
                    write(clients[i], "OK\n", 3);
                }

            }

        }

	int count = 0;
	for (i = 0; i < MAX_CLIENTS; i++)
	    if (clients[i] != -1)
		count++;

	printf("Connexions actives: %d\n", count);
    }

    return 0;
}
