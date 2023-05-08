#include "networking.h"
#include <sstream>

static const char ackMessage[] = "A|OK\n";
#define intermediateBufferSize 50
#define maxPortNum 0xffff
#define minPortNum 0

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

// If you're unfamiliar with the syntax here, I'm initializing the 'log' variable
// by calling the Logger constructor
// This way it can just stay as a regular member and not a pointer or anything weird
RCnetworking::RCnetworking(std::string portInput, std::string addressInput, std::string userNameInput, Logger *logInput)
{

    int getAddressReturnVal = 0;
    int bytesSent = 0;
    int bytesRecv = 0;

    struct timeval timeoutVal;
    timeoutVal.tv_sec = timeOutSec;
    timeoutVal.tv_usec = timeOutUSec;

    int highFileDescriptor = 0;
    int selectRV = 0;

    fd_set readfds;

    char ackRecvBuffer[20];

    memset(ackRecvBuffer, 0, sizeof(ackRecvBuffer));
    memset(&basedInfo, 0, sizeof(struct addrinfo));

    std::stringstream ss;

    // check user input
    int portInt = std::stoi(portInput);
    // check for vaild user port inputs
    if (portInt > maxPortNum || portInt < minPortNum)
    {

        ss << "RCnetworking() - gave invalid port number: " << portInt;
        log->AddMessageToLog(ss.str());
        throw std::runtime_error("Invalid port number given to RCnetworking()");
    }

    log = logInput;

    // set socket for tcp and dont care about ip version
    basedInfo.ai_family = AF_UNSPEC;
    basedInfo.ai_socktype = SOCK_STREAM;

    if ((getAddressReturnVal = getaddrinfo(addressInput.c_str(), portInput.c_str(), &basedInfo, &serverAddrInfo)))
    {

        ss << "RCnetworking() - problem with getaddrinfo(): Line: " << (__LINE__ - 2)
           << "| return code: " << gai_strerror(getAddressReturnVal);
        log->AddMessageToLog(ss.str());
        throw std::runtime_error("Error retrieving address info");
    }

    getAdderListCreated = true;

    for (addList = serverAddrInfo; addList != NULL; addList = addList->ai_next)
    {
        if ((clientSocket = socket(addList->ai_family, addList->ai_socktype, addList->ai_protocol)) == -1)
        {

            ss << "RCnetworking() - problem with socket(): " << strerror(errno);
            log->AddMessageToLog(ss.str());
            continue;
        }

        if (connect(clientSocket, addList->ai_addr, addList->ai_addrlen) == -1)
        {
            if (close(clientSocket))
            {

                ss << "RCnetworking() - problem with close(): " << strerror(errno)
                   << std::endl
                   << "Line: " << (__LINE__ - 5);
                log->AddMessageToLog(ss.str());
            }
            clientSocket = closedSocketVal;
            continue;
        }

        break;
    }

    if (addList == NULL)
    {

        ss << "RCnetworking() - failed to connect to server with address: " << addressInput;
        log->AddMessageToLog(ss.str());

        throw std::runtime_error("Failed to connect to server with address " + addressInput);
    }

    socketCreated = true;

    if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1)
    {

        ss << "prepareClient() - problem setting the socket to non-blocking: "
           << strerror(errno);
        log->AddMessageToLog(ss.str());
        throw std::runtime_error("Error setting up socket configuration");
    }

    prepareMessage(&userNameInput, 'H');

    if ((bytesSent = send(clientSocket, userNameInput.c_str(), userNameInput.size(), 0)) == -1)
    {

        ss << "RCnetworking() - problem with send(): " << strerror(errno);
        log->AddMessageToLog(ss.str());

        throw std::runtime_error("Error sending initial message to server");
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

        ss << "RCnetworking() - problem with select(): " << strerror(errno);
        log->AddMessageToLog(ss.str());

        throw std::runtime_error("Error with select() while configuring socket");
    }
    else if (!selectRV)
    {

        throw std::runtime_error("select() timed out for receiving ack message from server ");
    }
    else
    {

        if ((bytesRecv = recv(clientSocket, ackRecvBuffer, sizeof(ackRecvBuffer), 0)) == -1)
        {

            if (errno != EAGAIN || errno != EWOULDBLOCK)
            {

                ss << "RCnetworking() - problem with recv(): " << strerror(errno);
                log->AddMessageToLog(ss.str());
                throw std::runtime_error("Error receiving ack message from server ");
            }
        }
        else
        {

            if (strncmp(ackRecvBuffer, ackMessage, sizeof(ackMessage)))
            {
                ss << "Received unexpected ack message from server: " << ackRecvBuffer;
                log->AddMessageToLog(ss.str());
                throw std::runtime_error("Server sent bad ack message");
            }
        }
    }
}

RCnetworking::~RCnetworking()
{

    if (socketCreated)
    {

        std::string goodbyeMessage;

        prepareMessage(&goodbyeMessage, 'G');

        if (send(clientSocket, goodbyeMessage.c_str(), goodbyeMessage.size(), 0) == -1)
        {
            std::stringstream ss;
            ss << "~RCnetworking() - problem with send()ing goodbye message: "
               << strerror(errno);
            log->AddMessageToLog(ss.str());
        }

        if (close(clientSocket))
        {
            std::stringstream ss;
            ss << "~RCnetworking() - problem with close()ing socket: "
               << strerror(errno);
            log->AddMessageToLog(ss.str());
        }
    }

    if (getAdderListCreated)
    {

        freeaddrinfo(serverAddrInfo);
    }
}

int RCnetworking::sendMessage(std::string messageInput)
{
    prepareMessage(&messageInput, 'T');
    int sentAmount = send(clientSocket, messageInput.c_str(), messageInput.size() + 1, 0);
    if (sentAmount == -1)
    {
        std::runtime_error e(strerror(errno));
        throw e;
    }
    else if (sentAmount < static_cast<int>(messageInput.size() + 1))
    {
        std::stringstream ss;
        ss << "Failed to send full message";
        log->AddMessageToLog(ss.str());
        // deal with this better
    }

    return sentAmount;
}

int RCnetworking::handleRecv()
{

    ssize_t bytesRecv = 0;
    ssize_t totalBytesRecv = 0;

    //used to hold bytes from recv() before appending them to main string buffer recvMessage
    char intermediateBuffer[intermediateBufferSize];

    size_t startOfMessage = 0;
    size_t endOfMessage = 0;
    size_t messageIndex = 0;

    std::stringstream ss;

    memset(intermediateBuffer, 0, intermediateBufferSize);

    if ((bytesRecv = recv(clientSocket, intermediateBuffer, (sizeof(intermediateBuffer) - 1), 0)) == -1)
    {

        if (errno != EAGAIN || errno != EWOULDBLOCK)
        {
            ss << "handleRecv() - problem with recv at line: " << (__LINE__ - 5) ;
            log->AddMessageToLog(ss.str());
            throw std::runtime_error(strerror(errno));
        }
    }
    else if (!bytesRecv)
    { // handle server disconnection


        ss << "handleRecv() - problem with recv at line: " << (__LINE__ - 14);
        log->AddMessageToLog(ss.str());
        throw std::runtime_error("Server dropped connection");
    }
    else if (bytesRecv > 0)
    {
        //add are recevied message to the string buffer
        recvMessage.append(intermediateBuffer);
        totalBytesRecv += bytesRecv;

        if (bytesRecv >= (ssize_t)(sizeof(intermediateBuffer) - 1))//for handling extra recvs
        {//keep looping until recv returns zero bytes

            while (bytesRecv)
            {

                memset(intermediateBuffer, 0, intermediateBufferSize);

                if ((bytesRecv = recv(clientSocket, intermediateBuffer, (sizeof(intermediateBuffer) - 1), 0)) == -1)
                {
                    //the if block handles any errors given by recv()
                    //other wise the errors are related to non-blocking returning zero bytes
                    //break the loop when there is no more to recv
                    if (errno != EAGAIN || errno != EWOULDBLOCK)
                    {
                        ss << "handleRecv() - problem with recv at line: " << (__LINE__ - 7);
                        log->AddMessageToLog(ss.str());
                        throw std::runtime_error(strerror(errno));
                    }
                    else
                    {

                        break;
                    }
                }
                else if (bytesRecv > 0)
                {

                    totalBytesRecv += bytesRecv;
                    recvMessage.append(intermediateBuffer);
                }
            }
        }

        //string buffer parser
        //the string buffer recvMessage is parsed to find any message it may contain
        //since all messages begin with T| this is the first thing it looks for
        //when found it moves to an inner loop that looks for the ending point of a message (if any)
        //when a message is found a substring of the message (not including the "T|" or "\n" is place in messsage vector)
        //the main loop begins iterating again, with its index being after the end of the last message

        for (size_t c = messageIndex; c < recvMessage.size(); c++)
        {

            if (!strncmp((recvMessage.c_str() + c), "T|", 2))
            {

                startOfMessage = (c + 2);

                for (size_t i = startOfMessage; i < recvMessage.size(); i++)
                {

                    if (recvMessage[i] == '\n')
                    {
                        c = i;
                        endOfMessage = (i - 2);
                        messageIndex = (i + 1);
                        recvedStrings.push_back(recvMessage.substr(startOfMessage, endOfMessage));
                        break;
                    }
                }
            }
        }
    }

    if (!recvedStrings.size())
    {
        // we may have recieved bytes but we do not have a complete message yet
        // set the return to zero to indicate to the user
        totalBytesRecv = 0;
    }
    else
    {
        //remove the portion string buffer that contained messages that were parsed
        recvMessage = recvMessage.substr(messageIndex);
    }

    return totalBytesRecv;
}

bool RCnetworking::checkForRecv()
{

    bool wasThereData = false;

    try
    {
        handleRecv();

        if (recvedStrings.size() > 0)
        {

            wasThereData = true;
        }
    }
    catch (const std::exception &e)
    {
        log->AddMessageToLog(e.what());
        throw std::runtime_error("checkForRecv() - failure in handleRecv()");
    }

    return wasThereData;
}

// returns empty string if there are no messages in vector
std::string RCnetworking::getMessage()
{

    std::string fetchedString;

    if (recvedStrings.size())
    { // only get a message if there is one in the vector

        fetchedString = recvedStrings[recvedStrings.size() - 1];

        recvedStrings.pop_back();
    }

    return fetchedString;
}
