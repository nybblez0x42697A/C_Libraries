#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "../include/server.h"

static void
print_server_info(server_t * server)
{
    char         buffer[INET6_ADDRSTRLEN] = { 0 };
    const char * ip_version
        = (server->address.ss_family == AF_INET6) ? "IPv6" : "IPv4";
    const char * ip_address = NULL;

    int port_number = 0;

    if (server->address.ss_family == AF_INET6)
    {
        struct sockaddr_in6 * ipv6_addr
            = (struct sockaddr_in6 *)&(server->address);
        ip_address = inet_ntop(
            AF_INET6, &(ipv6_addr->sin6_addr), buffer, sizeof(buffer));
        port_number = ntohs(ipv6_addr->sin6_port);
    }
    else if (server->address.ss_family == AF_INET)
    {
        struct sockaddr_in * ipv4_addr
            = (struct sockaddr_in *)&(server->address);
        ip_address = inet_ntop(
            AF_INET, &(ipv4_addr->sin_addr), buffer, sizeof(buffer));
        port_number = ntohs(ipv4_addr->sin_port);
    }
    else
    {
        fprintf(stderr, "Invalid address family\n");
        return;
    }

    printf(
        "Server listening on %s %s:%d", ip_version, ip_address, port_number);

    // Check if TCP socket is listening
    if (server->tcp_sockfd >= 0)
    {
        printf(" (TCP)");
    }

    // Check if UDP socket is listening
    if (server->udp_sockfd >= 0)
    {
        printf(" (UDP)");
    }

    printf("\n");
}

static void
get_server_address_info(int                       sockfd,
                        struct sockaddr_storage * addr,
                        socklen_t *               addrlen)
{
    *addrlen = sizeof(*addr);
    if (getsockname(sockfd, (struct sockaddr *)addr, addrlen) < 0)
    {
        perror("Could not retrieve server socket information");
        exit(EXIT_FAILURE);
    }
}

server_t *
server_create(void)
{
    server_t * server;

    server = calloc(1, sizeof(server_t));
    if (NULL == server)
    {
        perror("Could not allocate memory for server");
        return NULL;
    }

    server->tcp_sockfd = -1;
    server->udp_sockfd = -1;
    memset(&(server->address), 0, sizeof(server->address));
    server->addr_len = sizeof(server->address);

    memset(&(server->udp_address), 0, sizeof(server->udp_address));
    server->udp_addr_len = sizeof(server->udp_address);

    return server;
}

int
server_listen(server_t * server, const char * host, const char * service)
{
    struct addrinfo   hints;
    struct addrinfo * res = NULL;
    int               err;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = 0;
    hints.ai_flags    = AI_PASSIVE;

    if ((err = getaddrinfo(host, service, &hints, &res)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
        return -1;
    }

    struct addrinfo * p;
    for (p = res; p != NULL; p = p->ai_next)
    {
        if (p->ai_socktype == SOCK_STREAM)
        {
            server->tcp_sockfd
                = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
            if (server->tcp_sockfd == -1)
            {
                perror("Could not create TCP socket");
                continue;
            }

            int optval = 1;
            setsockopt(server->tcp_sockfd,
                       SOL_SOCKET,
                       SO_REUSEADDR,
                       &optval,
                       sizeof(optval));
            setsockopt(server->tcp_sockfd,
                       SOL_SOCKET,
                       SO_REUSEPORT,
                       &optval,
                       sizeof(optval));

            if (bind(server->tcp_sockfd, p->ai_addr, p->ai_addrlen) < 0)
            {
                perror("Could not bind TCP socket to address");
                close(server->tcp_sockfd);
                server->tcp_sockfd = -1;
                continue;
            }

            if (listen(server->tcp_sockfd, 10) < 0)
            {
                perror("Could not listen on TCP socket");
                close(server->tcp_sockfd);
                server->tcp_sockfd = -1;
                continue;
            }

            get_server_address_info(
                server->tcp_sockfd, &(server->address), &(server->addr_len));
        }
        else if (p->ai_socktype == SOCK_DGRAM)
        {
            server->udp_sockfd
                = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
            if (server->udp_sockfd == -1)
            {
                perror("Could not create UDP socket");
                continue;
            }

            int optval = 1;
            setsockopt(server->udp_sockfd,
                       SOL_SOCKET,
                       SO_REUSEADDR,
                       &optval,
                       sizeof(optval));
            setsockopt(server->udp_sockfd,
                       SOL_SOCKET,
                       SO_REUSEPORT,
                       &optval,
                       sizeof(optval));

            if (bind(server->udp_sockfd, p->ai_addr, p->ai_addrlen) < 0)
            {
                perror("Could not bind UDP socket to address");
                close(server->udp_sockfd);
                server->udp_sockfd = -1;
                continue;
            }

            // Copy the UDP address information to the server structure
            memcpy(&(server->udp_address), p->ai_addr, p->ai_addrlen);
            server->udp_addr_len = p->ai_addrlen;
        }
    }

    if (server->tcp_sockfd == -1 && server->udp_sockfd == -1)
    {
        fprintf(stderr, "Could not bind socket to any address\n");
        freeaddrinfo(res);
        return -1;
    }

    freeaddrinfo(res);

    print_server_info(server);
    return 0;
}

void
server_destroy(server_t * server)
{
    if (server->tcp_sockfd >= 0)
    {
        close(server->tcp_sockfd);
    }
    if (server->udp_sockfd >= 0)
    {
        close(server->udp_sockfd);
    }
    free(server);
    server = NULL;
}

client_t *
client_create(int sockfd)
{
    client_t * client = calloc(1, sizeof(client_t));
    client->sockfd    = sockfd;
    return client;
}
