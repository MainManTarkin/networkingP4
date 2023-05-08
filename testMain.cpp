
//file is for test the RCnetworking class
#include "networking.h"
//used for a delay
#include <chrono>
#include <thread>

#define goodReturn 0
#define defualtBadReturn 1

const char clientHelpText[] =
    "|---------------------------------------------------|\n"
    "|====================== Help =======================|\n"
    "|    -h - prints out program arg contents           |\n"
    "|    -p - defines port number other then defualt    |\n"
    "|         5077                                      |\n"
    "|    -s - overrides the default server IP address   |\n"
    "|         of 127.0.0.1                              |\n"
    "|    -n - overrides the default name of Unknown     |\n"
    "|---------------------------------------------------|\n";

std::string address = "localhost";
std::string portNumber = "5077";
std::string username = "Unknown";

inline int handleArgs(int argcInput, char *argvInput[]){

    int argHandlerRetVal = 0;
    int optRetVal = 0;

    while ((optRetVal = getopt(argcInput, argvInput, "hs:p:n:")) != -1)
    {

        switch (optRetVal)
        {
        case 'h':

            argHandlerRetVal = -1;
            std::cout << clientHelpText << std::endl;

            break;
        case 's':

            address.clear();
            address.append(optarg);

            break;
        case 'p':

            portNumber.clear();
            portNumber.append(optarg);

            break;
        case 'n':

            username.clear();
            username.append(optarg);

            break;
        default:
            // if any defualt case occurs it is an error so set return value to one end loop and return
            argHandlerRetVal = 1;

            std::cerr << "argHandler() - ran into defualt case: " << std::endl;

            break;
        }
    }

    return argHandlerRetVal;

}

int main(int argc, char* argv[]){

    int mainRetVal = goodReturn;
    Logger logit("networking-log.txt");
    if((mainRetVal = handleArgs(argc,argv))){

        if(mainRetVal == -1){

            return goodReturn;

        }else{

            std::cerr << "main() - problem with handleArgs()" << std::endl;
            return mainRetVal;
        }

    }

    //read back server detail strings
    std::cout << "Server to connect to: " << address << std::endl;
    std::cout << "Server's port to connect to: " << portNumber << std::endl;
    std::cout << "Username to join as: " << username << std::endl;

    //to test using netcat or joey's server
    try
    {
        RCnetworking testNetworkingAPI(portNumber, address, username, &logit);
        testNetworkingAPI.sendMessage("Hello!");
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // delay to avoid crashing Perry's server

        while (!testNetworkingAPI.checkForRecv())
        {
            
            ;

        }
        
        std::cout << testNetworkingAPI.getMessage() << std::endl;

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        mainRetVal = 1;
    }




    return mainRetVal;
}
