#ifndef MAILSERVER_SOCKET_READ_H
#define MAILSERVER_SOCKET_READ_H

#include <unistd.h>
#include <cerrno>

#define MAXLINE 1024

/**
 * Helper function for readline
 */
static ssize_t my_read (int fd, char *ptr);

/**
 * Reads from a clientsocket until next newline and writes read line into a buffer
 * @param fd the socket from the client
 * @param vptr a char* buffer
 * @param maxlen the maximum number of characters in one line
 * @return number of bytes read or -1 in case of an error
 */
ssize_t readline (int fd, char *vptr, size_t maxlen);

#endif //MAILSERVER_SOCKET_READ_H
