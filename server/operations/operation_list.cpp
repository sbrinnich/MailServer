#include "operation_list.h"

OperationList::OperationList(int clientsocket) : Operation(clientsocket, mailspooldir) {
}

int OperationList::parseRequest() {
    return 0;
}

int OperationList::doOperation() {
    printf("Doing List\n");
    return 0;
}