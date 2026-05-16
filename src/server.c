#include "../include/server.h"
#include "../include/utils.h"

#include <signal.h>
#include <sys/wait.h>

/* ========== SIGCHLD ==========*/
/* Objectif: éviter les processus zombies */

void sigchld_handler(int sig) 
{
	(void)sig;
	while (waitpid(-1, NULL, WNOHANG) > 0);
}

/* =========== MAIN ============*/

int main(void)
{
    int listenfd, connfd;
    struct sockaddr_in srv;
    int opt = 1;
    pid_t pid;
    int conn_num;

    /* Gestion zombies */
    signal(SIGCHLD, sigchld_handler);

    /* Création socket */
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    if (listenfd < 0)
    {
        perror("socket");
        return 1;
    }

    /* SO_REUSEADDR */
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

    if (listen(listenfd, BACKLOG) < 0) {
        perror("listen");
        return 1;
    }

    printf("Serveur concurrent démarré sur port %d\n", PORT);

    /* ======== BOUCLE ======== */
    while (1)
    {
        connfd = accept(listenfd, NULL, NULL);

        if (connfd < 0) {
            perror("accept");
            continue;
        }

	/* IPC computeur propre */
        conn_num = get_conn_count() + 1;
	update_conn_count(conn_num);

	pid = fork();

        if (pid == 0) 
	{
	/* ====== FILS ========*/
            close(listenfd);

	    printf("Client #%d traité (PID%d)\n", conn_num, getpid());

            handle_client(connfd, conn_num);

	    close(connfd);
	    return 0;
	}
	else if (pid > 0)
	{
	    close(connfd);
	}
	else
	{
	/* ====== PERE ========*/
	    perror("fork");
	}
    }

    return 0;
}
