#ifndef MAILSERVER_CLIENT_HANDLER_H
#define MAILSERVER_CLIENT_HANDLER_H

#include "operation.h"

class ClientHandler {
private:
    /**
     * Searches for the correct implementation of the operation for the command in buffer
     * @param buffer a char* containing a command
     * @param clientsocket a socket from the client sending the command
     * @return a new instance of the found operation or nullptr, if no fitting operation could be found
     */
    Operation* getOperation(char* buffer, int clientsocket);

    /**
     * Handles a request of a client
     * @param clientsocket the socket from the client
     * @param request_msg the message that the client entered to start an operation
     */
    void handleClientRequest(int clientsocket, char* request_msg);
public:
    /**
     * Handles a client's requests
     * @param clientsocket the socket from the client
     */
    void handleClient(int clientsocket);
};

#endif //MAILSERVER_CLIENT_HANDLER_H
