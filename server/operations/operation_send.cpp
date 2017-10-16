#include <iostream>
#include <fstream>
#include <sys/time.h>

#include "operation_send.h"
#include "../socket_read.h"

OperationSend::OperationSend(int clientsocket, char* mailspooldir) : Operation(clientsocket, mailspooldir) {
}

int OperationSend::parseRequest() {

    int ret = 0;
    send(clientsocket, "You chose to send an email. Please enter the following data.\nSender: ", strlen("You choose to send an email. Please enter the following data.\nSender: "), 0);
    ret = getClientInput(9, Operation::SENDER);
    if(ret == 1){
        return 1;
    }
    send(clientsocket, "Receiver: ", strlen("Receiver:"), 0);
    ret = getClientInput(9, Operation::RECEIVER);
    if(ret == 1){
        return 1;
    }
    send(clientsocket, "Subject: ", strlen("Subject:"), 0);
    ret = getClientInput(81, Operation::SUBJECT);
    if(ret == 1){
        return 1;
    }
    send(clientsocket, "Content: ", strlen("Content:"), 0);
    ret = getClientInput(MAXLINE, Operation::CONTENT);
    if(ret == 1){
        return 1;
    }
    return 0;
}

int OperationSend::doOperation() {

    struct timeval tp;
    gettimeofday(&tp, NULL);
    long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    std::string filename = std::to_string(ms);

    std::stringstream dirpath;
    dirpath << mailspooldir << "/" << Data[Operation::RECEIVER] << "/" << filename << ".txt";

    std::ofstream file;
    file.open(dirpath.str().c_str(), std::ios::out);
    file << "Sender: " << Data[Operation::SENDER];
    file << "Receiver: " << Data[Operation::RECEIVER];
    file << "Subject: " << Data[Operation::SUBJECT];
    file << "Content: " << Data[Operation::CONTENT];

    file.close();

    return 0;
}

int OperationSend::getClientInput(int buffersize, int key){
    char buffer[buffersize];
    std::fill(buffer, buffer + sizeof(buffer), 0);
    ssize_t size = recv(clientsocket, buffer, MAXLINE-1, 0);
    if(size > buffersize){
        send(clientsocket, "ERR\n", strlen("ERR\n"), 0);
        return 1;
    }
    buffer[size] = '\0';
    Data[key] = buffer;
    return 0;
}