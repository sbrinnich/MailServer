#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <dirent.h>

#include "operation_send.h"
#include "../socket_read.h"

OperationSend::OperationSend(int clientsocket, char* mailspooldir) : Operation(clientsocket, mailspooldir) {
    sender = new char[9];
    receiver = new char[9];
    subject = new char[81];
    content = new char[MAXLINE];
}

int OperationSend::parseRequest() {

    int ret = 0;
    send(clientsocket, "You chose to send an email. Please enter the following data.\nSender: ", strlen("You choose to send an email. Please enter the following data.\nSender: "), 0);
    ret = getClientInput(9, &sender);
    if(ret == 1){
        return 1;
    }
    send(clientsocket, "Receiver: ", strlen("Receiver:"), 0);
    ret = getClientInput(9, &receiver);
    if(ret == 1){
        return 1;
    }
    send(clientsocket, "Subject: ", strlen("Subject:"), 0);
    ret = getClientInput(81, &subject);
    if(ret == 1){
        return 1;
    }
    send(clientsocket, "Content: ", strlen("Content:"), 0);
    ret = getClientInput(MAXLINE, &content);
    if(ret == 1){
        return 1;
    }
    return 0;
}

int OperationSend::doOperation() {

    // Create filename
    struct timeval tp;
    gettimeofday(&tp, NULL);
    long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
    std::string filename = std::to_string(ms);

    std::stringstream dirpath;

    // Check if directory exists
    dirpath << mailspooldir << "/" << receiver;
    DIR* dir = opendir(dirpath.str().c_str());
    if(!dir){
        // Create directory if it doesn't exist yet
        mkdir(dirpath.str().c_str(), 0777);
    }
    closedir(dir);

    // Add filename to path
    dirpath << "/" << filename << ".txt";

    // Open file and write data into it
    std::ofstream file;
    file.open(dirpath.str().c_str(), std::ios::out);
    file << "Sender: " << sender << std::endl;
    file << "Subject: " << subject << std::endl;
    file << "Content: " << content << std::endl;

    file.close();

    return 0;
}

int OperationSend::getClientInput(int buffersize, char* *ptr){
    char buffer[buffersize];
    std::fill(buffer, buffer + sizeof(buffer), 0);
    // Receive text from client, write to buffer
    ssize_t size = recv(clientsocket, buffer, MAXLINE-1, 0);

    if(size > buffersize){
        send(clientsocket, "ERR\n", strlen("ERR\n"), 0);
        return 1;
    }

    // Append null-terminator to buffer
    if(size >= buffersize) {
        buffer[buffersize-1] = '\0';
    }else if(buffer[size-1] == '\n'){
        buffer[size-1] = '\0';
    }else{
        buffer[size] = '\0';
    }

    // Copy text in buffer to ptr
    strcpy(*ptr, buffer);
    return 0;
}