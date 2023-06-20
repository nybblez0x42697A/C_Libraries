#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>
#include <signal.h>

volatile sig_atomic_t running = 1;

typedef struct server_t
{
    int                     tcp_sockfd;
    int                     udp_sockfd;
    int                     protocol;
    struct sockaddr_storage address;
    socklen_t               addr_len;
    struct sockaddr_storage udp_address;
    socklen_t               udp_addr_len;
} server_t;

typedef struct client_t
{
    int                     sockfd;
    struct sockaddr_storage address;
    socklen_t               addr_len;
} client_t;

void       handle_sigint(int sig);
server_t * server_create(int protocol_flags);
void       server_destroy(server_t * server);
int  server_listen(server_t * server, const char * host, const char * port);
void server_close(server_t * server);

client_t * client_create(int sockfd);
void       client_destroy(client_t * client);
ssize_t    client_read(client_t * client, void * buffer, size_t size);
ssize_t    client_write(client_t * client, const void * buffer, size_t size);
void       client_close(client_t * client);

#endif /* SERVER_H */
