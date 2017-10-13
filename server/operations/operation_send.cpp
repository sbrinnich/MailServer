#include "operation_send.h"

OperationSend::OperationSend(int clientsocket) : Operation(clientsocket) {
}

int OperationSend::parseRequest() {
    return 0;
}

int OperationSend::doOperation() {
    printf("Doing send");
    return 0;
}