#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <sstream>

#include "server.h"

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
    if(s.fail()){
        printf("Invalid parameters specified! Not a valid port number!\n");
        printf("Usage: %s [port] [mailspooldirectory]\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Check given directory
    char* directory;
    directory = argv[2];

    // Create server
    Server *server = new Server(directory, port);
    int stat = server->startServer();

    delete server;

    if(stat != 0){
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}