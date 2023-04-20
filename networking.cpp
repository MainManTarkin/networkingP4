#include "networking.h"

const char ackMessage[] = "A|OK\n";

void RCnetworking::prepareMessage(std::string *messageToSend, char command)
{

    switch (command)
    {
    case 'G':
        messageToSend->clear();
        messageToSend->insert(messageToSend->begin(), command);
        messageToSend->append("|OODBYE\n");

        break;

    default:
        messageToSend->insert(messageToSend->begin(), command);
        messageToSend->insert((messageToSend->begin() + 1), '|');
        messageToSend->append("\n");
        break;
    }
}

RCnetworking::RCnetworking(std::string portInput, std::string addressInput, std::string userNameInput)
{

    int getAddressReturnVal = 0;
    int bytesSent = 0;
    int bytesRecv = 0;

    struct timeval timeoutVal;
    timeoutVal.tv_sec = timeOutSec;
    timeoutVal.tv_usec = timeOutUSec;

    fd_set readfds;

    int highFileDescriptor = 0;
    int selectRV = 0;

    memset(&basedInfo, 0, sizeof(struct addrinfo));

    // set up buffer

    try
    {

        bufferRecv = new char[recvSocketBufferSize];
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        throw std::runtime_error("failed allocation of recv buffer");
    }

    bufferAllocated = true;

    // set socket for tcp and dont care about ip version
    basedInfo.ai_family = AF_UNSPEC;
    basedInfo.ai_socktype = SOCK_STREAM;

    if ((getAddressReturnVal = getaddrinfo(addressInput.c_str(), portInput.c_str(), &basedInfo, &serverAddrInfo)) != 0)
    {
        std::cerr << "RCnetworking() - problem with getaddrinfo(): Line: " << (__LINE__ - 2) << "| return code: " << gai_strerror(getAddressReturnVal) << std::endl;

        this->~RCnetworking();

        throw std::runtime_error("you put in the wrong input num-nuts");
    }

    getAdderListCreated = true;

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
            continue;
        }

        break;
    }

    if (addList == NULL)
    {
        std::cerr << "RCnetworking() - failed to connect to server" << std::endl;
        this->~RCnetworking();
        throw std::runtime_error("check your internet connection cause it probs sucks num-nuts");
    }

    socketCreated = true;

    if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1)
    {

        perror("prepareClient() - problem setting the socket to non-blocking: ");
        this->~RCnetworking();
        throw std::runtime_error("you can not even block a socket num-nuts");
    }

    prepareMessage(&userNameInput, 'H');

    if ((bytesSent = send(clientSocket, userNameInput.c_str(), userNameInput.size(), 0)) == -1)
    {

        perror("RCnetworking() - problem with send(): ");
        this->~RCnetworking();
        throw std::runtime_error("your grandmas potato cant even send the message num-nuts");
    }

    /*
     *    clear and add our only socket to the file descriptor list
     *    we prepare the variables needed for select() by stating the highest number FD+1
     *    the timeout value is set the three seconds
     *    select() is called to wait until we recv the ack message from the server or the timeout expires
     *
     */

    FD_ZERO(&readfds);
    FD_SET(clientSocket, &readfds);

    highFileDescriptor = clientSocket + 1;

    if ((selectRV = select(highFileDescriptor, &readfds, NULL, NULL, &timeoutVal)) == -1)
    {

        perror("RCnetworking() - problem with select(): ");
        this->~RCnetworking();
        throw std::runtime_error("you could not even select() a functional socket num-nuts");
    }
    else if (!selectRV)
    {
        this->~RCnetworking();
        throw std::runtime_error("select() timed out for receiving ack message from server ");
    }
    else
    {

        if ((bytesRecv = recv(clientSocket, bufferRecv, bufferSize, 0)) == -1)
        {

            if (errno != EAGAIN || errno != EWOULDBLOCK)
            {

                perror("RCnetworking() - problem with recv(): ");
                this->~RCnetworking();
                throw std::runtime_error("your computer recv()s messages like a drunken sailor num-nuts");
            }
        }
        else
        {

            if (strncmp(bufferRecv, ackMessage, sizeof(ackMessage)))
            {
                this->~RCnetworking();
                throw std::runtime_error("server sent bad ack message");
            }
        }
    }

    memset(bufferRecv, 0, bufferSize);
}

RCnetworking::~RCnetworking()
{

    if (socketCreated)
    {

        std::string goodbyeMessage;

        prepareMessage(&goodbyeMessage, 'G');

        if (send(clientSocket, goodbyeMessage.c_str(), goodbyeMessage.size(), 0) == -1)
        {

            perror("~RCnetworking() - problem with send()ing goodbye message: ");
        }

        if (close(clientSocket))
        {

            perror("~RCnetworking() - problem with close()ing socket: ");
        }
    }

    if (bufferAllocated)
    {

        delete[] bufferRecv;
    }

    if (getAdderListCreated)
    {

        freeaddrinfo(serverAddrInfo);
    }
}