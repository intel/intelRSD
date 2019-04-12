/*!
 * @brief C logger prototypes and macros
 *
 * @copyright Copyright (c) 2016-2019 Intel Corporation
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
 * @file logger.h
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>

/*!
 * Set default initial stream size for all created logger stream instances
 *
 * @def LOGGER_DEFAULT_STREAM_SIZE
 * Set default initial stream size for all created logger stream instances
 *
 * @def LOGGER_DEFAULT_BUFFER_SIZE
 * Set default initial buffer size for all created logger buffer instances
 *
 * @def LOGGER_DEFAULT_SOCKET_SIZE
 * Set default initial socket buffer size for all created logger stream
 * instances
 *
 * @def LOGGER_STREAM_UDP_RECEIVE_TIMEOUT_US
 * Set stream UDP receive timeout in microseconds. This should  avoid buffer
 * override on the side of receiver. UDP server can speed up getting new log
 * messages sending any UDP frame (even empty) to logger
 *
 * @def LOGGER_DEFAULT_THREAD_WAKE_SEC
 * Set default wake up time in seconds for all created logger stream threads.
 * After wake up, thread will flush all remaining data in buffer stream
 *
 * */
#define LOGGER_DEFAULT_STREAM_SIZE              4096
#define LOGGER_DEFAULT_BUFFER_SIZE              1024
#define LOGGER_DEFAULT_SOCKET_SIZE              512
#define LOGGER_STREAM_UDP_RECEIVE_TIMEOUT_US    5000
#define LOGGER_DEFAULT_THREAD_WAKE_SEC          1

/*!
 * @def LOGGER_ARRAY_SIZE(array)
 * Get number of elements in array. May be used with logbuf_array
 */
#define LOGGER_ARRAY_SIZE(array)            (sizeof(array)/sizeof(array[0]))

/*!
 * @def LOGGER_PRINTF_FORMAT(x, y)
 * Tell compiler that we use formated string like in printf functions. Compiler
 * will check for us invalid arguments and parameters in that functions
 * */
#define LOGGER_PRINTF_FORMAT(fmt_index, val_index)\
    __attribute__((__format__(__printf__, fmt_index, val_index)))

/*!
 * @def LOG_LEVEL_MASK
 * @brief Emergency messages, system is about to crash or is unstable
 *
 * @def LOG_EMERGENCY
 * @brief Something bad happened and action must be taken immediately
 *
 * @def LOG_ALERT
 * @brief Something bad happened
 *
 * @def LOG_CRITICAL
 * @brief A critical condition occurred like a serious hardware/software
 * failure
 *
 * @def LOG_ERROR
 * @brief An error condition, often used by drivers to indicate difficulties
 * with the hardware
 *
 * @def LOG_WARNING
 * @brief A warning, meaning nothing serious by itself but might indicate
 * problems
 *
 * @def LOG_NOTICE
 * @brief Nothing serious, but notably nevertheless. Often used to report
 * security events
 *
 * @def LOG_INFO
 * @brief Informational message e.g. startup information at driver
 * initialization
 *
 * @def LOG_DEBUG
 * @brief Debug messages
 *
 * @def LOG_TIME_MASK
 * @brief Time stamp mask
 *
 * @def LOG_TIME_NONE
 * @brief No time stamp
 *
 * @def LOG_TIME_DATE_SEC
 * @brief Time format: YYYY-MM-DD HH:MM:SS
 *
 * @def LOG_TIME_DATE_MS
 * @brief Time format: YYYY-MM-DD HH:MM:SS.mmm
 *
 * @def LOG_TIME_DATE_US
 * @brief Time format: YYYY-MM-DD HH:MM:SS.mmmuuu
 *
 * @def LOG_TIME_DATE_NS
 * @brief Time format: YYYY-MM-DD HH:MM:SS.mmmuuunnn
 * */
#define LOG_LEVEL_MASK      0x7
#define LOG_EMERGENCY       0
#define LOG_ALERT           1
#define LOG_CRITICAL        2
#define LOG_ERROR           3
#define LOG_WARNING         4
#define LOG_NOTICE          5
#define LOG_INFO            6
#define LOG_DEBUG           7

#define LOG_TIME_MASK       0x7
#define LOG_TIME_NONE       0
#define LOG_TIME_DATE_SEC   1
#define LOG_TIME_DATE_MS    2
#define LOG_TIME_DATE_US    3
#define LOG_TIME_DATE_NS    4

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @union logger_options
 * @brief Options for logger and stream objects
 *
 * @var logger_options::raw
 * Raw access to options
 *
 * @var logger_options::option
 * Bit field access to options
 *
 * @var logger_options::level
 * Log level
 *
 * @var logger_options::time_format
 * Time stamp format. Only applies for logger stream object
 *
 * @var logger_options::color
 * Enable/disable coloring output
 *
 * @var logger_options::tagging
 * Enable/disable tagging output
 *
 * @var logger_options::more_debug
 * Enable/disable more debug information like file name, function name and
 * line number
 *
 * @var logger_options::def_level
 * Logger level defined in the config file, used to
 * restore level on SIG_USR2
 *
 * @var logger_options::output_enable
 * Enable/disable output
 * */
union logger_options {
    struct {
        unsigned int level : 3;
        unsigned int time_format : 3;
        unsigned int color : 1;
        unsigned int tagging : 1;
        unsigned int more_debug : 1;
        unsigned int output_enable : 1;
        unsigned int def_level:3;
    }option;
    unsigned int raw;
};

/*!
 * @enum logger_status
 * @brief Logger status returned by all logger functions
 *
 * @var logger_status::LOGGER_SUCCESS
 * Success
 *
 * @var logger_status::LOGGER_ERROR
 * Unknown error
 *
 * @var logger_status::LOGGER_ERROR_NULL
 * Null pointer
 *
 * @var logger_status::LOGGER_ERROR_MEMORY_OUT
 * Failure after dynamic memory allocation
 *
 * @var logger_status::LOGGER_ERROR_TYPE
 * Invalid operations for particular logger stream type
 *
 * @var logger_status::LOGGER_ERROR_TIMEOUT
 * Timeout occur
 * */
enum logger_status {
    LOGGER_SUCCESS              =  0,
    LOGGER_ERROR                = -1,
    LOGGER_ERROR_NULL           = -2,
    LOGGER_ERROR_MEMORY_OUT     = -3,
    LOGGER_ERROR_TYPE           = -4,
    LOGGER_ERROR_TIMEOUT        = -5
};


/*!
 * @brief Handle logger configuration request.
 *
 * String must be formatted with "separators" ('=' for level, '@' for file,
 * ':' for enable), first value must be existing logger name.
 * @param[in] logger_command command to be parsed and executed
 * @note Executed on signal (SIGUSRx) and from debugger.
 * @see logger_handle_usr_signal()
 */
void logger_execute_command(const char* logger_command);

struct logger;
struct logger_stream;

/*!
 * @brief Create and initialize logger instance with default parameters
 *
 * @param[in]   tag Logger tag ID string
 * @param[in]   options Options for logger. NULL means default options
 * @return      When success return created logger instance otherwise
 *              return NULL
 * */
struct logger *logger_create(const char *tag, union logger_options *options);

/*!
 * @brief Deinitialize and destroy logger instance
 *
 * @param[in]   inst    Logger instance to destroy
 * */
void logger_destroy(struct logger *inst);

/*!
 * @brief Add logger stream output to logger instance
 *
 * @warning Note that this function is reentrant, but not thread-safe!
 *
 * @param[in]   inst    Logger instance
 * @param[in]   stream  Logger stream output instance to add
 * */
void logger_add_stream(struct logger *inst, struct logger_stream *stream);

/*!
 * @brief Remove logger stream output from logger instance
 *
 * @warning Note that this function is reentrant, but not thread-safe!
 *
 * @param[in]   inst    Logger instance
 * @param[in]   stream  Logger stream output instance to remove
 * */
void logger_remove_stream(struct logger *inst, struct logger_stream *stream);

/*!
 * @brief Set logger options
 *
 * @param[in]   inst    Logger instance
 * @param[in]   options Options for logger. NULL for default values
 * */
void logger_set_options(struct logger *inst, union logger_options *options);

/*!
 * @brief Set logging level
 *
 * @param[in]   inst    Logger instance
 * @param[in]   level   Level to be set in the logger instance
 * */
void logger_set_level(struct logger* inst, unsigned level);

/*!
 * @brief Get logger options
 *
 * @param[in]   inst    Logger instance
 * @param[out]  options Get logger options
 * */
void logger_get_options(struct logger *inst, union logger_options *options);

/*!
 * @brief Check if message is to be logged
 *
 * @param[in]   inst    Logger instance
 * @param[in]   level   Checked level
 * @return      true if message is to be logged
 * */
int logger_is_loggable(struct logger* inst, unsigned level);

/*!
 * @brief Get logger name (tag)
 *
 * @param[in]   inst    Logger instance
 * @return      logger name (tag)
 */
const char* logger_get_tag(struct logger* inst);

/*!
 * @brief Write log. Low level function. Please use log_write instead
 * #_log_write
 *
 * @param[in]   inst            Logger instance
 * @param[in]   logger_name     Logger name to be shown in the log
 * @param[in]   level           Logger level
 * @param[in]   file_name       File name from log was executed
 * @param[in]   function_name   Function name from log was executed
 * @param[in]   line_number     File line number from log was executed
 * @param[in]   message         Message string
 * */
void _log_write(
        struct logger *inst,
        const char* logger_name,
        const unsigned int level,
        const char *file_name,
        const char *function_name,
        const unsigned int line_number,
        const char *message);

/*!
 * @brief Write log. Low level function. Please use log_write instead
 * #_log_write
 *
 * @param[in]   inst            Logger instance
 * @param[in]   logger_name     Logger name to be shown in the log
 * @param[in]   level           Logger level
 * @param[in]   file_name       File name from log was executed
 * @param[in]   function_name   Function name from log was executed
 * @param[in]   line_number     File line number from log was executed
 * @param[in]   fmt             Format string like in printf
 * @param[in]   ...             Variadic variables like in printf
 * */
void _log_fwrite(
        struct logger *inst,
        const char* logger_name,
        const unsigned int level,
        const char *file_name,
        const char *function_name,
        const unsigned int line_number,
        const char *fmt, ...) LOGGER_PRINTF_FORMAT(7, 8);

/*!
 * @brief Write log. Low level function. Please use log_write instead
 * #_log_write. Variadic variables version
 *
 * @param[in]   inst            Logger instance
 * @param[in]   logger_name     Logger name to be shown in the log
 * @param[in]   level           Logger level
 * @param[in]   file_name       File name from log was executed
 * @param[in]   function_name   Function name from log was executed
 * @param[in]   line_number     File line number from log was executed
 * @param[in]   fmt             Format string like in printf
 * @param[in]   args            Variadic variables like in vprintf
 * */
void _log_vwrite(
        struct logger *inst,
        const char* logger_name,
        const unsigned int level,
        const char *file_name,
        const char *function_name,
        const unsigned int line_number,
        const char *fmt, va_list args) LOGGER_PRINTF_FORMAT(7, 0);

#ifdef __cplusplus
} /* extern "C" */
#endif

#ifndef LOGGER_CPP_OVER_LOGGER_C_MACROS

#if defined(LOGGER_DISABLED)
#define log_write(inst, level, stream)
#else

/*!
 * @def LOGGER_FILE_NAME
 * Generate file name for log_* functions called in that file
 *
 * @def LOGGER_FUNCTION_NAME
 * Generate function name for log_* functions called in that function
 *
 * @def LOGGER_LINE_NUMBER
 * Generate line number for log_* functions called in that file
 */

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
 * @brief Write log
 *
 * @param[in]   inst  Logger instance
 * @param[in]   lvl   Logger level
 * @param[in]   ...   Format and variadic variables like in printf
 * */
#define log_write(inst, lvl, ...) \
    do {\
        struct logger* __inst = (inst);\
        unsigned __level = (lvl);\
        if (__inst && logger_is_loggable(__inst, __level)) {\
            _log_fwrite(\
                __inst,\
                logger_get_tag(__inst),\
                __level,\
                LOGGER_FILE_NAME,\
                LOGGER_FUNCTION_NAME,\
                LOGGER_LINE_NUMBER,\
                __VA_ARGS__);\
        }\
    } while (0)

#endif /* defined(LOGGER_DISABLED) */

/*!
 * @brief Emergency message, system is about to crash or is unstable
 *
 * @param[in]       inst Logger instance
 * @param[in]       ... Variadic arguments with formatting string like printf
 * */
#define log_emergency(inst, ...)\
    log_write((inst), LOG_EMERGENCY, __VA_ARGS__)

/*!
 * @brief Something bad happened and action must be taken immediately
 *
 * @param[in]       inst Logger instance
 * @param[in]       ... Variadic arguments with formatting string like printf
 * */
#define log_alert(inst, ...)\
    log_write((inst), LOG_ALERT, __VA_ARGS__)

/*!
 * @brief A critical condition occured like serious hardware/software failure
 *
 * @param[in]       inst Logger instance
 * @param[in]       ... Variadic arguments with formatting string like printf
 * */
#define log_critical(inst, ...)\
    log_write((inst), LOG_CRITICAL, __VA_ARGS__)

/*!
 * @brief An error condition, often used by drivers to indicate diffulties
 * with the hardware
 *
 * @param[in]       inst Logger instance
 * @param[in]       ... Variadic arguments with formatting string like printf
 * */
#define log_error(inst, ...)\
    log_write((inst), LOG_ERROR, __VA_ARGS__)

/*!
 * @brief A warning, meaning nothing serious by itself but might indicate
 * problems
 *
 * @param[in]       inst Logger instance
 * @param[in]       ... Variadic arguments with formatting string like printf
 * */
#define log_warning(inst, ...)\
    log_write((inst), LOG_WARNING, __VA_ARGS__)

/*!
 * @brief Nothing serious, but notable nevertheless. Often used to report
 * security events
 *
 * @param[in]       inst Logger instance
 * @param[in]       ... Variadic arguments with formatting string like printf
 * */
#define log_notice(inst, ...)\
    log_write((inst), LOG_NOTICE, __VA_ARGS__)

/*!
 * @brief Informational message e.g. startup information
 *
 * @param[in]       inst Logger instance
 * @param[in]       ... Variadic arguments with formatting string like printf
 * */
#define log_info(inst, ...)\
    log_write((inst), LOG_INFO, __VA_ARGS__)

/*!
 * @brief Debug messages
 *
 * @param[in]       inst Logger instance
 * @param[in]       ... Variadic arguments with formatting string like printf
 * */
#define log_debug(inst, ...)\
    log_write((inst), LOG_DEBUG, __VA_ARGS__)

#endif /* LOGGER_CPP_OVER_LOGGER_C_MACROS */

#endif /* LOGGER_H */
