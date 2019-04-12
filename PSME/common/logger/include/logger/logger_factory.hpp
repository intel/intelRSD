/*!
 * @brief Collection of configured loggers
 *
 * Keeps mapping 'name' to 'logger' with default and "system" loggers.
 * Default one is used when requested (by name) doesn't exist. System one
 * exists always (is configuration agnostic).
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
 * @file logger_factory.hpp
 */

#pragma once
#include "logger/stream.hpp"
#include "logger/logger.hpp"

#include <unordered_map>

namespace logger_cpp {

/*!
 * @brief Name of global logger.
 *
 * Logger with that name always exists, doesn't have to be configured.
 * It is used for logging when no application (configured) logger is requested.
 */
constexpr const char* GLOBAL_LOGGER_NAME = "usr";

/*!
 * @brief The LoggerFactory class manage all loggers in application
 */
class LoggerFactory final {
public:
    /*! logger map [name, logger object] */
    using loggers_t = std::unordered_map<std::string, LoggerSPtr>;

    /*!
     * @brief Singleton pattern. Get global logger factory instance,
     * contains loggers map
     * @return LoggerFactory object
     */
    static LoggerFactory& instance();

    /*!
     * @brief set_loggers Set loggers for each application module
     * @param[in] loggers Logger map
     */
    void set_loggers(const loggers_t& loggers);

    /*!
     * @brief get all defined loggers (for application)
     * @return vector with defined loggers
     */
    std::vector<LoggerSPtr> get_loggers(void) const;

    /*!
     * @brief get_logger Return logger object for given name
     * @param[in] name Logger name, nullptr is allowed
     * @param[in] only_defined if logger isn't defined return NULL instead of app one
     * @return If name exists in loggers map, module logger is returned,
     * otherwise return global logger for NULL.
     */
    LoggerSPtr get_logger(const char* name, bool only_defined = false) const;

    /*!
     * @brief get_logger Return logger object for given name
     * @param[in] name Logger name
     * @param[in] only_defined if logger isn't defined return NULL instead of app one
     * @return If name exists in loggers map, module logger is returned,
     * otherwise application logger is returned
     */
    LoggerSPtr get_logger(const std::string& name, bool only_defined = false) const;

    /*!
     * @brief Get logger
     *
     * Dumb method to handle overloaded calls (from log_xxx() macro).
     *
     * @param logger Shared pointer to logger
     * @return logger argument
     */
    LoggerSPtr get_logger(LoggerSPtr logger) const;

    /*!
     * @brief global_logger Get global logger object
     * @return Global logger object for application
     */
    static LoggerSPtr global_logger();

    /*!
     * @brief Get global stream object
     * @return Global stream object for application
     */
    static StreamSPtr global_stream();

    /*!
     * @brief cleanup Clean LoggerFactory objects
     */
    static void cleanup() __attribute__((deprecated)) {}

    /*!
     * @brief Sets main logger name
     *
     * @param name logger name
     */
    static void set_main_logger_name(const char* name);

    /*!
     * @brief Gets main logger name
     *
     * @return logger name
     */
    static const char* get_main_logger_name();

private:
    LoggerFactory();
    ~LoggerFactory();
    LoggerFactory(const LoggerFactory&) = delete;
    LoggerFactory(const LoggerFactory&&) = delete;

    loggers_t m_loggers{};
    StreamSPtr m_global_stream{};
    LoggerSPtr m_global_logger{};
    static const char* g_main_logger_name;
};

}

