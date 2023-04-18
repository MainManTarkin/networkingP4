#include <iostream>
#include <curses.h>
#include <thread>
#include <map>
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

//defines
const int closedSocketVal = -1;

//functions here


//inline int initClientSocket(std::string portValInput, std::string addressValInput);

class bestNetworkingClassEver{

    public:
    //were going to open a socket
    //throws if an error pops up
    bestNetworkingClassEver(std::string portValInput, std::string addressValInput, std::string userNameInput);

    //sends disconnection message to server and closes socket
    ~bestNetworkingClassEver();

    // send a message to the server
    int sendMessage(std::string messageInput);

    // check if there is a message to be received
    bool checkForRecv();

    // get a new message from the server
    std::string getMessage();

    private:

    int clientSocket = -1;



};
