#include "../include/server.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAX_THREADS 16

int connexions_actives = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/* ========= THREAD-SAFE ============*/
void afficher_statut()
{
	pthread_mutex_lock(&mutex);
	printf("Clients actifs: %d\n", connexions_actives);
	pthread_mutex_unlock(&mutex);
}

/* ========= THREAD CLIENT ============*/
void* handle_client_thread(void *arg)
{
        int connfd = *(int*)arg;
        free(arg);
	printf("THREAD ACTIF TID=%d\n", getpid());
        pthread_mutex_lock(&mutex);
        connexions_actives++;
        pthread_mutex_unlock(&mutex);
	afficher_statut();

	/* ======= Traitement  Client =======*/
	sleep(10);
        handle_client(connfd, connexions_actives);
        close(connfd);
        pthread_mutex_lock(&mutex);
        connexions_actives--;
        pthread_mutex_unlock(&mutex);
	afficher_statut();

	return NULL;
}

/* ========== MAIN ============*/
int main(void)
{
    int listenfd, connfd;
    struct sockaddr_in srv;
    int opt = 1;
    pthread_t tid;

    /* ====== Création socket ======= */
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

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

    printf("Serveur multi-thread démarré sur port %d\n", PORT);

    /* ======== BOUCLE ======== */
    while (1)
    {
        connfd = accept(listenfd, NULL, NULL);

        if (connfd < 0) {
            perror("accept");
            continue;
        }

	pthread_mutex_lock(&mutex);

        if (connexions_actives >= MAX_THREADS) 
	{
            pthread_mutex_unlock(&mutex);
	    char *msg = "Serveur saturé\n";
	    write(connfd, msg, strlen(msg));
	    close(connfd);
	    continue;
	}

	pthread_mutex_unlock(&mutex);

	int *fd = malloc(sizeof(int));
	if (fd == NULL)
	{
	    perror("malloc");
	    close(connfd);
            continue;
	}
	*fd = connfd;

	if (pthread_create(&tid, NULL, handle_client_thread, fd) != 0)
	{
	    perror("pthread_create");
	    free(fd);
            close(connfd);
            continue;
	}

	pthread_detach(tid);
    }

    close(listenfd);
    return 0;
}
