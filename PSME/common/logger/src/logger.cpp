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
 * @file logger.cpp
 *
 * @brief Logger C++ interface
 * */

#include "logger/logger.hpp"
#include "logger/stream.hpp"

extern "C" {
#include "logger/logger.h"
}

using namespace logger_cpp;

/*
 * level = LOG_DEBUG,
 * time_format = LOG_TIME_DATE_NS,
 * color = false,
 * tagging = true,
 * more_debug = true,
 * output_enable = true
 * This default options store in int value give number 0x3a7
 * */
static constexpr unsigned int OPTION_DEFAULT = 0x3a7;

Options::Options() : m_raw(OPTION_DEFAULT) {}

void Options::set_level(enum Level level) {
    union logger_options options;
    options.raw = m_raw;
    options.option.level = LOG_LEVEL_MASK & static_cast<unsigned int>(level);
    m_raw = options.raw;
}

void Options::set_time_format(enum TimeFormat time_format) {
    union logger_options options;
    options.raw = m_raw;
    options.option.time_format =
        LOG_TIME_MASK & static_cast<unsigned int>(time_format);
    m_raw = options.raw;
}

void Options::enable_color(bool enable) {
    union logger_options options;
    options.raw = m_raw;
    options.option.color = enable;
    m_raw = options.raw;
}

void Options::enable_output(bool enable) {
    union logger_options options;
    options.raw = m_raw;
    options.option.output_enable = enable;
    m_raw = options.raw;
}

void Options::enable_tagging(bool enable) {
    union logger_options options;
    options.raw = m_raw;
    options.option.tagging = enable;
    m_raw = options.raw;
}

void Options::enable_more_debug(bool enable) {
    union logger_options options;
    options.raw = m_raw;
    options.option.more_debug = enable;
    m_raw = options.raw;
}

Logger::Logger(const char* tag, const Options& options) : m_streams{} {
    union logger_options opt;
    opt.raw = options.m_raw;
    m_impl = logger_create(tag, &opt);
}

Logger::~Logger() {
    logger_destroy(static_cast<struct logger*>(m_impl));
}

void Logger::add_stream(const std::shared_ptr<Stream> stream) {
    if (nullptr == stream) {
        return;
    }

    logger_add_stream(
            static_cast<struct logger*>(m_impl),
            static_cast<struct logger_stream*>(stream->m_impl));

    m_streams.push_back(stream);
}

void Logger::add_stream(Stream* stream) {
    if (nullptr == stream) {
        return;
    }

    logger_add_stream(
            static_cast<struct logger*>(m_impl),
            static_cast<struct logger_stream*>(stream->m_impl));
}

void Logger::remove_stream(const std::shared_ptr<Stream> stream) {
    if (nullptr == stream) {
        return;
    }

    logger_remove_stream(
            static_cast<struct logger*>(m_impl),
            static_cast<struct logger_stream*>(stream->m_impl));

    m_streams.remove(stream);
}

void Logger::write(enum Level level,
        const char* file_name,
        const char* function_name,
        unsigned int line_number,
        const std::string& str) {
    _log_write(
            static_cast<struct logger*>(m_impl),
            static_cast<unsigned int>(level),
            file_name, function_name, line_number, str.c_str()
        );
}

void Logger::set_options(const Options& options) {
    union logger_options opt;
    opt.raw = options.m_raw;
    logger_set_options(static_cast<struct logger*>(m_impl), &opt);
}

Options Logger::get_options() {
    union logger_options opt;
    logger_get_options(static_cast<struct logger*>(m_impl), &opt);
    Options options;
    options.m_raw = opt.raw;
    return options;
}
