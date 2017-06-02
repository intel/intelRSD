/*!
 * @brief C++ logger interface
 *
 * @copyright Copyright (c) 2016-2017 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @header{Files}
 * @file logger.hpp
 */

#pragma once

#include <sstream>
#include <string>
#include <iomanip>
#include <vector>
#include <array>
#include <memory>
#include <list>

/*!
 * @def LOGGER_CPP_OVER_LOGGER_C_MACROS
 * If defined, disable C logger macros that can redefine C++ logger macros
 * */
#ifndef LOGGER_CPP_OVER_LOGGER_C_MACROS
#define LOGGER_CPP_OVER_LOGGER_C_MACROS
#endif
#include "logger/logger.h"

#include "logger/logger_options.hpp"

namespace logger_cpp {

class Stream;

/*!
 * @class logger_cpp::Logger
 * @brief Logger object get and send log message to #logger_cpp::Stream
 * */
class Logger {
private:
    struct logger* m_impl = nullptr;

    Logger(Logger&&) = delete;
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::list<std::shared_ptr<const Stream>> m_streams;

public:
    /*!
     * @brief Create logger instance with tag string and override logger
     * options
     *
     * @param[in]   tag     Tag string, if nullptr disable tagging
     * @param[in]   options Logger options, if nullptr set default options
     * */
    Logger(const char* tag = nullptr, const Options& options = Options());

    /*!
     * @brief Destroy logger object instance
     * */
    virtual ~Logger();

    /*!
     * @brief Get "C" struct with logger instance
     * @return logger instance
     * @warning It must be public, it is called from static function
     */
    struct logger* get_instance() const;

    /*!
     * @brief Check if message should be logged
     * @param level message level
     * @return true if should be logged
     */
    bool is_loggable(Level level) const;

    /*!
     * @brief Set logger options @see logger_cpp::Options
     *
     * @param[in]   options Logger options, if nullptr set default options,
     *              also call method without argument set to default
     * */
    void set_options(const Options& options);

    /*!
     * @brief Get logger options @see logger_cpp::Options
     *
     * @return      Logger options
     * */
    Options get_options();

    /*!
     * @brief Add Stream object to communicate with Logger object.
     * Logger object will send log messages to all added Stream objects
     *
     * @warning Note that this method is reentrant, but not thread-safe!
     *
     * @param[in]   stream  Stream object to add
     * */

    void add_stream(const std::shared_ptr<Stream> stream);
    void add_stream(Stream* stream);

    /*!
     * @brief Remove Stream object from logger object
     *
     * @warning Note that this method is reentrant, but not thread-safe!
     *
     * @param[in]   stream  Stream object to remove
     * */
    void remove_stream(const std::shared_ptr<Stream> stream);

    /*!
     * @brief Write log message. Use log_* functions instead this method
     *
     * @param[in]   logger_name     Logger name to be put in the log file
     * @param[in]   level           Log level
     * @param[in]   file_name       File name string
     * @param[in]   function_name   Function name string
     * @param[in]   line_number     Line number
     * @param[in]   str             Log message string to write
     * */
    virtual void write(
            const char* logger_name,
            enum Level level,
            const char* file_name,
            const char* function_name,
            unsigned int line_number,
            const std::string& str);

};
using LoggerSPtr = std::shared_ptr<Logger>;
using loggerUPtr = std::unique_ptr<Logger>;

/*!
 * @brief Write array to output stream object
 *
 * @param[in]   os Output stream
 * @param[in]   v  Array
 * @return      Output stream
 * */
template<typename T, size_t N>
std::ostream& operator<<(std::ostream& os, const std::array<T, N>& v) {
    std::ios::fmtflags flags = os.flags();
    os << std::uppercase << std::hex;

    os << "[" << v.size() << "][";
    if (v.size() > 0) {
        os << std::setfill('0') << std::setw(2);
        os << static_cast<unsigned int>(v[0]);
    }
    for (size_t i = 1; i < v.size(); i++) {
        os << " ";
        os << std::setfill('0') << std::setw(2);
        os << static_cast<unsigned int>(v[i]);
    }
    os << "]";
    os.flags(flags);
    return os;
}

/*!
 * @brief Write array contains signed char to output stream object
 *
 * @param[in]   os Output stream
 * @param[in]   v  Array contains signed char
 * @return      Output stream
 * */
template<size_t N>
std::ostream& operator<<(std::ostream& os, const std::array<char, N>& v) {
    return operator<<<char, N>(os, v);
}

/*!
 * @brief Write array contains unsigned char to output stream object
 *
 * @param[in]   os Output stream
 * @param[in]   v  Array contains unsigned char
 * @return      Output stream
 * */
template<size_t N>
std::ostream& operator<<(std::ostream& os, const std::array<unsigned char, N>& v) {
    return operator<<<unsigned char, N>(os, v);
}

/*!
 * @brief Write vector to output stream object
 *
 * @param[in]   os Output stream
 * @param[in]   v  Vector
 * @return      Output stream
 * */
template<typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
    std::ios::fmtflags flags = os.flags();
    os << std::uppercase << std::hex;

    os << "[" << v.size() << "][";
    if (v.size() > 0) {
        os << std::setfill('0') << std::setw(2);
        os << static_cast<unsigned int>(v[0]);
    }
    for (size_t i = 1; i < v.size(); i++) {
        os << " ";
        os << std::setfill('0') << std::setw(2);
        os << static_cast<unsigned int>(v[i]);
    }
    os << "]";
    os.flags(flags);
    return os;
}

/*!
 * @brief Write vector contains signed char to output stream object
 *
 * @param[in]   os Output stream
 * @param[in]   v  Vector contatins signed char
 * @return      Output stream
 * */
template<size_t N>
std::ostream& operator<<(std::ostream& os, const std::vector<char>& v) {
    return operator<<<char, N>(os, v);
}

/*!
 * @brief Write vector contains unsigned char to output stream object
 *
 * @param[in]   os Output stream
 * @param[in]   v  Vector contatins unsigned char
 * @return      Output stream
 * */
template<size_t N>
std::ostream& operator<<(std::ostream& os, const std::vector<unsigned char>& v) {
    return operator<<<unsigned char, N>(os, v);
}

/*!
 * @def LOGGER_FILE_NAME
 * Generate file name for log_* functions called in that file
 *
 * @def LOGGER_FUNCTION_NAME
 * Generate function name for log_* functions called in that function
 *
 * @def LOGGER_LINE_NUMBER
 * Generate line number for log_* functions called in that file
 * */
#ifndef LOGGER_MORE_DEBUG_OFF
#ifndef __FILENAME__
#define LOGGER_FILE_NAME                    __FILE__
#else
#define LOGGER_FILE_NAME                    __FILENAME__
#endif
#define LOGGER_FUNCTION_NAME                __func__
#define LOGGER_LINE_NUMBER                  __LINE__
#else
#define LOGGER_FILE_NAME                    NULL
#define LOGGER_FUNCTION_NAME                NULL
#define LOGGER_LINE_NUMBER                  0
#endif

/*!
 * @brief Get name of the logger requested by name
 *
 * @param logger_name name of the logger to be returned
 * @return logger_name
 */
const char* logger_get_logger_name(const char* logger_name);

/*!
 * @brief Get name of the requested logger
 *
 * @param logger logger shared poiter
 * @return logger_name
 */
const char* logger_get_logger_name(LoggerSPtr logger);

/*!
 * @brief Get logger by name
 *
 * Loggers are always found by name. This allow to log the name from log_xxx()
 * instead of the name of the configured logger.
 */
#define GET_LOGGER(name) name

/*!
 * @brief Get system logger
 *
 * Always return the name for default system logger.
 */
#define LOGUSR GET_LOGGER("default")

/*!
 * @brief Logger output stream write
 *
 * @param[in]   inst    Logger buffer instance
 * @param[in]   level   Log level
 * @param[in]   stream  Stream
 * */
#define log_write(inst, level, stream) \
    do {\
        logger_cpp::LoggerSPtr __ptr = logger_cpp::LoggerFactory::instance().get_logger(inst);\
        if (__ptr && __ptr->is_loggable(level)) {\
            std::stringstream __stream;\
            __stream << std::string("") << stream;\
            __ptr->write(\
                logger_cpp::logger_get_logger_name(inst),\
                level,\
                LOGGER_FILE_NAME,\
                LOGGER_FUNCTION_NAME,\
                LOGGER_LINE_NUMBER,\
                __stream.str()\
            );\
        }\
    } while (0)

/*!
 * @brief Emergency message, system is about to crash or is unstable
 *
 * @param[in]       inst   Logger instance
 * @param[in]       stream String stream object
 * */
#define log_emergency(inst, stream)\
    log_write(inst, logger_cpp::Level::EMERGENCY, stream)

/*!
 * @brief Something bad happened and action must be taken immediately
 *
 * @param[in]       inst    Logger instance
 * @param[in]       stream  String stream object
 * */
#define log_alert(inst, stream)\
   log_write(inst, logger_cpp::Level::ALERT, stream)

/*!
 * @brief A critical condition occured like serious hardware/software failure
 *
 * @param[in]       inst    Logger instance
 * @param[in]       stream  String stream object
 * */
#define log_critical(inst, stream)\
   log_write(inst, logger_cpp::Level::CRITICAL, stream)

/*!
 * @brief An error condition, often used by drivers to indicate difficulties
 * with the hardware
 *
 * @param[in]       inst    Logger instance
 * @param[in]       stream  String stream object
 * */
#define log_error(inst, stream)\
   log_write(inst, logger_cpp::Level::ERROR, stream)

/*!
 * @brief A warning, meaning nothing serious by itself but might indicate
 * problems
 *
 * @param[in]       inst    Logger instance
 * @param[in]       stream  String stream object
 * */
#define log_warning(inst, stream)\
   log_write(inst, logger_cpp::Level::WARNING, stream)

/*!
 * @brief Nothing serious, but notable nevertheless. Often used to report
 * security events
 *
 * @param[in]       inst    Logger instance
 * @param[in]       stream  String stream object
 * */
#define log_notice(inst, stream)\
   log_write(inst, logger_cpp::Level::NOTICE, stream)

/*!
 * @brief Informational message e.g. startup information
 *
 * @param[in]       inst    Logger instance
 * @param[in]       stream  String stream object
 * */
#define log_info(inst, stream)\
   log_write(inst, logger_cpp::Level::INFO, stream)

/*!
 * @brief Debug messages
 *
 * @param[in]       inst    Logger instance
 * @param[in]       stream  String stream object
 * */
#define log_debug(inst, stream)\
   log_write(inst, logger_cpp::Level::DEBUG, stream)

} /* namespace logger_cpp */

