/**
 * @file Logger.cxx
 * @ingroup Core
 * @brief Implements the Logger class for writing log, error, and warning messages.
 * @details Provides log-level formatted output to a stream and allows runtime toggling of logging behavior.
 */

#include "logging/Logger.hxx"


Logger& Logger::printLog(const std::string& msg, bool isVerbose){
    if(enabled_){
        os_ << "[LOG] "<< msg;
    }
    return *this;
}

Logger& Logger::printError(const std::string& msg){
    if(enabled_){
        os_ << "[ERROR] "<< msg;
    }
    return *this;
}

Logger& Logger::printWarning(const std::string& msg){
    if(enabled_){
        os_ << "[WARNING] "<< msg;
    }
    return *this;
}

void Logger::enable(){
    enabled_ = true;
}

void Logger::disable(){
    enabled_ = false;
}