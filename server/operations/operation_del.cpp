#include "operation_del.h"

OperationDel::OperationDel(int clientsocket) : Operation(clientsocket, mailspooldir) {
}

int OperationDel::parseRequest() {
    return 0;
}

int OperationDel::doOperation() {
    printf("Doing Del\n");
    return 0;
}