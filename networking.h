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

    ~bestNetworkingClassEver();

    int sendMessage(std::string messageInput);

    bool checkForRecv();

    std::string getMessage();

    private:

    int clientSocket = -1;



};