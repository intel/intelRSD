/*!
 * @brief Logger stream implementation
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
 * @file logger_stream.cpp
 */

#include "logger/stream.hpp"

extern "C" {
#include "logger/stream.h"
#include "logger/logger.h"
}

namespace logger_cpp {

Stream::Stream(Stream::Type type, const char* tag, const Options& options) {

    auto logtype = static_cast<enum logger_stream_type>(type);

    union logger_options opt;
    opt.raw = options.m_raw;
    m_impl = logger_stream_create(logtype, tag, &opt);
}

Stream::~Stream() {
    logger_stream_destroy(static_cast<struct logger_stream*>(m_impl));
}

void Stream::start() {
    logger_stream_start(static_cast<struct logger_stream*>(m_impl));
}

void Stream::stop() {
    logger_stream_stop(static_cast<struct logger_stream*>(m_impl));
}

void Stream::open_file(const char* file_name) {
    logger_stream_open_file(static_cast<struct logger_stream*>(m_impl),
            file_name);
}

void Stream::open_udp(const char* ip_address, uint16_t port) {
    logger_stream_open_udp(static_cast<struct logger_stream*>(m_impl),
            ip_address, port);
}

void Stream::open_tcp(const char* ip_address, uint16_t port) {
    logger_stream_open_tcp(static_cast<struct logger_stream*>(m_impl),
            ip_address, port);
}

void Stream::set_options(const Options& options) {
    union logger_options opt;
    opt.raw = options.m_raw;
    logger_stream_set_options(static_cast<struct logger_stream*>(m_impl), &opt);
}

Options Stream::get_options() {
    union logger_options opt;
    logger_stream_get_options(
            static_cast<struct logger_stream*>(m_impl), &opt);
    Options options;
    options.m_raw = opt.raw;
    return options;
}

}
