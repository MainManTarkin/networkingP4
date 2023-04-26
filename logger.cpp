#include "logger.h"

Logger::Logger(std::string fileName)
{
    fileStream.open(fileName, std::ofstream::out | std::ofstream::app);
    if(!fileStream.is_open())
    {
        throw std::runtime_error("Error when initializing log file \"" + fileName + "\" ");
    }
    
    fileStream << "Log file begin\n";
    lastMessage = "Log file begin\n";
}
//--
Logger::Logger(const char* fileName) : Logger(std::string(fileName))
{   
}
//--
Logger::~Logger()
{
    fileStream << "Log file closing...\n";
    fileStream.close();
}
//--
bool Logger::AddMessageToLog(std::string message)
{
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