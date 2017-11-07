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

// Client commands
#define OPERATION_LOGIN "login"
#define OPERATION_SEND "send"
#define OPERATION_LIST "list"
#define OPERATION_READ "read"
#define OPERATION_DELETE "del"
#define OPERATION_QUIT "quit"

// Text to display at client before an operation
#define INFO_TEXT "Please enter a command:\n- "<<\
OPERATION_LOGIN<<"\tperform login operation\n- "<<\
OPERATION_SEND<<"\tsend a mail\n- "<<\
OPERATION_LIST<<"\tlist all your received mails\n- "<<\
OPERATION_READ<<"\tread a mail\n- "<<\
OPERATION_DELETE<<"\tdelete a mail\n- "<<\
OPERATION_QUIT<<"\texit program\n"

// Server commands
#define SVR_COMMAND_WRITE "write"
#define SVR_COMMAND_QUIT "quit"

#endif //MAILSERVER_DEFINITIONS_H
