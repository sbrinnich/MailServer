#ifndef MAILSERVER_SERVER_H
#define MAILSERVER_SERVER_H

// Minutes until blocked clients are able to connect again
#define IP_BLOCK_MINUTES 2

#include <cstdio>
#include <cstring>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <mutex>

#include <dirent.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <thread>
#include <pthread.h>

#include "client_handler.h"

class Server {
private:
    char *mailspooldir;
    int port;
    int server_socket;
    bool listening;

    // Contains all currently blocked clients with ip address and expiration time
    std::unordered_map<std::string, time_t> blocked_clients;

    // Mutexes for 'blacklist.txt' file and blacklist-map (blocked_clients)
    std::mutex blacklist_map_mutex;
    std::mutex blacklist_file_mutex;

    /**
     * Reads all blocked clients (ip address and block expiration time) from a 'blacklist.txt' file inside the
     * Mailspool-Directory and writes them into the blacklist-map defined in this class
     */
    void readBlockedClients();
    /**
     * Writes the blocked ip addresses defined via the blacklist-map in this class to a 'blacklist.txt' file
     * inside the Mailspool-Directory
     */
    void writeBlockedClients();
    /**
     * Checks the blacklist-map defined in this class and removes every ip address inside where the blocking time
     * has expired already.
     */
    void checkBlockedClients();
    /**
     * Returns the ip address which is bound to a given socket
     * @param socket the socket from which the ip address should be read
     * @return a string containing the ip address of the socket
     */
    std::string getIpFromSocket(int socket);
    /**
     * Handles the connection of a new client.
     * Checks if the client is allowed to connect to the server (if the ip address is currently not on the blacklist)
     * and creates a new ClientHandler instance for handling the client's requests if he is allowed to connect
     * @param clientSocket the socket descriptor of the new client
     */
    void clientConnect(int clientSocket);
    /**
     * Initiates the blacklist (Should be called when starting the server)
     */
    void initBlacklist();

public:
    /**
     * Creates a new instance of a Server
     * @param mailspooldir the path to the directory where all the mails should be saved to
     * @param port the port on which the server should listen for clients
     */
    Server(char *mailspooldir, int port);
    /**
     * Checks if the port defined in this class is a valid port (is greater than 0)
     * @return 1 if the port is not valid or
     *          0 if the port is valid
     */
    int checkPort();
    /**
     * Checks if the Mailspool-Directory defined in this class is a valid path and existing directory.
     * If the directory does not exist, the user is asked via the console if an attempt should be made to create the
     * given directory.
     * @return 1 if the directory does not exist and could not be created or
     *          0 either if the directory exists or it was created successfully
     */
    int checkDir();
    /**
     * Binds the server socket to the port defined in this class
     * @return 1 if the socket could not be bound or
     *          0 if socket was bound successfully
     */
    int bindSocket();
    /**
     * Listens in a loop for up to 5 clients connecting to this server and calls clientConnect in a new thread
     * for each client connecting
     */
    void listenForClients();
    /**
     * Adds current client's IP address to a blacklist to stop him from connecting again in a while
     */
    void blockClient(int clientSocket);
    /**
     * Starts the server. Calls the methods from this class in following order:
     * - checkPort
     * - checkDir
     * - bindSocket
     * - initBlacklist
     * - listenForClients
     *
     * After initiating the server start, this method waits for input in the server console.
     * Following commands are handled:
     * - write (writes the currently available blacklist of this server to the blacklist-file)
     * - quit (stops the server from listening to any more clients and returns from this method)
     * @return either 1 if something went wrong in one of the called methods or
     *          0 if server is closed properly with the 'quit' command
     */
    int startServer();
};

#endif //MAILSERVER_SERVER_H
