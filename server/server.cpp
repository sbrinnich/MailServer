#include "server.h"

void clientConnect(int clientSocket, char *mailspooldir){
    ClientHandler* clientHandler = new ClientHandler(mailspooldir);
    clientHandler->handleClient(clientSocket);
    delete clientHandler;
}

Server::Server(char *mailspooldir, int port) : mailspooldir(mailspooldir), port(port), server_socket(0){
}

int Server::checkPort() {
    if(port <= 0){
        return 1;
    }
    return 0;
}

int Server::checkDir() {
    if(mailspooldir == nullptr || strcmp(mailspooldir, "") == 0){
        return 1;
    }
    DIR* dir = opendir(mailspooldir);
    if(!dir){
        // TODO ask if directory should be created
        return 1;
    }
    closedir(dir);
    return 0;
}

int Server::bindSocket() {
    struct sockaddr_in address;

    // Create socket
    server_socket = socket (AF_INET, SOCK_STREAM, 0);

    // Define socket information (IPv4, listening port)
    memset(&address,0,sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons ((uint16_t) port);

    // Bind server socket
    if (bind ( server_socket, (struct sockaddr *) &address, sizeof (address)) != 0) {
        perror("bind error");
        return 1;
    }

    return 0;
}

void Server::listenForClients() {
    socklen_t addrlen;
    int client_socket;
    struct sockaddr_in cliaddress;

    // Listen for up to 5 clients
    listen(server_socket, 5);


    addrlen = sizeof (struct sockaddr_in);

    while (1) {
        // Wait for clients to connect to server
        printf("Waiting for connections...\n");
        client_socket = accept ( server_socket, (struct sockaddr *) &cliaddress, &addrlen );

        if (client_socket > 0) {
            // New client connected, give socket to clientHandler
            printf ("Client connected from %s:%d...\n", inet_ntoa (cliaddress.sin_addr),ntohs(cliaddress.sin_port));

            std::thread t(clientConnect, client_socket, mailspooldir);
            t.detach();
        }
    }

    // Close server socket
    close (server_socket);
}