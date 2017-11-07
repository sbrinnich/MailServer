#include "server.h"

void Server::clientConnect(int clientSocket){
    // Lock map
    {
        std::lock_guard<std::mutex> lock(blacklist_map_mutex);

        // Check if client is blocked
        auto it = blocked_clients.find(getIpFromSocket(clientSocket));
        if (it != blocked_clients.end()) {
            if (it->second > time(nullptr)) {
                printf("Blocked client tried to connect.\n");
                send(clientSocket, "Your IP is blocked for a limited time!\n",
                     strlen("Your IP is blocked for a limited time!\n"), 0);
                close(clientSocket);
                return;
            } else {
                // Blocked time ran out, client allowed to connect again
                blocked_clients.erase(it->first);
            }
        }
    } // Release lock

    auto clientHandler = new ClientHandler(mailspooldir);
    int status = clientHandler->handleClient(clientSocket);
    switch(status){
        case 1:
            // Client should be blocked
            blockClient(clientSocket);
            printf("Client blocked because of too many wrong login tries.\n");
            send(clientSocket, "ERR\n\nToo many wrong tries! Your IP got blocked for a limited time!\n",
                 strlen("ERR\n\nToo many wrong tries! Your IP got blocked for a limited time!\n"), 0);
            break;
        default:
            printf("Client closed connection.\n");
            break;
    }

    close(clientSocket);
    delete clientHandler;
}

Server::Server(char *mailspooldir, int port) :
        mailspooldir(mailspooldir), port(port), server_socket(0), listening(true){
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
        // Ask if directory should be created
        printf("Directory does not exist! Do you want to create it? (y/n) ");
        auto buffer = new char[MAXLINE];
        char *fgetret = std::fgets(buffer, MAXLINE, stdin);
        if (fgetret != nullptr && strcmp(buffer, "y\n") == 0) {
            delete[] buffer;
            if(mkdir(mailspooldir, 0755) != 0){
                printf("Directory could not be created!\n");
                return 1;
            }
            printf("Directory created!\n");
            return 0;
        }
        delete[] buffer;
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
    if (bind(server_socket, (struct sockaddr *) &address, sizeof(address)) != 0) {
        perror("bind error");
        return 1;
    }

    return 0;
}

void Server::listenForClients() {
    socklen_t addrlen;
    int client_socket;
    struct sockaddr_in cliaddress;
    fd_set fds;

    // Set timeout to 5 seconds
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;


    // Listen for up to 5 clients
    listen(server_socket, 5);

    addrlen = sizeof (struct sockaddr_in);

    printf("Waiting for connections...\n");

    while (listening) {
        // Reset fd_set
        FD_ZERO(&fds);
        FD_SET(server_socket, &fds);

        // Wait for clients to connect to server (timeout after 5 seconds)
        if(select(server_socket+1, &fds, (fd_set *) nullptr, (fd_set *) nullptr, &timeout) > 0){
            // New client connected
            client_socket = accept(server_socket, (struct sockaddr *) &cliaddress, &addrlen);
            printf("Client connected from %s:%d...\n", inet_ntoa (cliaddress.sin_addr),ntohs(cliaddress.sin_port));

            // Create new thread for client
            std::thread t(&Server::clientConnect, this, client_socket);
            t.detach();
        }
    }

    // Close server socket
    close (server_socket);
}

void Server::readBlockedClients() {
    std::stringstream filepath;
    filepath << mailspooldir << "/blacklist.txt";

    std::string ip;
    time_t blockingtime;

    std::ifstream file;

    // Lock file
    std::lock_guard<std::mutex> lock(blacklist_file_mutex);
    file.open(filepath.str(), std::ios::in);
    if(file.is_open()){
        while(file >> ip >> blockingtime){
            if(blockingtime > time(nullptr)){
                blocked_clients[ip] = blockingtime;
            }
        }
    }
    file.close();
}

void Server::writeBlockedClients() {
    printf("Writing blacklist...\n");
    // Check if blocking time of some clients expired
    checkBlockedClients();

    // Write from map into file
    std::stringstream filepath;
    filepath << mailspooldir << "/blacklist.txt";

    std::ofstream file;

    // Lock file
    std::lock_guard<std::mutex> lock(blacklist_file_mutex);

    file.open(filepath.str(), std::ios::out | std::ios::trunc);
    if(file.is_open()){
        for(auto &blocked_client : blocked_clients) {
            file << blocked_client.first << " " << blocked_client.second << std::endl;
        }
        printf("Write complete! Blacklist now up-to-date!\n");
    }else{
        perror("Could not write blocked clients into file!");
    }
    file.close();
}

void Server::checkBlockedClients() {
    // Lock map
    std::lock_guard<std::mutex> lock(blacklist_map_mutex);
    for(auto it = blocked_clients.begin(); it != blocked_clients.end(); it++){
        if(it->second <= time(nullptr)){
            // Time expired, remove client from list
            blocked_clients.erase(it->first);
        }
    }
}

std::string Server::getIpFromSocket(int socket) {
    // Get client ip from socket
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);
    int res = getpeername(socket, (struct sockaddr *) &addr, &addr_size);
    if(res != 0){
        perror("Could not get client's ip address! getpeername returned "+res);
        return "";
    }
    return inet_ntoa(addr.sin_addr);
}

void Server::blockClient(int clientSocket) {
    std::string ip = getIpFromSocket(clientSocket);
    if(strcmp(ip.c_str(), "") == 0){
        perror("Could not block client!");
        return;
    }

    // Get time for when client is not blocked again
    time_t until = time(nullptr)+IP_BLOCK_MINUTES*60;

    // Write client to list
    {
        std::lock_guard<std::mutex> lock(blacklist_map_mutex);
        blocked_clients[ip] = until;
    } // Release lock

    // Write client to file
    std::stringstream filepath;
    filepath << mailspooldir << "/blacklist.txt";

    std::ofstream file;

    // Lock file
    std::lock_guard<std::mutex> lock(blacklist_file_mutex);

    file.open(filepath.str(), std::ios::out | std::ios::app);
    if(file.is_open()){
        file << ip << " " << until << std::endl;
    }else{
        perror("Could not write client into blacklist file!");
    }
    file.close();
}

void Server::initBlacklist() {
    // Read currently listed clients from file
    readBlockedClients();
    // Write updated list of blocked clients back to file
    writeBlockedClients();
}

int Server::startServer() {
    if(checkPort() != 0 || checkDir() != 0){
        return 1;
    }
    if(bindSocket() != 0){
        return 1;
    }
    initBlacklist();

    // Listen for clients in different thread
    std::thread listeningThread(&Server::listenForClients, this);

    // Get optional input from server console
    char buffer[MAXLINE];
    do {
        std::fill(buffer, buffer + sizeof(buffer), 0);
        char *fgetret = std::fgets(buffer, MAXLINE, stdin);
        if (fgetret != nullptr) {
            if(strcasecmp(buffer, "write\n") == 0){
                writeBlockedClients();
            }else if(strcasecmp(buffer, "quit\n") == 0){
                listening = false;
            }
        }
    }while(listening);

    // Join listening thread
    printf("Exiting...\n");
    listeningThread.join();
    return 0;
}