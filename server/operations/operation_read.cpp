#include "operation_read.h"

OperationRead::OperationRead(int clientsocket, char* mailspooldir, ClientHandler* clientHandler) :
        Operation(clientsocket, mailspooldir, clientHandler) {
}

int OperationRead::doPreparation() {
    if(clientHandler->getUsername() == nullptr){
        send(clientsocket, "Please login to use this command!\n", strlen("Please login to use this command!\n"), 0);
        return 1;
    }
    return 0;
}

int OperationRead::parseRequest() {
    int ret = 0;

    char *nr;
    nr = new char[5];
    send(clientsocket, "You chose to read an email. Please enter the following data.\nMessage Number: ",
         strlen("You chose to read an email. Please enter the following data.\nMessage Number: "), 0);
    ret = getClientInput(5, &nr);
    if(ret == 1 || ret == -1){
        delete[] nr;
        return ret;
    }

    std::stringstream s(nr);
    s >> messagenr;
    delete[] nr;
    if(s.fail()){
        return 1;
    }
    return 0;
}

int OperationRead::doOperation() {
    if(messagenr <= 0){
        return 1;
    }

    // Get filepath
    std::stringstream filepath;
    filepath << mailspooldir << "/" << clientHandler->getUsername();
    char* filename = getNthMailFilename(filepath.str().c_str(), messagenr);
    if(filename == nullptr){
        return 1;
    }
    filepath << "/" << filename;

    // Parse file
    char *sender, *subject, *content;
    sender = new char[9];
    subject = new char[81];
    content = new char[MAXMSG];
    int parsing = parseMailFile(filepath.str().c_str(), &sender, &subject, &content);

    // Check if parsing was successful
    if(parsing != 0){
        return 1;
    }

    // Print email (send to socket)
    std::stringstream out;
    out << "Sender: " << sender << "\nReceiver: " << clientHandler->getUsername() <<
        "\nSubject: " << subject << "\nContent: " << content << "\n";


    //handle attachment if exists
    std::string filenamestr(filepath.str().c_str());
    //get rid of .txt
    long extension = filenamestr.find(".txt");
    if(extension != std::string::npos){
        filenamestr.resize(extension);
    }
    filenamestr += "_attachment";
    std::ifstream infile(filenamestr);
    bool ret = infile.good();
    if(ret){
        auto *temp = new char[8];
        out << "This file has an attachment, do you want to download it(y/n)?\n";
        send(clientsocket, out.str().c_str(), strlen(out.str().c_str()), 0);
        int ret = getClientInput(MAXLINE, &temp);
        if(ret == 1 || ret == -1){
            delete[]temp;
            return ret;
        }
        if(strcmp(temp,"y") == 0){
            send(clientsocket, "Downloading file...\n", strlen("Downloading file...\n"), 0);
            int ret = getClientInput(MAXLINE, &temp);
            if(ret == 1 || ret == -1){
                delete[]temp;
                return ret;
            }
            //get size of file
            struct stat st{};
            stat(filenamestr.c_str(), &st);
            long filesize = st.st_size;                 //long fileSize = getFileSize(file);
            auto * const filesizechar = reinterpret_cast<char * const>(&filesize);//send as char array so that get client input works
            send(clientsocket, filesizechar, strlen(filesizechar), 0);

            //open file
            FILE *file;
            // Open the file in binary mode using the "rb" format string
            // This also checks if the file exists and/or can be opened for reading correctly
            if ((file = fopen(filenamestr.c_str(), "rb")) == nullptr){
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
                    ssize_t Sent = send(clientsocket, CopyHelper, Read, 0);
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
        delete[]temp;

    }
    send(clientsocket, out.str().c_str(), strlen(out.str().c_str()), 0);

    // Delete allocations
    delete[] sender;
    delete[] subject;
    delete[] content;
    return 0;
}

OperationRead::~OperationRead() {
}