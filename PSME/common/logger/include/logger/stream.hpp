/*!
 * @brief Logger stream C++ interface
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
 * @file stream.hpp
 */

#pragma once

#include "logger/logger_options.hpp"
#include "logger/logger.hpp"

namespace logger_cpp {

/*!
 * @class logger_cpp::Stream
 * @brief Stream object that receive log messages from #logger_cpp::Logger.
 * It will formatting and write log messages to output
 * */
class Stream {
private:
    void* m_impl = nullptr;
    friend class Logger;
    Stream(Stream&&) = delete;
    Stream(const Stream&) = delete;
    Stream& operator=(const Stream&) = delete;
public:
    /*!
     * @enum Type
     * @brief Stream types
     * */
    typedef enum class Type {
        STDOUT    = 0,
        STDERR    = 1,
        FILE      = 2,
        UDP       = 3,
        TCP       = 4,
        DIRECT    = 5
    } Type;

    /*!
     * @brief Default constructor. Create specific stream object given by first
     * argument, optional set tag string and override stream options. It will
     * create and start stream thread
     *
     * @param[in]   type    Stream type
     * @param[in]   tag     Tag string for tagging
     * @param[in]   options Stream options to set
     * */
    Stream(Type type, const char* tag = nullptr,
            const Options& options = Options());

    /*!
     * @brief Flush all remaining data in the stream to output, stop and close
     * nicely thread, close stream and destroy stream object instance
     * */
    ~Stream();

    /*!
     * @brief Start Stream thread
     * */
    void start();

    /*!
     * @brief Stop Stream thread
     * */
    void stop();

    /*!
     * @brief Open file with given file name path. Stream type must match to
     * file type, otherwise it will now work
     *
     * @param[in]   file_name   File name path
     * */
    void open_file(const char* file_name);

    /*!
     * @brief Open IP UDP socket with given IP address and UDP port.
     * Stream type must match to UDP type, otherwise it will now work
     *
     * @param[in]   ip_address  IP address like "127.0.0.1"
     * @param[in]   port        UDP port
     * */
    void open_udp(const char* ip_address, uint16_t port);

    /*!
     * @brief Open IP TCP socket with given IP address and TCP port.
     * Stream type must match to TCP type, otherwise it will now work
     *
     * @param[in]   ip_address  IP address like "127.0.0.1"
     * @param[in]   port        TCP port
     * */
    void open_tcp(const char* ip_address, uint16_t port);

    /*!
     * @brief Set stream options @see logger_cpp::Options
     *
     * @param[in]   options Stream options, if nullptr set default options,
     *              also call method without argument set to default
     * */
    void set_options(const Options& options);

    /*!
     * @brief Get stream options @see logger_cpp::Options
     *
     * @return      Stream options
     * */
    Options get_options();
};

using StreamSPtr = std::shared_ptr<Stream>;
using StreamUPtr = std::unique_ptr<Stream>;

}

