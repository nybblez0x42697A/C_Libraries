// server.c
#include "server.h"
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

volatile sig_atomic_t running = 1;

void
handle_sigint(int sig)
{
    running = 0;
}

server_t *
server_create()
{
    server_t * server = (server_t *)malloc(sizeof(server_t));

    server->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server->sockfd < 0)
    {
        perror("Could not create socket");
        free(server);
        return NULL;
    }

    server->addr_len = sizeof(server->address);
    return server;
}

void
server_destroy(server_t * server)
{
    close(server->sockfd);
    free(server);
}

int
server_listen(server_t * server, const char * host, int port)
{
    server->address.sin_family = AF_INET;
    server->address.sin_port   = htons(port);
    inet_pton(AF_INET, host, &(server->address.sin_addr));

    if (bind(server->sockfd,
             (struct sockaddr *)&(server->address),
             server->addr_len)
        < 0)
    {
        perror("Could not bind socket to address");
        return -1;
    }

    if (listen(server->sockfd, 10) < 0)
    {
        perror("Could not listen on socket");
        return -1;
    }

    return 0;
}

client_t *
server_accept(server_t * server)
{
    int new_sockfd = accept(server->sockfd,
                            (struct sockaddr *)&(server->address),
                            (socklen_t *)&(server->addr_len));
    if (new_sockfd < 0)
    {
        perror("Could not accept client");
        return NULL;
    }

    return client_create(new_sockfd);
}

void
server_close(server_t * server)
{
    close(server->sockfd);
}

client_t *
client_create(int sockfd)
{
    client_t * client = (client_t *)malloc(sizeof(client_t));
    client->sockfd    = sockfd;
    return client;
}

void
client_destroy(client_t * client)
{
    close(client->sockfd);
    free(client);
}

ssize_t
client_read(client_t * client, void * buffer, size_t size)
{
    ssize_t bytes_read = read(client->sockfd, buffer, size);
    if (bytes_read < 0)
    {
        perror("Could not read from client");
    }

    return bytes_read;
}

ssize_t
client_write(client_t * client, const void * buffer, size_t size)
{
    ssize_t bytes_written = write(client->sockfd, buffer, size);
    if (bytes_written < 0)
    {
        perror("Could not write to client");
    }

    return bytes_written;
}

void
client_close(client_t * client)
{
    close(client->sockfd);
}

// Driver Function to test
int
main(int argc, char * argv[])
{
    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sa.sa_flags   = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT, &sa, NULL) == -1)
    {
        perror("Error setting signal handler");
        return EXIT_FAILURE;
    }

    server_t * server = server_create();
    if (!server)
    {
        fprintf(stderr, "Failed to create server\n");
        return EXIT_FAILURE;
    }

    if (server_listen(server, "127.0.0.1", 8080) == -1)
    {
        fprintf(stderr, "Failed to start server\n");
        server_destroy(server);
        return EXIT_FAILURE;
    }

    printf("Server listening on 127.0.0.1:8080\n");

    while (running)
    {
        client_t * client = server_accept(server);
        if (!client)
        {
            if (running)
            {
                fprintf(stderr, "Failed to accept client\n");
            }
            continue;
        }

        printf("Accepted a client\n");

        char buffer[1024];
        while (running)
        {
            ssize_t bytes_read
                = client_read(client, buffer, sizeof(buffer) - 1);
            if (bytes_read <= 0)
            {
                if (bytes_read && running)
                {
                    fprintf(stderr, "Failed to read from client\n");
                }
                break;
            }

            buffer[bytes_read] = '\0';
            printf("Received: %s\n", buffer);

            if (client_write(client, buffer, bytes_read) == -1)
            {
                fprintf(stderr, "Failed to write to client\n");
                break;
            }
        }

        printf("Closing client\n");
        client_close(client);
        client_destroy(client);
    }

    printf("Shutting down server\n");
    server_close(server);
    server_destroy(server);

    return EXIT_SUCCESS;
}
