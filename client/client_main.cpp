#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <termios.h>
#include <fstream>
#include <iostream>

#include "../definitions.h"

void showInput(bool show);
void sendAttachment(int clientSocket);
void recvAttachment(int clientSocket);

int main (int argc, char **argv) {
    int clientSocket;
    char buffer[MAXMSG];
    std::fill(buffer, buffer + sizeof(buffer), 0);
    struct sockaddr_in address;
    int size;

    // Check if all parameters are specified
    if( argc != 3 ){
        printf("Invalid parameters specified!\n");
        printf("Usage: %s [server_address] [port]\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Check given port
    int port;
    std::stringstream s(argv[2]);
    s >> port;
    if(s.fail() || port <= 0){
        printf("Invalid parameters specified! Not a valid port number!\n");
        printf("Usage: %s [server_address] [port]\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Create socket
    if ((clientSocket = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket error");
        return EXIT_FAILURE;
    }

    // Define address and port of remote server
    memset(&address,0,sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    inet_aton (argv[1], &address.sin_addr);

    // Try to connect to remote server
    if (connect(clientSocket, (struct sockaddr *) &address, sizeof (address)) == 0) {
        printf ("Connection with server (%s) established\n", inet_ntoa (address.sin_addr));
        std::fill(buffer, buffer + sizeof(buffer), 0);
    }else{
        perror("Connect error - no server available");
        return EXIT_FAILURE;
    }

    // Quit command
    std::stringstream quit;
    quit << OPERATION_QUIT << "\n";

    // Loop send and receive
    do {
        // Clear buffer
        std::fill(buffer, buffer + sizeof(buffer), 0);

        // Receive message from server
        size = recv(clientSocket,buffer,MAXMSG-1, 0);
        if (size>0)
        {
            buffer[size]= '\0';
            printf("%s",buffer);
        }

        if(strstr(buffer, "Which file") != nullptr){
            // Clear buffer
            std::fill(buffer, buffer + sizeof(buffer), 0);
            sendAttachment(clientSocket);
        }else if(strstr(buffer, "Downloading file") != nullptr){
            // Clear buffer
            std::fill(buffer, buffer + sizeof(buffer), 0);
            recvAttachment(clientSocket);
        }else{
            if(strstr(buffer, "Found ") != nullptr || strstr(buffer, "Reading Mail") != nullptr){
                // On command list or read, read again to receive next message
                send(clientSocket, "OK", strlen("OK"), 0);
            }else {
                bool pwd = (strstr(buffer, "Password") != nullptr);
                if (pwd) {
                    // Hide password input
                    showInput(false);
                }
                // Clear buffer
                std::fill(buffer, buffer + sizeof(buffer), 0);

                // Read message from stdin and send to server
                char *fgetret = std::fgets(buffer, MAXMSG, stdin);
                if (fgetret != nullptr) {
                    send(clientSocket, buffer, strlen(buffer), 0);
                }
                if (pwd) {
                    // Show input again
                    showInput(true);
                    printf("\n");
                }
            }
        }
    }while (strcasecmp(buffer, quit.str().c_str()) != 0);

    // Close connection
    printf ("Connection to server closed.\n");
    close (clientSocket);

    return EXIT_SUCCESS;
}

void showInput(bool show){
    termios stty;
    tcgetattr(STDIN_FILENO, &stty);
    if(show){
        stty.c_lflag |= ECHO;
    }else{
        stty.c_lflag &= ~ECHO;
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &stty);
}

void sendAttachment(int clientSocket){
    // Read filename
    auto buffer = new char[MAXMSG];
    char *fgetret = std::fgets(buffer, MAXMSG, stdin);
    if (fgetret != nullptr) {
        auto filename = new char[MAXMSG];
        strcpy(filename, buffer);
        std::string filename_str = filename;
        filename_str.erase(filename_str.length()-1);

        // Check if file can be opened
        std::ifstream file;
        file.open(filename_str.c_str(), std::ios::in | std::ios::binary);
        if(!file.is_open()){
            perror("Could not open specified file\n");
            send(clientSocket, "Error", strlen("Error"), 0);
            delete[] buffer;
            delete[] filename;
            return;
        }

        // Get size of file
        std::streampos fsize = 0;
        std::ifstream fileforsize;
        fileforsize.open(filename_str.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
        fsize = fileforsize.tellg();
        fileforsize.close();
        std::stringstream filesizestream;
        filesizestream << fsize;

        // Remove path from input
        std::size_t found = filename_str.find_last_of('/');
        filename_str = filename_str.substr(found+1);

        // Send filename
        send(clientSocket, filename_str.c_str(), strlen(filename_str.c_str()), 0);
        // Receive OK from server
        std::fill(buffer, buffer + sizeof(buffer), 0);
        recv(clientSocket, buffer, MAXMSG, 0);

        // Send size as char array
        send(clientSocket, filesizestream.str().c_str(), strlen(filesizestream.str().c_str()), 0);

        // Read file and send to server
        long read_size = 0;
        if(fsize <= 0){
            file.close();
            delete[] buffer;
            delete[] filename;
            return;
        }
        do {
            // Receive OK from server
            std::fill(buffer, buffer + sizeof(buffer), 0);
            recv(clientSocket, buffer, MAXMSG, 0);
            if(fsize-read_size > MAXMSG-1){
                file.read(buffer, MAXMSG-1);
                send(clientSocket, buffer, MAXMSG-1, 0);
                read_size += MAXMSG-1;
            }else{
                file.read(buffer, fsize-read_size);
                send(clientSocket, buffer, fsize-read_size, 0);
                break;
            }
        }while(true);

        // Close file
        file.close();

        delete[] filename;
    }
    delete[] buffer;
}

void recvAttachment(int clientSocket){
    // Send OK to server
    send(clientSocket, "OK", strlen("OK"), 0);

    auto buffer = new char[MAXMSG];
    auto filename = new char[MAXMSG];
    std::fill(buffer, buffer + sizeof(buffer), 0);
    std::fill(filename, filename + sizeof(filename), 0);

    // Receive filename from server
    size_t size = recv(clientSocket, filename, MAXMSG, 0);
    if(size>0){
        filename[size]= '\0';
    }else{
        perror("Could not receive filename of attachment from server");
        delete[] buffer;
        delete[] filename;
        return;
    }

    // Send OK to server
    send(clientSocket, "OK", strlen("OK"), 0);

    // Receive filesize from server
    size = recv(clientSocket, buffer, MAXMSG, 0);
    if(size>0){
        buffer[size]= '\0';
    }else{
        perror("Could not receive filesize of attachment from server");
        delete[] buffer;
        delete[] filename;
        return;
    }

    // Parse filesize
    long filesize = 0;
    std::stringstream sizeStream(buffer);
    sizeStream >> filesize;
    if(sizeStream.fail()){
        perror("Could not parse attachment filesize");
        delete[] buffer;
        delete[] filename;
        return;
    }

    // Get full filepath
    std::stringstream filepath;
    filepath << "attachment_" << filename;

    // Open file for writing
    std::ofstream attachFile;
    attachFile.open(filepath.str().c_str(), std::ios::binary);
    if(attachFile.is_open()){
        // Receive from server and write to file
        auto readBuffer = new char[MAXMSG];
        long read_size = 0;
        do{
            // Send OK to server
            send(clientSocket, "OK", strlen("OK"), 0);
            std::fill(readBuffer, readBuffer + sizeof(readBuffer), 0);

            // Receive from server and write to file
            if(filesize-read_size > MAXMSG-1){
                recv(clientSocket, readBuffer, MAXMSG-1, 0);
                attachFile.write(readBuffer, MAXMSG-1);
                read_size += MAXMSG-1;
            }else{
                recv(clientSocket, readBuffer, filesize-read_size, 0);
                attachFile.write(readBuffer, filesize-read_size);
                break;
            }
        }while(true);

        attachFile.close();
        delete[] readBuffer;
    }else{
        perror("Could not write attachment file");
    }

    delete[] buffer;
    delete[] filename;
}