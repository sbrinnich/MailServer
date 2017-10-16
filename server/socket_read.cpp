#include <cstdio>
#include <bits/stl_algobase.h>
#include "socket_read.h"

static ssize_t my_read (int fd, char *ptr) {
    static int   read_cnt = 0 ;
    static char  *read_ptr = nullptr;
    static char  read_buf[MAXLINE] ;
    std::fill(read_buf, read_buf + sizeof(read_buf), 0);
    if (read_cnt <= 0) {
        again:
        if ( (read_cnt = read(fd,read_buf,sizeof(read_buf))) < 0) {
            if (errno == EINTR)
                goto again ;
            return (-1) ;
        } else if (read_cnt == 0)
            return (0) ;
        read_ptr = read_buf ;
    } ;
    read_cnt-- ;
    *ptr = *read_ptr++ ;
    return (1) ;
}

/**
 * Reads from a clientsocket until next newline and writes read line into a buffer
 * @param fd the socket from the client
 * @param vptr a char* buffer
 * @param maxlen the maximum number of characters in one line
 * @return number of bytes read or -1 in case of an error
 */
ssize_t readline (int fd, char *vptr, size_t maxlen) {
    ssize_t   n, rc ;
    char      c = '\0', *ptr ;
    ptr = vptr ;
    for (n = 1 ; n < maxlen ; n++) {
        if ( (rc = my_read(fd,&c)) == 1 ) {
            *ptr++ = c ;
            if (c == '\n')
                break ;                // newline is stored
        } else if (rc == 0) {
            if (n == 1)
                return (0) ;         // EOF, no data read
            else
                break ;              // EOF, some data was read
        } else
            return (-1) ;          // error, errno set by read() in my_read()
    } ;
    *ptr = 0 ;                   // null terminate
    return (n) ;
}