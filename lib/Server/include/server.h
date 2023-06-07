// server.h
#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <stdlib.h>

typedef struct
{
    int                sockfd;
    struct sockaddr_in address;
    int                addr_len;
} server_t;

typedef struct
{
    int sockfd;
} client_t;

server_t * server_create();
void       server_destroy(server_t * server);

int        server_listen(server_t * server, const char * host, int port);
client_t * server_accept(server_t * server);
void       server_close(server_t * server);

client_t * client_create(int sockfd);
void       client_destroy(client_t * client);

ssize_t client_read(client_t * client, void * buffer, size_t size);
ssize_t client_write(client_t * client, const void * buffer, size_t size);
void    client_close(client_t * client);

#endif // SERVER_H
