#include "operation_send.h"

OperationSend::OperationSend(int clientsocket) : Operation(clientsocket, mailspooldir) {

}

int OperationSend::parseRequest() {

    send(clientsocket, "You choose to send an email. Please enter the following data.\n", strlen("You choose to send an email. Please enter the following data.\n"), 0);
    send(clientsocket, "Sender: ", strlen("Sender:"), 0);
    char * sender = getClientInput(8);
    Data ["Sender"] = sender;

    return 0;
}

int OperationSend::doOperation() {

    return 0;
}

char * OperationSend::getClientInput(int buffersize){
    char buffer[buffersize];
    std::fill(buffer, buffer + sizeof(buffer), 0);
    ssize_t size = readline(clientsocket, buffer, MAXLINE);
    return buffer;
}