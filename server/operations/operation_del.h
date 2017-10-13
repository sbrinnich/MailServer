#ifndef MAILSERVER_OPERATION_DEL_H
#define MAILSERVER_OPERATION_DEL_H

#include "../operation.h"

class OperationDel : public Operation {
public:
    OperationDel(int clientsocket);
    ~OperationDel();
    int parseRequest() override;
    int doOperation() override;
};

#endif //MAILSERVER_OPERATION_DEL_H
