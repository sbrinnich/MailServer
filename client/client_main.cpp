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
#include <sys/stat.h>
#include "../server/operation.h"

#define BUF 16384

int main (int argc, char **argv) {
    int create_socket;
    char buffer[BUF];
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
    if ((create_socket = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Socket error");
        return EXIT_FAILURE;
    }

    // Define address and port of remote server
    memset(&address,0,sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons (port);
    inet_aton (argv[1], &address.sin_addr);

    // Try to connect to remote server
    if (connect ( create_socket, (struct sockaddr *) &address, sizeof (address)) == 0) {
        printf ("Connection with server (%s) established\n", inet_ntoa (address.sin_addr));
        std::fill(buffer, buffer + sizeof(buffer), 0);
    }else{
        perror("Connect error - no server available");
        return EXIT_FAILURE;
    }

    // Loop send and receive
    do {
        // Clear buffer
        std::fill(buffer, buffer + sizeof(buffer), 0);

        // Receive message from server
        size = recv(create_socket,buffer,BUF-1, 0);
        if (size>0)
        {
            buffer[size]= '\0';
            printf("%s",buffer);
        }

        if(strcmp(buffer, "Which file do you want to add? ") == 0){
            char *fgetret = std::fgets(buffer, BUF, stdin);
            if (fgetret != nullptr) {
                //send filename
                send(create_socket, buffer, strlen(buffer), 0);

                //get size of file
                struct stat st{};
                stat(buffer, &st);
                long filesize = st.st_size;                 //long fileSize = getFileSize(file);
                auto * const filesizechar = reinterpret_cast<char * const>(&filesize);//send as char array so that get client input works
                send(create_socket, filesizechar, strlen(filesizechar), 0);

                //open file
                FILE *file;
                // Open the file in binary mode using the "rb" format string
                // This also checks if the file exists and/or can be opened for reading correctly
                if ((file = fopen(buffer, "rb")) == nullptr){
                    perror("Could not open specified file\n");
                } else{
                    printf("File opened successfully\n");
                }

                // read file and send file to server
                long SizeCheck = 0;
                auto *CopyHelper = (char*)malloc(1024);
                if(filesize > 1024){
                    while(SizeCheck < filesize){
                        size_t Read = fread(CopyHelper, 1024, 1, file);//gibt anzahl speicherobjecte zurück
                        ssize_t Sent = send(create_socket, CopyHelper, Read, 0);
                        SizeCheck += Sent;
                        for(int i = 0; i < Sent; i++){
                            if(CopyHelper[i] == '\n'){
                                SizeCheck += 1;//because \n is 2 byte
                            }
                        }
                    }
                }
                fclose(file);
                free(CopyHelper);
            }
        }

        if(strcmp(buffer, "Downloading file...\n") == 0){
            //send
            send(create_socket, "Ok", strlen("Ok"), 0);
            //get filename
            auto * filename = new char [MAXLINE];
            recv(create_socket,filename,BUF-1, 0);

            send(create_socket, "Creating file...", strlen("Creating file..."), 0);

            //get filesize
            char * FileSizeChar = new char[MAXLINE];
            recv(create_socket,FileSizeChar,BUF-1, 0);
            char *end;
            auto FileSize = static_cast<size_t>(strtol(FileSizeChar,&end,10));

            FILE *file = fopen(filename, "w");//creates empty file to write into
            char* copyhelper;
            long SizeCheck = 0;
            copyhelper = (char*)malloc(FileSize + 1);
            while(SizeCheck < FileSize){
                ssize_t Received = recv(create_socket, copyhelper, FileSize, 0);
                ssize_t  Written = fwrite(copyhelper, sizeof(char), static_cast<size_t>(Received), file);
                SizeCheck += Written;
                for(int i = 0; i < Written; i++){
                    if(copyhelper[i] == '\n'){
                        SizeCheck += 1;//because \n is 2 byte
                    }
                }
            }
            fclose(file);
            free(copyhelper);
        }

        bool pwd = (strstr(buffer, "Password") != nullptr);
        if(pwd){
            // Hide password input
            termios stty;
            tcgetattr(STDIN_FILENO, &stty);
            stty.c_lflag &= ~ECHO;
            tcsetattr(STDIN_FILENO, TCSANOW, &stty);
        }
        // Clear buffer
        std::fill(buffer, buffer + sizeof(buffer), 0);

        // Read message from stdin and send to server
        char *fgetret = std::fgets(buffer, BUF, stdin);
        if (fgetret != nullptr) {
            send(create_socket, buffer, strlen(buffer), 0);
        }
        if(pwd){
            // Show input again
            termios stty;
            tcgetattr(STDIN_FILENO, &stty);
            stty.c_lflag |= ECHO;
            tcsetattr(STDIN_FILENO, TCSANOW, &stty);

            // Add newline
            printf("\n");
        }
    }while (strcasecmp (buffer, "quit\n") != 0);

    // Close connection
    printf ("Connection to server closed.\n");
    close (create_socket);

    return EXIT_SUCCESS;
}