/**
 * @file Logger.hxx
 * @ingroup Core
 * @brief Defines a lightweight Logger class for formatted output and log-level control in the MASFENON framework.
 * @details Provides print utilities for standard messages, warnings, and errors. The logger supports stream-style output and 
 * can be enabled or disabled dynamically during runtime.
 */

#pragma once

#include <iostream>
#include <stdexcept>

/**
 * @class Logger
 * @brief Simple logging utility for writing formatted messages to an output stream.
 * @details The Logger class allows writing generic log messages, warnings, and errors to a provided output stream. Logging can be toggled on or off.
 */
class Logger {
public:
    /**
     * @brief Constructs a Logger with the given output stream.
     * @param os Reference to the output stream (e.g., std::cout, std::cerr).
     */
    Logger(std::ostream& os) : os_(os) {}

    /**
     * @brief Overloaded stream insertion operator for generic types.
     * @tparam T The type of the object to stream.
     * @param t The object to write to the stream.
     * @return Reference to the current Logger instance.
     */
    template<typename T>
    Logger& operator<<(const T& t) {
        if(enabled_) os_ << t;
        return *this;
    }

    /**
     * @brief Overloaded stream insertion operator for manipulators (e.g., std::endl).
     * @param pf Stream manipulator function.
     * @return Reference to the current Logger instance.
     */
    Logger& operator<<(std::ostream& (*pf)(std::ostream&)) {
        if(enabled_) os_ << pf;
        return *this;
    }

    /**
     * @brief Prints a standard log message.
     * @param msg The message to print.
     * @param isVerbose If true, the message will only be printed if verbose mode is enabled.
     * @return Reference to the current Logger instance.
     */
    Logger& printLog(const std::string& msg, bool isVerbose = false);

    /**
     * @brief Prints a standard log message. Using variadic templates for flexibility.
     * @param isVerbose If true, the message will only be printed if verbose mode is enabled.
     * @tparam Args Types of the arguments to format into the message.
     * @return Reference to the current Logger instance.
     */
    template<typename... Args>
    Logger& printLog(bool isVerbose, Args&&... args) {
        if (enabled_) {
            if (isVerbose && !verbose_) {
                return *this; // Skip verbose messages if not enabled
            }
            os_ << "[LOG] ";
            ((os_ << ' ' << std::forward<Args>(args)), ...);
        }
        return *this;
    }

    /**
     * @brief Prints an error message.
     * @param msg The error message to print.
     * @return Reference to the current Logger instance.
     */
    Logger& printError(const std::string& msg);

    /**
     * @brief Prints a warning message.
     * @param msg The warning message to print.
     * @return Reference to the current Logger instance.
     */
    Logger& printWarning(const std::string& msg);

    /**
     * @brief Enables the logger.
     * @details Once enabled, log output will be written to the configured stream.
     */
    void enable();

    /**
     * @brief Disables the logger.
     * @details When disabled, no output will be written to the stream.
     * @see enable
     */
    void disable();

    /**
     * @brief Enables verbose logging mode.
     * @details In verbose mode, additional detailed messages can be printed.
     */
    void enableVerbose();
    
    /**
     * @brief Disables verbose logging mode.
     * @details In non-verbose mode, only standard log messages will be printed.
     */
    void disableVerbose();

private:
    std::ostream& os_; ///< Output stream reference
    bool enabled_ = true; ///< Flag indicating if logging is currently enabled
    bool verbose_ = false; ///< Flag for verbose logging mode
};
 