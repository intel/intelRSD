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
 * @file logger_factory.cpp
 */

#include "logger/logger_factory.hpp"
#include "logger/logger.h"
#include <csignal>
#include <mutex>

extern "C" {

/*!
 * @brief Demangled version for getting logger instance from C code
 * It is called from execute_logger_command()
 * @param name name of the logger
 * @return logger instance struct
 */
struct logger* logger_get_logger_by_name(const char* name);

}

/*!
 * @brief Set logging level for main_logger
 * @param sig SIGUSR1 for DEBUGs, SIGUSR2 for INFOs
 */
static void logger_handle_usr_signal(int sig) {
    logger_cpp::Level level;
    for (auto logger: logger_cpp::LoggerFactory::instance().get_loggers()) {
        switch (sig) {
            case SIGUSR1:
                level = logger_cpp::Level::DEBUG;
                break;
            case SIGUSR2:
                level = logger->get_options().get_default_level();
                break;
            default:
                return;
        }
        logger_set_level(logger->get_instance(), static_cast<unsigned>(level));
    }
}

struct logger* logger_get_logger_by_name(const char* name) {
    logger_cpp::LoggerSPtr logger = logger_cpp::LoggerFactory::instance().get_logger(name, true);
    if (logger) {
        return logger->get_instance();
    }
    return nullptr;
}

namespace logger_cpp {

const char* LoggerFactory::g_main_logger_name = nullptr;

LoggerFactory& LoggerFactory::instance() {
    static LoggerFactory factory{};
    return factory;
}

std::vector<LoggerSPtr> LoggerFactory::get_loggers() const {
    std::vector<LoggerSPtr> loggers;
    loggers.emplace_back(global_logger());
    for (auto logger: m_loggers) {
        loggers.emplace_back(logger.second);
    }
    return loggers;
}

void LoggerFactory::set_loggers(const LoggerFactory::loggers_t& loggers) {
    m_loggers = loggers;
}


LoggerSPtr LoggerFactory::get_logger(const char* name, bool only_defined) const {
    if (!name) {
        return global_logger();
    }
    std::string logger(name);
    return get_logger(logger, only_defined);
}


LoggerSPtr LoggerFactory::get_logger(const std::string& name, bool only_defined) const {
    if (name == GLOBAL_LOGGER_NAME) {
        return global_logger();
    }

    const auto it = m_loggers.find(name);
    if (it != m_loggers.cend()) {
        return it->second;
    }

    // logger isn't defined
    if (only_defined) {
        return nullptr;
    }

    const auto main_it = m_loggers.find(LoggerFactory::get_main_logger_name());
    if (main_it != m_loggers.cend()) {
        return main_it->second;
    }

    return LoggerFactory::global_logger();
}

LoggerSPtr LoggerFactory::get_logger(LoggerSPtr logger) const {
    return logger;
}

LoggerFactory::LoggerFactory() :
    m_loggers{},
    m_global_stream{std::make_shared<Stream>(Stream::Type::DIRECT, GLOBAL_LOGGER_NAME)},
    m_global_logger{std::make_shared<Logger>(GLOBAL_LOGGER_NAME)} {
    m_global_logger->add_stream(m_global_stream);
    // handle signals for changing log level (DEBUG/INFO)
    // previous handler is ignored (by default it is Term).
    std::signal(SIGUSR1, logger_handle_usr_signal);
    std::signal(SIGUSR2, logger_handle_usr_signal);
}

LoggerFactory::~LoggerFactory() {
    m_global_logger.reset();
    for(auto& logger : m_loggers) {
        /* Explicit remove loggers */
        logger.second.reset();
    }
    m_loggers.clear();
}

StreamSPtr LoggerFactory::global_stream() {
    return instance().m_global_stream;
}

LoggerSPtr LoggerFactory::global_logger() {
    return instance().m_global_logger;
}

void LoggerFactory::set_main_logger_name(const char* name) {
    g_main_logger_name = name;
}

const char* LoggerFactory::get_main_logger_name() {
    return !g_main_logger_name ? GLOBAL_LOGGER_NAME : g_main_logger_name;
}

}
