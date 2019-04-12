/*!
 * @copyright Copyright (c) 2018-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file connectors/unix_domain_socket_client_connector.cpp
 */

/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    unixdomainsocketclient.cpp
 * @date    11.05.2015
 * @author  Alexandre Poirot <alexandre.poirot@legrand.fr>
 * @license See attached LICENSE.txt
 ************************************************************************/


#include "json-rpc/connectors/unix_domain_socket_client_connector.hpp"
#include "json-rpc/common.hpp"

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <safe-string/safe_lib.hpp>



namespace {

constexpr const char DEFAULT_DELIMITER_CHAR = char(0x0A);
constexpr const int DEFAULT_BUFFER_SIZE = 1024;
constexpr const int MAX_PATH_SIZE = 107;


bool write_request(const std::string& source, int fd) {
    ssize_t bytes_written;
    size_t total_size = source.size();
    size_t remaining_size = total_size;

    do {
        bytes_written = write(fd, source.c_str() + (total_size - remaining_size), remaining_size);
        if (bytes_written < 0) {
            return false;
        }
        else {
            remaining_size -= static_cast<size_t>(bytes_written);
        }
    }
    while (remaining_size > 0);
    return true;
}


bool read_response(std::string& target, int fd, char delimiter) {

    ssize_t bytes_read;
    char buffer[DEFAULT_BUFFER_SIZE];

    do {
        bytes_read = read(fd, buffer, DEFAULT_BUFFER_SIZE);
        if (bytes_read < 0) {
            return false;
        }
        else {
            target.append(buffer, static_cast<size_t>(bytes_read));
        }
    }
    while (target.find(delimiter) == std::string::npos && bytes_read > 0);

    target.pop_back();
    return true;
}

}

namespace json_rpc {

UnixDomainSocketClientConnector::UnixDomainSocketClientConnector(const std::string& path)
    : m_path(path) {
}


json_rpc::UnixDomainSocketClientConnector::~UnixDomainSocketClientConnector() {
}


std::string UnixDomainSocketClientConnector::send_request(const std::string& message) {

    sockaddr_un address{};
    std::string result{};

    int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket_fd < 0) {

        throw JsonRpcException(json_rpc::common::ERROR_CLIENT_CONNECTOR,
                               "Unable to initialize socket client connector");
    }

    memset(&address, 0, sizeof(sockaddr_un));
    address.sun_family = AF_UNIX;
    strncpy_s(address.sun_path, MAX_PATH_SIZE, this->m_path.c_str(), MAX_PATH_SIZE);

    try {
        if (connect(socket_fd, (struct sockaddr*) &address, sizeof(sockaddr_un)) != 0) {
            throw JsonRpcException(json_rpc::common::ERROR_CLIENT_CONNECTOR,
                                   "Could not connect to: " + this->m_path);
        }

        std::string to_send = message + DEFAULT_DELIMITER_CHAR;
        if (!write_request(to_send, socket_fd)) {
            throw JsonRpcException(json_rpc::common::ERROR_CLIENT_CONNECTOR,
                                   "Could not write request");
        }

        if (!read_response(result, socket_fd, DEFAULT_DELIMITER_CHAR)) {
            throw JsonRpcException(json_rpc::common::ERROR_CLIENT_CONNECTOR,
                                   "Could not read response");
        }
        close(socket_fd);
    }
    catch (const JsonRpcException&) {
        close(socket_fd);
        throw;
    }

    return result;
}
}






