#ifndef MAILSERVER_OPERATION_LIST_H
#define MAILSERVER_OPERATION_LIST_H

#include "../operation.h"

class OperationList : public Operation {
private:

public:
    OperationList(int clientsocket, char* mailspooldir, ClientHandler* clientHandler);
    ~OperationList();
    int doPreparation() override;
    int parseRequest() override;
    int doOperation() override;
};

#endif //MAILSERVER_OPERATION_LIST_H
