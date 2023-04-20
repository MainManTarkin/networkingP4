#pragma once
#include <iostream>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <memory.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdexcept>

// defines
const size_t recvSocketBufferSize = 500;
const time_t timeOutSec = 10;
const time_t timeOutUSec = 0;
const int closedSocketVal = -1;

// functions here

class RCnetworking
{

public:
    // were going to open a socket
    // throws if an error pops up
    RCnetworking(std::string portInput, std::string addressInput, std::string userNameInput);

    // sends disconnection message to server and closes socket
    ~RCnetworking();

    // send a message to the server
    int sendMessage(std::string messageInput);

    // check if there is a message to be received
    bool checkForRecv();

    // returns a message appended with a newline
    std::string getMessage();

private:
    // fucntions

    void prepareMessage(std::string *messageToSend, char command);

    // variables
    int clientSocket = -1;

    //buffer and its size variable
    char *bufferRecv;

    size_t bufferSize = recvSocketBufferSize;

    //adderinfo and it list used placed here for constructer and deconstructer use
    struct addrinfo basedInfo;
    struct addrinfo *serverAddrInfo = nullptr;
    struct addrinfo *addList = nullptr;

    //used by the deconstructer to determine what it has to free
    bool bufferAllocated = false;
    bool socketCreated = false;
    bool getAdderListCreated = false;
};
