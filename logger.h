#pragma once
#include <string>
#include <fstream>
#include <errno.h>

/*
    Objects of this class will allow devs to add messages to a text files 
    which will allow for any sort of message logging (i.e. Errors, Checkpoints)
*/
class Logger
{
    public:
        /*
            The constructor will create a file with the name fileName. 
            It will also automatically input an initialization message to the file ("Log file begin") to indicate the file was created properly.
            If the file is not opened/created, a runtime exception will be thrown containing the issue
        */
        Logger(std::string fileName);
        Logger(const char* fileName);

        /*
            This deconstructor will output a "Log file closing..." message to indicate we have reached the end of it's scope
            And will close the file stream
        */
        ~Logger();

        /*
            These methods will add the provided message string to the log file
            Returns:
            -> True, if the message was successfully added to the opened file
            -> False, if the file was able to be written to
        */
        bool AddMessageToLog(std::string message);
        bool AddMessageToLog(const char* message);

        /*
            This method will return whatever the previously added string was to the file.
        */
        std::string GetLastMessage();
    private:
        std::string fileName;
        std::fstream fileStream;
        std::string lastMessage;
};