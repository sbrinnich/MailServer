#include <strings.h>
#include <cstring>
#include <sys/socket.h>

#include "socket_read.h"
#include "client_handler.h"
#include "operations/operation_send.h"
#include "operations/operation_list.h"
#include "operations/operation_read.h"
#include "operations/operation_del.h"

Operation* ClientHandler::getOperation(char* buffer, int clientsocket) {
    if(strcasecmp(buffer, "SEND\n") == 0){//WO WAR DER BACKSLASH????????????????????????????
        return new OperationSend(clientsocket);
    }else if(strcasecmp(buffer, "LIST\n") == 0){
        return new OperationList(clientsocket);
    }else if(strcasecmp(buffer, "READ\n") == 0){
        return new OperationRead(clientsocket);
    }else if(strcasecmp(buffer, "DEL\n") == 0){
        return new OperationDel(clientsocket);
    }
    return nullptr; //ist das sicher eine gute idee?
}

void ClientHandler::handleClient(int clientsocket) {
    // Get Operation from first input line of client
    char buffer[MAXLINE];
    std::fill(buffer, buffer + sizeof(buffer), 0);//vorher war irgendwas da drin...und das ist nie schön...
    ssize_t size = readline(clientsocket, buffer, MAXLINE);

    if(size <= 0) {
        // Could not read line, display error
        strcpy(buffer,"ERR\n");
        send(clientsocket, buffer, strlen(buffer), 0);
        return;
    }

    Operation *op = getOperation(buffer, clientsocket);

    // Let operation parse the rest of client input
    if(op != nullptr){
        int parse = op->parseRequest();
        if(parse != 0){
            // Operation could not parse input of client, display error
            perror("Could not parse client input.\n");
            strcpy(buffer,"ERR\n");
            send(clientsocket, buffer, strlen(buffer), 0);
            return;
        }
    }else{
        //printf("I ate bugs and you input weird stuff\n"); best typo
        perror("Invalid Input.\n");
        return;

    }
    /*  int parse = op->parseRequest(); // da ist der segv gewesen wegen dem nullptr den das zurück gibt...

    if(parse != 0){
        // Operation could not parse input of client, display error
        strcpy(buffer,"ERR\n");
        send(clientsocket, buffer, strlen(buffer), 0);
        return;
    }*/

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