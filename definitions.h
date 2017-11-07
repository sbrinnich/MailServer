#ifndef MAILSERVER_DEFINITIONS_H
#define MAILSERVER_DEFINITIONS_H

// Maximum number of characters for one line
#define MAXLINE 1024
// Maximum number of characters for a single mail's content
#define MAXMSG 16384

// Minutes until blocked clients are able to connect again
#define IP_BLOCK_MINUTES 2

// Maximum number of clients to connect to server
#define MAX_CLIENTS 5

// Timeout seconds for listening
#define TIMEOUT_SECONDS 5

// Name of blacklist file
#define BLACKLIST_FILENAME "blacklist.txt"

#endif //MAILSERVER_DEFINITIONS_H
