#include "networking.h"

const char ackMessage[] = "A|OK\n";

void RCnetworking::prepareMessage(std::string *messageToSend, char command){


    switch (command)
    {
    case 'G':
        messageToSend->clear();
        messageToSend->insert(messageToSend->begin(), command);
        messageToSend->append("|OODBYE\n");
        
        break;
    
    default:
        messageToSend->insert(messageToSend->begin(),command);
        messageToSend->insert((messageToSend->begin() + 1),command);
        messageToSend->append("\n");
        break;
    }


}

RCnetworking::RCnetworking(std::string portInput, std::string addressInput, std::string userNameInput)
{

    int getAddressReturnVal = 0;
    int bytesSent = 0;
    int bytesRecv = 0;

    int delayIterations = 30;

    struct timeval timeoutVal = { .tv_sec = 3, .tv_usec = 0};
    
    fd_set readfds;

    int highFileDescriptor = 0;
    int selectRV = 0;

    struct addrinfo basedInfo;
    struct addrinfo *serverAddrInfo = nullptr;
    struct addrinfo *addList = nullptr;

    memset(&basedInfo, 0, sizeof(struct addrinfo));

    //set up buffer

    try
    {

        bufferRecv = new char[recvSocketBufferSize];

    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        throw std::runtime_error("failed allocation of recv buffer");
    }
    



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

    if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1)
        {

            perror("prepareClient() - problem setting the socket to non-blocking: ");

            throw std::runtime_error("you can not even block a socket num-nuts");
        }

    prepareMessage(&userNameInput,'H');

    if((bytesSent = send(clientSocket,userNameInput.c_str(), userNameInput.size(),0)) == -1){

        perror("RCnetworking() - problem with send(): ");

        throw std::runtime_error("your grandmas potato cant even send the message num-nuts");
    }

    /* commented incase we need to go back to this approach
    do
    {
    
        if((bytesRecv = recv(clientSocket,bufferRecv, bufferSize,0)) == -1){

            if (errno != EAGAIN || errno != EWOULDBLOCK)
            {

                perror("RCnetworking() - problem with recv(): ");


                throw std::runtime_error("your computer recv()s messages like a drunken sailor num-nuts");
            }

        }

        if(bytesRecv > 0){



        }

    } while (delayIterations--);
    */

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

    if((selectRV = select(highFileDescriptor,&readfds,NULL,NULL, &timeoutVal)) == -1){

        perror("RCnetworking() - problem with select(): ");

        throw std::runtime_error("you could not even select() a functional socket num-nuts");

    }else if(!selectRV){

        throw std::runtime_error("select() timed out for receiving ack message from server ");

    }else{

        if((bytesRecv = recv(clientSocket,bufferRecv, bufferSize,0)) == -1){

            if (errno != EAGAIN || errno != EWOULDBLOCK)
            {

                perror("RCnetworking() - problem with recv(): ");


                throw std::runtime_error("your computer recv()s messages like a drunken sailor num-nuts");
            }

        }else{

            if(strncmp(bufferRecv, ackMessage, sizeof(ackMessage))){

                throw std::runtime_error("server sent bad ack message");

            }

        }



    }

    memset(bufferRecv, 0, bufferSize);
}
