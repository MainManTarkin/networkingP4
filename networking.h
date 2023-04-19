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

class bestNetworkingClassEver{

    public:
    //were going to open a socket
    //throws if an error pops up
    bestNetworkingClassEver(std::string portValInput, std::string addressValInput, std::string userNameInput);

    ~bestNetworkingClassEver();

    int sendMessage(std::string messageInput);

    bool checkForRecv();

    //returns a message appended with a newline
    std::string getMessage();

    private:

    //fucntions

    void prepareMessage(std::string messageToSend);

    //variables
    int clientSocket = -1;



};