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
 * */

#include "logger/logger_factory.hpp"

using namespace logger_cpp;

LoggerFactory* LoggerFactory::g_logger_factory = nullptr;
Logger* LoggerFactory::g_global_logger = nullptr;
StreamSPtr* LoggerFactory::g_global_stream = nullptr;

const char* LoggerFactory::g_main_logger_name = nullptr;

LoggerFactory& LoggerFactory::instance() {
    if (!g_logger_factory) {
        g_logger_factory = new LoggerFactory{};
    }
    return *g_logger_factory;
}

void LoggerFactory::set_loggers(const LoggerFactory::loggers_t& loggers) {
    m_loggers = loggers;
}

Logger* LoggerFactory::get_logger(const std::string& name) const {
    const auto it = m_loggers.find(name);
    if (it != m_loggers.cend()) {
        return it->second.get();
    }

    const auto main_it = m_loggers.find(LoggerFactory::get_main_logger_name());
    if (main_it != m_loggers.cend()) {
        return main_it->second.get();
    }

    return LoggerFactory::global_logger();
}

LoggerFactory::LoggerFactory() : m_loggers{} {}

StreamSPtr LoggerFactory::global_stream() {
    if (!g_global_stream) {
        g_global_stream = new std::shared_ptr<Stream>(new Stream(
                                                Stream::Type::STDOUT,
                                                "stdout"));
    }
    return *g_global_stream;
}

Logger* LoggerFactory::global_logger() {
    if (!g_global_logger) {
        g_global_logger = new Logger("USR");
        g_global_logger->add_stream(LoggerFactory::global_stream());
    }
    return g_global_logger;
}

void LoggerFactory::cleanup() {
    if (g_global_stream) {
        g_global_stream->reset();
        delete g_global_stream;
        g_global_stream = nullptr;
    }

    if (g_global_logger) {
        delete g_global_logger;
        g_global_logger = nullptr;
    }
    if (g_logger_factory) {
        delete g_logger_factory;
        g_logger_factory = nullptr;
    }
}

void LoggerFactory::set_main_logger_name(const char* name) {
    g_main_logger_name = name;
}

std::string LoggerFactory::get_main_logger_name() {
    return !g_main_logger_name ? "" : std::string(g_main_logger_name);
}

