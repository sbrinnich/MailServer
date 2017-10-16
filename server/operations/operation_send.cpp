#include <iostream>
#include <fstream>
#include <sys/time.h>

#include "operation_send.h"
#include "../socket_read.h"

OperationSend::OperationSend(int clientsocket) : Operation(clientsocket, mailspooldir) {

}

int OperationSend::parseRequest() {

    int ret = 0;
    send(clientsocket, "You chose to send an email. Please enter the following data.\nSender: ", strlen("You choose to send an email. Please enter the following data.\nSender: "), 0);
    ret = getClientInput(9,"Sender");
    if(ret == 1){
        return 1;
    }
    send(clientsocket, "Receiver: ", strlen("Receiver:"), 0);
    ret = getClientInput(9,"Receiver");
    if(ret == 1){
        return 1;
    }
    send(clientsocket, "Subject: ", strlen("Subject:"), 0);
    ret = getClientInput(81,"Subject");
    if(ret == 1){
        return 1;
    }
    send(clientsocket, "Content: ", strlen("Content:"), 0);
    ret = getClientInput(MAXLINE,"Content");
    if(ret == 1){
        return 1;
    }
    return 0;
}

int OperationSend::doOperation() {

    std::ofstream file;

    struct timeval tp;
    gettimeofday(&tp, NULL);
    long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    std::string filename = std::to_string(ms);

    std::stringstream dirpath;
    dirpath << mailspooldir << "/" << Data["Receiver"] << "/" << filename;

    printf("Writing to file %s", dirpath.str().c_str());

    file.open (dirpath.str().c_str());
    file << "Sender: " << Data["Sender"];
    file << "Receiver: " << Data["Receiver"];
    file << "Subject: " << Data["Subject"];
    file << "Content: " << Data["Content"];

    file.close();
    return 0;
}

int OperationSend::getClientInput(int buffersize, std::string key){
    char buffer[buffersize];
    std::fill(buffer, buffer + sizeof(buffer), 0);
    ssize_t size = recv(clientsocket, buffer, MAXLINE-1, 0);
    if(size > buffersize){
        send(clientsocket, "ERR\n", strlen("ERR\n"), 0);
        return 1;
    }
    Data [key] = buffer;
    return 0;
}