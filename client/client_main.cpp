#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <sstream>

#define BUF 16384

int main (int argc, char **argv) {
    int create_socket;
    char buffer[BUF];
    std::fill(buffer, buffer + sizeof(buffer), 0);
    struct sockaddr_in address;
    int size;

    // Check if all parameters are specified
    if( argc != 3 ){
        printf("Invalid parameters specified!\n");
        printf("Usage: %s [server_address] [port]\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Check given port
    int port;
    std::stringstream s(argv[2]);
    s >> port;
    if(s.fail() || port <= 0){
        printf("Invalid parameters specified! Not a valid port number!\n");
        printf("Usage: %s [server_address] [port]\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Create socket
    if ((create_socket = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket error");
        return EXIT_FAILURE;
    }

    // Define address and port of remote server
    memset(&address,0,sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons (port);
    inet_aton (argv[1], &address.sin_addr);

    // Try to connect to remote server
    if (connect ( create_socket, (struct sockaddr *) &address, sizeof (address)) == 0) {
        printf ("Connection with server (%s) established\n", inet_ntoa (address.sin_addr));
        std::fill(buffer, buffer + sizeof(buffer), 0);
    }else{
        perror("Connect error - no server available");
        return EXIT_FAILURE;
    }

    // Loop send and receive
    do {
        // Clear buffer
        std::fill(buffer, buffer + sizeof(buffer), 0);

        // Receive message from server
        size = recv(create_socket,buffer,BUF-1, 0);
        if (size>0)
        {
            buffer[size]= '\0';
            printf("%s",buffer);
        }

        // Clear buffer
        std::fill(buffer, buffer + sizeof(buffer), 0);

        // Read message from stdin and send to server
        char* fgetret = std::fgets (buffer, BUF, stdin);
        if(fgetret != nullptr) {
            send(create_socket, buffer, strlen(buffer), 0);
        }
    }while (strcmp (buffer, "quit\n") != 0);

    // Close connection
    printf ("Connection to server closed.\n");
    close (create_socket);

    return EXIT_SUCCESS;
}