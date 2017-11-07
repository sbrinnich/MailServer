#ifndef MAILSERVER_OPERATION_DEL_H
#define MAILSERVER_OPERATION_DEL_H

#include "../operation.h"

class OperationDel : public Operation {

private:
    int messagenr;

public:
    /**
     * Creates a new instance of a del-operation
     * @param clientsocket the socket of the client requesting the operation
     * @param mailspooldir the directory where all the mails are saved to
     * @param clientHandler an instance to the ClientHandler handling this client's requests
     */
    OperationDel(int clientsocket, char* mailspooldir, ClientHandler* clientHandler);
    ~OperationDel() override = default;
    int doPreparation() override;
    int parseRequest() override;
    int doOperation() override;
};

#endif //MAILSERVER_OPERATION_DEL_H
