#ifndef MAILSERVER_CLIENT_HANDLER_H
#define MAILSERVER_CLIENT_HANDLER_H

#define IP_BLOCK_MINUTES 2

class Operation;

class ClientHandler {
private:
    /**
     * Char pointer containing the directory path to the mailspool
     */
    char* mailspooldir;

    /**
     * Char pointer containing the username of the user currently logged in (or nullptr, if user is not logged in)
     */
    char* username;

    /**
     * Integer containing the number of failed logins
     */
    int failedLogins;

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

    /**
     * Sets the username of currently logged in user to given char pointer
     * @param username a char pointer containing a username of max 8 characters
     */
    void setUsername(char* username);

    /**
     * Returns the username of currently logged in user
     * @return a char pointer of max 8 characters length containing the username
     */
    char* getUsername();

    /**
     * Increments failedLogins
     */
    void incrementFailedLogins();

    /**
     * Returns the number of failed logins
     * @return an int containing the number of failed logins
     */
    int getFailedLogins();

    /**
     * Goes through the list of blocked IPs and deletes those, which are allowed to connect again.
     * Also checks if currently connected client IP is allowed
     * @return 0 if currently connected client IP is allowed or
     *          1 if currently connected client IP is not allowed to connect (is on blacklist)
     */
    int checkBlockedIPs();

    /**
     * Adds current client's IP address to a blacklist to stop him from connecting again in a while
     */
    void blockClientIP();
};

#endif //MAILSERVER_CLIENT_HANDLER_H
