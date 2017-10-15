#include "operation_send.h"
#include "../socket_read.h"

OperationSend::OperationSend(int clientsocket) : Operation(clientsocket, mailspooldir) {

}

int OperationSend::parseRequest() {

    send(clientsocket, "You chose to send an email. Please enter the following data.\n", strlen("You choose to send an email. Please enter the following data.\n"), 0);
    send(clientsocket, "Sender: ", strlen("Sender:"), 0);
    getClientInput(8,"Sender");
    send(clientsocket, "Receiver: ", strlen("Receiver:"), 0);
    getClientInput(8,"Receiver");
    send(clientsocket, "Subject: ", strlen("Subject:"), 0);
    getClientInput(80,"Subject");
    send(clientsocket, "Content: ", strlen("Content:"), 0);
    getClientInput(MAXLINE,"Content");

    return 0;
}

int OperationSend::doOperation() {

    return 0;
}

int OperationSend::getClientInput(int buffersize, std::string key){
    char buffer[buffersize];
    std::fill(buffer, buffer + sizeof(buffer), 0);
    ssize_t size = readline(clientsocket, buffer, MAXLINE);
    if(size > buffersize){
        send(clientsocket, "ERR\n", strlen("ERR\n"), 0);
        return -1;
    }
    Data [key] = buffer;
    return 0;
}