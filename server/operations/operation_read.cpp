#include "operation_read.h"

OperationRead::OperationRead(int clientsocket) : Operation(clientsocket) {
}

int OperationRead::parseRequest() {
    return 0;
}

int OperationRead::doOperation() {
    printf("Doing Read\n");
    return 0;
}