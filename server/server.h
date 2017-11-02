#ifndef MAILSERVER_SERVER_H
#define MAILSERVER_SERVER_H

#include <cstdio>
#include <cstring>
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

public:
    Server(char *mailspooldir, int port);
    int checkPort();
    int checkDir();
    int bindSocket();
    void listenForClients();
};

#endif //MAILSERVER_SERVER_H
