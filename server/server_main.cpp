#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "client_handler.h"

#define BUF 1024
#define PORT 6543

int main (void) {
    int create_socket, new_socket;
    socklen_t addrlen;
    char buffer[BUF];
    int size;
    struct sockaddr_in address, cliaddress;

    ClientHandler* clientHandler = new ClientHandler();

    create_socket = socket (AF_INET, SOCK_STREAM, 0);

    memset(&address,0,sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons (PORT);

    if (bind ( create_socket, (struct sockaddr *) &address, sizeof (address)) != 0) {
        perror("bind error");
        return EXIT_FAILURE;
    }

#ifdef SO_REUSEPORT
    int reuse = 1;
    if (setsockopt(create_socket, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0)
        perror("setsockopt(SO_REUSEPORT) failed");
#endif

    listen (create_socket, 5);

    addrlen = sizeof (struct sockaddr_in);

    while (1) {
        // Wait for clients to connect to server
        printf("Waiting for connections...\n");
        new_socket = accept ( create_socket, (struct sockaddr *) &cliaddress, &addrlen );
        if (new_socket > 0)
        {
            // New client connected, send welcome message
            printf ("Client connected from %s:%d...\n", inet_ntoa (cliaddress.sin_addr),ntohs(cliaddress.sin_port));
            strcpy(buffer,"Welcome to mailserver, Please enter your command:\n");
            send(new_socket, buffer, strlen(buffer),0);
            clientHandler->handleClient(new_socket);
        }
        /*
        do {
            // Handle client requests
            size = recv (new_socket, buffer, BUF-1, 0);
            if( size > 0)
            {
                buffer[size] = '\0';
                printf ("Message received: %s\n", buffer);
            }
            else if (size == 0)
            {
                printf("Client closed remote socket\n");
                break;
            }
            else
            {
                perror("recv error");
                return EXIT_FAILURE;
            }
        } while (strncmp (buffer, "quit", 4)  != 0);*/
        // Close socket of client
        close (new_socket);
    }
    // Close server socket
    close (create_socket);
    delete clientHandler;
    return EXIT_SUCCESS;
}