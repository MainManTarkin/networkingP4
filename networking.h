#pragma once

/* CSC 3600 Spring 2023 Project 4
 * Retro Chat - RCnetworking class
 *
 * Created by:
 * Brandon Davis
 * Joey Sachtleben
 * Dalton Callow
 * Kyle Stuersel
 * Daniel Anilao
 */

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

/* RCnetworking
 * The RCnetworking class is responsible for handling all network communication
 * for the retro chat application. It provides a simple interface for establishing
 * a connection, sending and receiving messages, and closing the connection.
 *
 * Function overview:
 *
 * RCnetworking(std::string portInput, std::string addressInput, std::string userNameInput)
 * The constructor for the class creates a socket and establishes a connection
 * to the server at the provided address and port. The server is informed of
 * the client's username as well.
 *
 * ~RCnetworking()
 * The destructor for the class sends a disconnection message to the server,
 * closes the socket, and performs any additional cleanup required.
 *
 * int sendMessage(std::string messageInput)
 * This function takes a provided string and sends it to the server.
 *
 * bool checkForRecv()
 * This function is used to check if there is a new message waiting to be recieved.
 * It will not block if there is no new messages from the server.
 *
 * std::string getMessage()
 * This function retrieves a complete message from the server.
 */
class RCnetworking
{
public:
    /* RCnetworking constructor
     * Responsible for opening a socket and connecting to the specified address.
     * Will also inform the server of the client's username in the first message.
     * After the constructor is finished the client is ready to send and receive
     * messages with the server.
     * Parameters:
     *   std::string portInput     -- The port on the server to connect to.
     *   std::string addressInput  -- The address of the server to connect to.
     *   std::string userNameInput -- The username of the client.
     * Exceptions:
     *   A std::runtime_error is thrown if any issues setting up the connection
     *   are encountered. An error message may be printed to standard error as
     *   well. Other exceptions may be thrown by other functions called in the
     *   constructor.
     */
    RCnetworking(std::string portInput, std::string addressInput, std::string userNameInput);

    /* RCnetworking destructor
     * Will send a disconnection message to the server informing it of the
     * client's departure. After this is sent, the socket will be closed. Any
     * other resources used by the class will be cleaned up as well.
     */
    ~RCnetworking();

    /* sendMessage
     * Sends a text message to the server. The message will be formatted for
     * sending; i.e., the proper command will be prefixed and a newline will be
     * appended.
     * Parameters:
     *   std::string messageInput -- the message to be sent
     * Returns:
     *   An integer indicating if the message was sent.
     * !!! Could someone confirm the purpose of this int return?
     * !!! I'm not sure why we need it given the function SHOULD throw if there
     * !!! is an error sending the message
     * Exceptions:
     *   A std::runtime_error will be thrown if there is an error sending the message.
     *   An error message may be printed to standard error as well. Other
     *   exceptions may be thrown by other functions called in the function.
     */
    int sendMessage(std::string messageInput);

    /* checkForRecv
     * Checks if there is a new message from the server. This call will not block
     * if there is no message to be had.
     * Parameters:
     *   None.
     * Returns:
     *   A boolean value; if it is true then there is a message that can be
     *   received with getMessage.
     * Exceptions:
     *   A std::runtime_error will be thrown if there is an error checking for
     *   incoming messages. An error message may be printed to standard error
     *   as well. Other exceptions may be thrown by other functions called in
     *   the function.
     */
    bool checkForRecv();

    /* getMessage
     * This function will fully receive a complete text message from the server
     * and return it to the caller. If there is no message to get (i.e.,
     * checkForRecv would return false) then an empty string will be returned.
     * The returned message will end in a newline.
     * Parameters:
     *   None.
     * Returns:
     *   A string with the message from the server. If the string is empty then
     *   there is no message to get.
     * Exceptions:
     *   A std::runtime_error will be thrown if there is an error reading
     *   incoming messages. An error message may be printed to standard error
     *   as well. Other exceptions may be thrown by other functions called in
     *   the function.
     */
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
