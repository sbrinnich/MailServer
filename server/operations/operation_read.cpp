#include "operation_read.h"

OperationRead::OperationRead(int clientsocket, char* mailspooldir) : Operation(clientsocket, mailspooldir) {
}

int OperationRead::parseRequest() {
    return 0;
}

int OperationRead::doOperation() {
    printf("Doing Read\n");
    return 0;
}