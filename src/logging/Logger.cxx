/**
 * @file Logger.cxx
 * @ingroup Core
 * @brief Implements the Logger class for writing log, error, and warning messages.
 * @details Provides log-level formatted output to a stream and allows runtime toggling of logging behavior.
 */

#include "logging/Logger.hxx"


Logger& Logger::printLog(const std::string& msg, bool isVerbose){
    if(enabled_){
        if(isVerbose && !verbose_){
            return *this; // Skip verbose messages if not enabled
        }
        std::lock_guard<std::mutex> lock(mtx_); // Lock the mutex
        os_ << "[LOG] "<< msg;
        os_ << std::endl; // Ensure log messages end with a newline
    }
    return *this;
}

Logger& Logger::printError(const std::string& msg){
    if(enabled_){
        std::lock_guard<std::mutex> lock(mtx_); // Lock the mutex
        os_ << "[ERROR] "<< msg;
        os_ << std::endl; // Ensure error messages end with a newline
    }
    return *this;
}

Logger& Logger::printWarning(const std::string& msg){
    if(enabled_){
        std::lock_guard<std::mutex> lock(mtx_); // Lock the mutex
        os_ << "[WARNING] "<< msg;
        os_ << std::endl; // Ensure warning messages end with a newline
    }
    if(treatWarningsAsErrors_){
        // If warnings are treated as errors, we print them as warnings but also throw an exception
        os_ << "[WARNING-AS-ERROR] " << msg;
        throw std::runtime_error("[WARNING-AS-ERROR] " + msg);
    }
        
    return *this;
}

void Logger::enable(){
    enabled_ = true;
}

void Logger::disable(){
    enabled_ = false;
}

void Logger::enableVerbose(){
    verbose_ = true;
}

void Logger::disableVerbose(){
    verbose_ = false;
}

void Logger::setTreatWarningsAsErrors(bool treatWarningsAsErrors){
    treatWarningsAsErrors_ = treatWarningsAsErrors;
}
