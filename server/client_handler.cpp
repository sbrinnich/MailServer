#include <strings.h>
#include <cstring>
#include <sys/socket.h>

#include "client_handler.h"
#include "operations/operation_send.h"
#include "operations/operation_list.h"
#include "operations/operation_read.h"
#include "operations/operation_del.h"

ClientHandler::ClientHandler(char *mailspooldir) : mailspooldir(mailspooldir) {}

Operation* ClientHandler::getOperation(char* buffer, int clientsocket) {
    if(strcasecmp(buffer, "SEND\n") == 0){
        return new OperationSend(clientsocket);
    }else if(strcasecmp(buffer, "LIST\n") == 0){
        return new OperationList(clientsocket);
    }else if(strcasecmp(buffer, "READ\n") == 0){
        return new OperationRead(clientsocket);
    }else if(strcasecmp(buffer, "DEL\n") == 0){
        return new OperationDel(clientsocket);
    }
    return nullptr;
}

void ClientHandler::handleClient(int clientsocket) {
    char buffer[MAXLINE];
    while(1) {
        send(clientsocket, "What do you want to do?\n", strlen("What do you want to do?\n"), 0);
        // Get Operation from first input line of client
        std::fill(buffer, buffer + sizeof(buffer), 0);
        perror("Testy");
        ssize_t size = readline(clientsocket, buffer, MAXLINE);
        perror("Testy2");

        if(strcasecmp(buffer, "QUIT\n") == 0) {
            // Close connection to client
            printf("Client closed connection\n");
            close (clientsocket);
            return;
        }else if (size <= 0) {
            // Could not read line, display error
            strcpy(buffer, "ERR\n");
            send(clientsocket, buffer, strlen(buffer), 0);
        }else{
            handleClientRequest(clientsocket, buffer);
        }
    }
}

void ClientHandler::handleClientRequest(int clientsocket, char* request_msg) {
    char buffer[MAXLINE];
    std::fill(buffer, buffer + sizeof(buffer), 0);

    Operation *op = getOperation(request_msg, clientsocket);

    // Let operation parse the rest of client input
    if(op == nullptr) {
        return;
    }
    int parse = op->parseRequest();
    if(parse != 0){
        // Operation could not parse input of client, display error
        perror("Could not parse client input.\n");
        strcpy(buffer,"ERR\n");
        send(clientsocket, buffer, strlen(buffer), 0);
        return;
    }

    // Let operation do something
    int done = op->doOperation();
    if(done != 0){
        // Operation could not do something, display error
        strcpy(buffer,"ERR\n");
        send(clientsocket, buffer, strlen(buffer), 0);
        return;
    }

    // Everything done, display OK message
    strcpy(buffer,"OK\n");
    send(clientsocket, buffer, strlen(buffer), 0);

    // Delete operation
    delete op;
}