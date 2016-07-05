/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION
 *
 * */

#ifndef PSME_LOGGER_FACTORY_HPP
#define PSME_LOGGER_FACTORY_HPP

#include "logger/stream.hpp"
#include "logger/logger.hpp"

#include <unordered_map>

#define LOGUSR \
    logger_cpp::LoggerFactory::instance().get_logger(( \
                logger_cpp::LoggerFactory::get_main_logger_name()))

#define GET_LOGGER(name) \
    logger_cpp::LoggerFactory::instance().get_logger((name))

namespace json { class Value; }

namespace logger_cpp {

/*!
 * @brief The LoggerFactory class manage all loggers in application
 */
class LoggerFactory {
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
     * @brief get_logger Return logger object for given name
     * @param[in] name Logger name
     * @return If name exists in loggers map, module logger is returned,
     * otherwise global logger is returned
     */
    Logger* get_logger(const std::string& name) const;

    /*!
     * @brief global_logger Get global logger object
     * @return Global logger object for application
     */
    static Logger* global_logger();

    /*!
     * @brief Get global stream object
     * @return Global stream object for application
     */
    static StreamSPtr global_stream();

    /*!
     * @brief cleanup Clean LoggerFactory objects
     */
    static void cleanup();

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
    static std::string get_main_logger_name();
private:
    LoggerFactory();
    ~LoggerFactory() = default;
    LoggerFactory(const LoggerFactory&) = delete;
    LoggerFactory(const LoggerFactory&&) = delete;

    loggers_t m_loggers;

    static LoggerFactory* g_logger_factory;
    static Logger* g_global_logger;
    static StreamSPtr* g_global_stream;

    static const char* g_main_logger_name;
};

}

#endif /* PSME_LOGGER_FACTORY_HPP  */
