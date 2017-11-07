#ifndef MAILSERVER_OPERATION_LIST_H
#define MAILSERVER_OPERATION_LIST_H

#include "../operation.h"

class OperationList : public Operation {
public:
    /**
     * Creates a new instance of a list-operation
     * @param clientsocket the socket of the client requesting the operation
     * @param mailspooldir the directory where all the mails are saved to
     * @param clientHandler an instance to the ClientHandler handling this client's requests
     */
    OperationList(int clientsocket, char* mailspooldir, ClientHandler* clientHandler);
    ~OperationList();
    int doPreparation() override;
    int parseRequest() override;
    int doOperation() override;
};

#endif //MAILSERVER_OPERATION_LIST_H
