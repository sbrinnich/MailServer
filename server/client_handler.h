#ifndef MAILSERVER_CLIENT_HANDLER_H
#define MAILSERVER_CLIENT_HANDLER_H

#include "operation.h"
#include "operations/operation_send.h"
#include "operations/operation_list.h"
#include "operations/operation_read.h"
#include "operations/operation_del.h"

class ClientHandler {
private:
    /**
     * Char pointer containing the directory path to the mailspool
     */
    char* mailspooldir;
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
     * @return 0 if client request was handled successfully or
     *          1 if something went wrong or
     *          -1 if client closed socket
     */
    int handleClientRequest(int clientsocket, char* request_msg);
public:
    /**
     * Creates an instance of a client handler class
     * @param mailspooldir a path to the mails pool directory
     */
    ClientHandler(char* mailspooldir);
    /**
     * Handles a client's requests
     * @param clientsocket the socket from the client
     */
    void handleClient(int clientsocket);
};

#endif //MAILSERVER_CLIENT_HANDLER_H
