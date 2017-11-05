#ifndef MAILSERVER_SERVER_H
#define MAILSERVER_SERVER_H

#define IP_BLOCK_MINUTES 2

#include <cstdio>
#include <cstring>
#include <unordered_map>
#include <sstream>
#include <fstream>

#include <dirent.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <thread>
#include <pthread.h>

#include "client_handler.h"

class Server {
private:
    char *mailspooldir;
    int port;
    int server_socket;
    bool listening;

    std::unordered_map<std::string, time_t> blocked_clients;

    void readBlockedClients();
    void writeBlockedClients();
    void checkBlockedClients();
    std::string getIpFromSocket(int socket);
    void clientConnect(int clientSocket);

public:
    Server(char *mailspooldir, int port);
    int checkPort();
    int checkDir();
    int bindSocket();
    void listenForClients();
    /**
     * Adds current client's IP address to a blacklist to stop him from connecting again in a while
     */
    void blockClient(int clientSocket);
    void initBlacklist();
    int startServer();
};

#endif //MAILSERVER_SERVER_H
