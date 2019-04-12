/*!
 * @brief Strategy for logger configuration
 *
 * This file maps user configuration (parsed from configuration.json) to
 * logger objects.
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
 * @file logger_loader.cpp
 */
#include "agent-framework/logger_loader.hpp"
#include "agent-framework/exceptions/exception.hpp"
#include "json-wrapper/json-wrapper.hpp"

#include <algorithm>
#include <memory>
#include <iostream>



using namespace logger_cpp;

/*!
 * @brief list of logger levels
 * @todo they should match journalctl strings
 */
std::array<const char*, 8> LoggerLoader::g_level = {
    {
        "EMERGENCY",
        "ALERT",
        "CRITICAL",
        "ERROR",
        "WARNING",
        "NOTICE",
        "INFO",
        "DEBUG"
    }
};

std::array<const char*, 5> LoggerLoader::g_time_format = {
    {
        "NONE",
        "DATE_SEC",
        "DATE_MS",
        "DATE_US",
        "DATE_NS"
    }
};

std::array<const char*, 6> LoggerLoader::g_stream_type = {
    {
        "STDOUT",
        "STDERR",
        "FILE",
        "UDP",
        "TCP",
        "DIRECT"
    }
};


void LoggerLoader::load(LoggerFactory& factory) {
    try {
        if (m_config_json.count("loggers")) {
            const auto& config = m_config_json["loggers"];
            LoggerFactory::loggers_t loggers = process_loggers(config);
            factory.set_loggers(loggers);

        }
    }
    catch (const std::exception& e) {
        log_error(LOGUSR, "Cannot parse loggers settings " << e.what() << "\n");
    }
}


LoggerFactory::loggers_t LoggerLoader::process_loggers(const json::Json& loggers) {
    LoggerFactory::loggers_t loggers_map;
    bool found_default_logger = false;
    for (const auto& logger_json : loggers) {
        LoggerSPtr logger = get_logger(logger_json);
        loggers_map.emplace(logger->get_name(), logger);
        if (logger_json.value("default", false)) {
            if (!found_default_logger) {
                const std::string& main_logger_name = logger->get_name();
                LoggerFactory::set_main_logger_name(main_logger_name.c_str());
                found_default_logger = true;
            }
            else {
                const std::string& main_logger_name = LoggerFactory::get_main_logger_name();
                log_warning(LOGUSR, "Multiple default loggers. Logger \"" + main_logger_name + "\" set as default.\n");
            }
        }
    }

    if (!loggers_map.empty() && !found_default_logger) {
        const std::string& main_logger_name = loggers_map.begin()->second->get_name();
        LoggerFactory::set_main_logger_name(main_logger_name.c_str());
        if (loggers_map.size() > 1) {
            log_info(LOGUSR, "No default logger specified. Logger \"" + main_logger_name + "\" set as default.\n");
        }
    }
    return loggers_map;
}


LoggerSPtr LoggerLoader::get_logger(const json::Json& config) {
    if (!config.is_object()) {
        throw std::domain_error("Logger is not an object");
    }
    if (!config.count("name")) {
        throw std::domain_error("Logger has no name");
    }

    const std::string& name = config.value("name", std::string{});
    auto logger_ptr = std::make_shared<logger_cpp::Logger>(name);
    add_options(logger_ptr, config);
    add_streams(logger_ptr, config);
    return logger_ptr;
}


void LoggerLoader::add_streams(LoggerSPtr logger, const json::Json& config) {
    bool add_global_stream = true;
    const auto& streams = config.value("streams", json::Json());
    if (streams.is_array()) {
        add_global_stream = process_streams(logger, streams);
    }

    if (add_global_stream) {
        logger->add_stream(LoggerFactory::global_stream());
    }
}


void LoggerLoader::add_options(LoggerSPtr logger, const json::Json& config) {
    logger->set_options(get_options(config));
}


StreamSPtr LoggerLoader::get_stream(const json::Json& stream) {
    std::string type = stream.value("type", std::string{});
    int index = LoggerLoader::check_enums(g_stream_type, type);
    if (-1 != index) {
        auto type_enum = Stream::Type(index);
        auto stream_ptr = std::make_shared<Stream>(type_enum,
                                                   stream.value("tag", json::Json()).is_null() ?
                                                   g_stream_type[unsigned(index)] :
                                                   stream["tag"].get<std::string>().c_str());
        stream_ptr->set_options(get_options(stream));
        set_stream_output(type_enum, stream_ptr, stream);
        return stream_ptr;
    }
    else {
        log_error(LOGUSR, "Invalid stream type for logger!\n");
    }
    return nullptr;
}


Options LoggerLoader::get_options(const json::Json& config) {
    Options options;

    if (!config.value("level", json::Json()).is_null()) {
        int index = LoggerLoader::check_enums(g_level, config.value("level", std::string{}));
        if (-1 != index) {
            options.set_level(Level(index));
        }
    }

    if (!config.value("timeformat", json::Json()).is_null()) {
        int index = LoggerLoader::check_enums(g_time_format, config.value("timeformat", std::string{}));
        if (-1 != index) {
            options.set_time_format(TimeFormat(index));
        }
    }

    if (!config.value("color", json::Json()).is_null()) {
        options.enable_color(config.value("color", bool{}));
    }

    if (!config.value("output", json::Json()).is_null()) {
        options.enable_output(config.value("output", bool{}));
    }

    if (!config.value("tagging", json::Json()).is_null()) {
        options.enable_tagging(config.value("tagging", bool{}));
    }

    if (!config.value("moredebug", json::Json()).is_null()) {
        options.enable_more_debug(config.value("moredebug", bool{}));
    }

    return options;
}


bool LoggerLoader::process_streams(LoggerSPtr logger, const json::Json& streams) {
    bool add_global_stream = true;
    for (const auto& stream : streams) {
        auto stream_ptr = get_stream(stream);
        if (stream_ptr) {
            logger->add_stream(stream_ptr);
            stream_ptr->set_options(logger->get_options());
            add_global_stream = false;
        }
    }
    return add_global_stream;
}


void LoggerLoader::set_stream_output(Stream::Type type, StreamSPtr stream, const json::Json& config) {
    switch (type) {
        case Stream::Type::FILE: {
            if (!config.value("file", json::Json()).is_null()) {
                stream->open_file(config.value("file", std::string{}).c_str());
            }
        }
            break;

        case Stream::Type::TCP: {
            if (!config.value("address", json::Json()).is_null() && !config.value("port", json::Json()).is_null()) {
                stream->open_tcp(config.value("address", std::string{}).c_str(), config.value("port", std::uint16_t{}));
            }
        }
            break;

        case Stream::Type::UDP: {
            if (!config.value("address", json::Json()).is_null() && !config.value("port", json::Json()).is_null()) {
                stream->open_udp(config.value("address", std::string{}).c_str(), config.value("port", std::uint16_t{}));
            }
        }
            break;

        case Stream::Type::STDOUT:
            break;
        case Stream::Type::STDERR:
            break;
        case Stream::Type::DIRECT:
            break;
        default:
            break;
    }
}
