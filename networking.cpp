#include "networking.h"

RCnetworking::RCnetworking(std::string portInput, std::string addressInput, std::string userNameInput)
{

    int getAddressReturnVal = 0;

    struct addrinfo basedInfo;
    struct addrinfo *serverAddrInfo = nullptr;
    struct addrinfo *addList = nullptr;

    memset(&basedInfo, 0, sizeof(struct addrinfo));

    // set socket for tcp and dont care about ip version
    basedInfo.ai_family = AF_UNSPEC;
    basedInfo.ai_socktype = SOCK_STREAM;

    if ((getAddressReturnVal = getaddrinfo(addressInput.c_str(), portInput.c_str(), &basedInfo, &serverAddrInfo)) != 0)
    {
        std::cerr << "RCnetworking() - problem with getaddrinfo(): Line: " << (__LINE__ - 2) << "| return code: " << gai_strerror(getAddressReturnVal) << std::endl;

        throw std::runtime_error("you put in the wrong input num-nuts");
    }

    for (addList = serverAddrInfo; addList != NULL; addList = addList->ai_next)
    {
        if ((clientSocket = socket(addList->ai_family, addList->ai_socktype, addList->ai_protocol)) == -1)
        {
            perror("RCnetworking() - problem with socket(): ");

            continue;
        }

        if (connect(clientSocket, addList->ai_addr, addList->ai_addrlen) == -1)
        {
            if (close(clientSocket))
            {

                perror("RCnetworking() - problem with close(): ");

                std::cerr << "Line: " << (__LINE__ - 5) << std::endl;
            }
            clientSocket = closedSocketVal;
            perror("initClient() - problem with connect(): ");
            continue;
        }

        break;
    }

    if (addList == NULL)
    {
        std::cerr << "RCnetworking() - failed to connect to server" << std::endl;
        throw std::runtime_error("check your internet connection cause it probs sucks num-nuts");
    }


    freeaddrinfo(serverAddrInfo);



}
