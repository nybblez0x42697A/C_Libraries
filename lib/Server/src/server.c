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
server_create(int protocol_flags)
{
    server_t * server  = calloc(1, sizeof(server_t));
    server->tcp_sockfd = -1;
    server->udp_sockfd = -1;

    if (protocol_flags & SOCK_STREAM)
    {
        server->tcp_sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (server->tcp_sockfd < 0)
        {
            perror("Could not create TCP socket");
            goto TCP_ERROR;
        }
    }

    if (protocol_flags & SOCK_DGRAM)
    {
        server->udp_sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (server->udp_sockfd < 0)
        {
            perror("Could not create UDP socket");
            if (server->tcp_sockfd >= 0)
            {
                goto UDP_ERROR;
            }
            else
            {
                goto TCP_ERROR;
            }
        }
    }
    goto EXIT;

UDP_ERROR:
    close(server->tcp_sockfd);

TCP_ERROR:
    free(server);

EXIT:
    return server;
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

int
server_listen(server_t * server, const char * host, const char * service)
{
    struct addrinfo   hints;
    struct addrinfo * res_tcp = NULL;
    struct addrinfo * res_udp = NULL;
    int               err;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags  = AI_PASSIVE;

    // TCP socket
    if (server->tcp_sockfd > 0)
    {

        hints.ai_socktype = SOCK_STREAM;
        if ((err = getaddrinfo(host, service, &hints, &res_tcp)) != 0)
        {
            fprintf(stderr, "getaddrinfo (TCP): %s\n", gai_strerror(err));
            return -1;
        }

        if (bind(server->tcp_sockfd, res_tcp->ai_addr, res_tcp->ai_addrlen)
            < 0)
        {
            perror("Could not bind TCP socket to address");
            freeaddrinfo(res_tcp);
            return -1;
        }

        if (listen(server->tcp_sockfd, 10) < 0)
        {
            perror("Could not listen on TCP socket");
            freeaddrinfo(res_tcp);
            return -1;
        }
    }

    // Retrieve the actual port number assigned to the server sockets
    if (res_tcp != NULL)
    {
        get_server_address_info(
            server->tcp_sockfd, &(server->address), &(server->addr_len));
        freeaddrinfo(res_tcp);
    }

    // UDP socket
    if (server->udp_sockfd > 0)
    {

        hints.ai_socktype = SOCK_DGRAM;

        if ((err = getaddrinfo(host, service, &hints, &res_udp)) != 0)
        {
            fprintf(stderr, "getaddrinfo (UDP): %s\n", gai_strerror(err));
            return -1;
        }

        if (bind(server->udp_sockfd, res_udp->ai_addr, res_udp->ai_addrlen)
            < 0)
        {
            perror("Could not bind UDP socket to address");
            freeaddrinfo(res_tcp);
            freeaddrinfo(res_udp);
            return -1;
        }
    }

    if (res_udp != NULL)
    {
        // Copy the UDP address information to the server structure
        memcpy(&(server->udp_address), res_udp->ai_addr, res_udp->ai_addrlen);
        server->udp_addr_len = res_udp->ai_addrlen;
        freeaddrinfo(res_udp);
    }
    print_server_info(server);
    return 0;
}

client_t *
client_create(int sockfd)
{
    client_t * client = calloc(1, sizeof(client_t));
    client->sockfd    = sockfd;
    return client;
}
