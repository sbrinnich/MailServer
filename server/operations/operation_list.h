#ifndef MAILSERVER_OPERATION_LIST_H
#define MAILSERVER_OPERATION_LIST_H

#include "../operation.h"

class OperationList : public Operation {
public:
    OperationList(int clientsocket);
    ~OperationList();
    int parseRequest() override;
    int doOperation() override;
};

#endif //MAILSERVER_OPERATION_LIST_H
