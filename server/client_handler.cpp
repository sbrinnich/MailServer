#include <strings.h>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

#include "client_handler.h"
#include "operations/operation_send.h"
#include "operations/operation_list.h"
#include "operations/operation_read.h"
#include "operations/operation_del.h"

ClientHandler::ClientHandler(char *mailspooldir) : mailspooldir(mailspooldir) {}

Operation* ClientHandler::getOperation(char* buffer, int clientsocket) {
    if(strcasecmp(buffer, "SEND\n") == 0){
        return new OperationSend(clientsocket, mailspooldir);
    }else if(strcasecmp(buffer, "LIST\n") == 0){
        return new OperationList(clientsocket, mailspooldir);
    }else if(strcasecmp(buffer, "READ\n") == 0){
        return new OperationRead(clientsocket, mailspooldir);
    }else if(strcasecmp(buffer, "DEL\n") == 0){
        return new OperationDel(clientsocket, mailspooldir);
    }
    return nullptr;
}

void ClientHandler::handleClient(int clientsocket) {
    char buffer[MAXLINE];
    int status = -1;
    char msg[MAXLINE];
    while(1) {
        // Display message to client
        switch(status){
            case 0:
                strcpy(msg, "OK\n\nWhat do you want to do?\n");
                break;
            case 1:
                strcpy(msg, "ERR\n\nWhat do you want to do?\n");
                break;
            default:
                strcpy(msg, "What do you want to do?\n");
                break;
        }
        send(clientsocket, msg, strlen(msg), 0);

        // Get Operation from first input line of client
        std::fill(buffer, buffer + sizeof(buffer), 0);
        ssize_t size = recv(clientsocket, buffer, MAXLINE-1, 0);

        if(size <= 0) {
            // Could not read line, display error
            strcpy(buffer, "ERR\n");
            send(clientsocket, buffer, strlen(buffer), 0);
        }else {
            buffer[size] = '\0';

            if (strcasecmp(buffer, "QUIT\n") == 0) {
                // Close connection to client
                printf("Client closed connection.\n");
                close(clientsocket);
                return;
            } else {
                status = handleClientRequest(clientsocket, buffer);
            }
        }
    }
}

int ClientHandler::handleClientRequest(int clientsocket, char* request_msg) {
    char buffer[MAXLINE];
    std::fill(buffer, buffer + sizeof(buffer), 0);

    Operation *op = getOperation(request_msg, clientsocket);

    if(op == nullptr) {
        return 1;
    }

    // Let operation parse the rest of client input
    int parse = op->parseRequest();
    if(parse != 0){
        // Operation could not parse input of client
        perror("Could not parse client input.\n");
        return 1;
    }

    // Let operation do something
    int done = op->doOperation();
    if(done != 0){
        // Operation could not do something
        return 1;
    }

    // Delete operation
    delete op;

    // Everything done
    return 0;
}