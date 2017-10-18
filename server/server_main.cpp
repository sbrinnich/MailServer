#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <dirent.h>

#include "client_handler.h"

int main (int argc, char** argv) {
    // Check if all parameters are specified
    if(argc != 3){
        printf("Invalid parameters specified!\n");
        printf("Usage: %s [port] [mailspooldirectory]\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Check given port
    int port;
    std::stringstream s(argv[1]);
    s >> port;
    if(s.fail() || port <= 0){
        printf("Invalid parameters specified! Not a valid port number!\n");
        printf("Usage: %s [port] [mailspooldirectory]\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Check given directory (check if directory exists)
    char* directory;
    directory = argv[2];
    DIR* dir = opendir(directory);
    if(!dir){
        printf("Invalid parameters specified! Could not access given directory!\n");
        printf("Usage: %s [port] [mailspooldirectory]\n", argv[0]);
        return EXIT_FAILURE;
    }
    closedir(dir);

    int create_socket, new_socket;
    socklen_t addrlen;
    struct sockaddr_in address, cliaddress;

    // Create instance of clientHandler
    ClientHandler* clientHandler = new ClientHandler(directory);

    // Create socket
    create_socket = socket (AF_INET, SOCK_STREAM, 0);

    // Define socket information (IPv4, listening port)
    memset(&address,0,sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons (port);

    // Bind server socket
    if (bind ( create_socket, (struct sockaddr *) &address, sizeof (address)) != 0) {
        perror("bind error");
        return EXIT_FAILURE;
    }

    // Listen for up to 5 clients
    listen (create_socket, 5);

    addrlen = sizeof (struct sockaddr_in);

    while (1) {
        // Wait for clients to connect to server
        printf("Waiting for connections...\n");
        new_socket = accept ( create_socket, (struct sockaddr *) &cliaddress, &addrlen );

        if (new_socket > 0) {
            // New client connected, give socket to clientHandler
            printf ("Client connected from %s:%d...\n", inet_ntoa (cliaddress.sin_addr),ntohs(cliaddress.sin_port));
            clientHandler->handleClient(new_socket);
        }
    }

    // Close server socket
    close (create_socket);
    delete clientHandler;

    return EXIT_SUCCESS;
}