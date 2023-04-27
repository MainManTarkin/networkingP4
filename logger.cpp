#include "logger.h"

Logger::Logger(std::string fileName)
{
    fileStream.open(fileName, std::ofstream::out | std::ofstream::app);
    if(!fileStream.is_open())
    {
        throw std::runtime_error("Error when initializing log file \"" + fileName + "\" ");
    }
    
    // This message will indicate when the file is correctly initialized in the constructor.
    fileStream << "Log file begin\n"; 
    lastMessage = "Log file begin\n";
}
//--
Logger::Logger(const char* fileName) : Logger(std::string(fileName))
{   
    // Just calls the string constructor
}
//--
Logger::~Logger()
{
    // This message will indicate the file closed via the deconstructor, and not some other failure
    fileStream << "Log file closing...\n"; 
    fileStream.close();
}
//--
bool Logger::AddMessageToLog(std::string message)
{
    // Note: good() will return false if any of the fstream's stateflags are set (badbit, eofbit or failbit)
    // Will want to consider changing this because a stream issue could occur before or after the write
    if(fileStream.good())
    {
        fileStream << message << "\n";
        lastMessage = message + "\n";
        return true;
    }
    else
    {
        return false;
    }
}
//--
bool Logger::AddMessageToLog(const char* message)
{
    return AddMessageToLog(std::string(message));
}
//--
std::string Logger::GetLastMessage()
{
    return lastMessage;
}