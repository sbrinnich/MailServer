#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sstream>

#define BUF 1024

int main (int argc, char **argv) {
    int create_socket;
    char buffer[BUF];
    std::fill(buffer, buffer + sizeof(buffer), 0);
    struct sockaddr_in address;
    int size;

    if( argc != 3 ){
        printf("Invalid parameters specified!\n");
        printf("Usage: %s [server_address] [port]\n", argv[0]);
        return EXIT_FAILURE;
    }

    int port;
    std::stringstream s(argv[2]);
    s >> port;
    if(s.fail() || port <= 0){
        printf("Invalid parameters specified! Not a valid port number!\n");
        printf("Usage: %s [server_address] [port]\n", argv[0]);
        return EXIT_FAILURE;
    }

    if ((create_socket = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket error");
        return EXIT_FAILURE;
    }

    memset(&address,0,sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons (port);
    inet_aton (argv[1], &address.sin_addr);

    if (connect ( create_socket, (struct sockaddr *) &address, sizeof (address)) == 0)
    {
        printf ("Connection with server (%s) established\n", inet_ntoa (address.sin_addr));
        std::fill(buffer, buffer + sizeof(buffer), 0);
    }
    else
    {
        perror("Connect error - no server available");
        return EXIT_FAILURE;
    }

    do {
        std::fill(buffer, buffer + sizeof(buffer), 0);
        size=recv(create_socket,buffer,BUF-1, 0);
        if (size>0)
        {
            buffer[size]= '\0';
            printf("%s",buffer);
        }
        std::fill(buffer, buffer + sizeof(buffer), 0);
        char buff[BUF];
        std::fill(buff, buff + sizeof(buff), 0);
        char* fgetret = std::fgets (buff, BUF, stdin);
        if(fgetret != nullptr) {
            strcpy(buffer, buff);
            send(create_socket, buff, strlen(buff), 0);
        }
    }
    while (strcmp (buffer, "quit\n") != 0);
    printf ("Connection to server closed.\n");
    close (create_socket);
    return EXIT_SUCCESS;
}