/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation
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
 * @file connectors/http_server_connector.cpp
 */

/*************************************************************************
 * libjson-rpc-cpp
 *************************************************************************
 * @file    httpserver.cpp
 * @date    31.12.2012
 * @author  Peter Spiess-Knafl <dev@spiessknafl.at>
 * @license See attached LICENSE.txt
 ************************************************************************/

#include "json-rpc/connectors/http_server_connector.hpp"
#include "json-rpc/common.hpp"

#include <arpa/inet.h>
#include <sstream>
#include <cstring>



using namespace json_rpc;

namespace {

unsigned get_mhd_flags() {
#ifdef MHD_FEATURE_EPOLL
    if (MHD_YES == MHD_is_feature_supported(MHD_FEATURE_EPOLL)) {
#ifdef MHD_USE_EPOLL_INTERNALLY
        return MHD_USE_EPOLL_INTERNALLY;
#else
        return MHD_USE_EPOLL_INTERNALLY_LINUX_ONLY;
#endif
    }
#endif
#ifdef MHD_FEATURE_POLL
    if (MHD_YES == MHD_is_feature_supported(MHD_FEATURE_POLL)) {
        return MHD_USE_POLL_INTERNALLY;
    }
#endif
    return MHD_USE_SELECT_INTERNALLY;
}


bool send_options_response(MHD_Connection* connection, int code) {

    struct MHD_Response* result = MHD_create_response_from_buffer(0, nullptr, MHD_RESPMEM_MUST_COPY);

    MHD_add_response_header(result, "Allow", "POST, OPTIONS");
    MHD_add_response_header(result, "Access-Control-Allow-Origin", "*");
    MHD_add_response_header(result, "Access-Control-Allow-Headers", "origin, content-type, accept");
    MHD_add_response_header(result, "DAV", "1");

    int ret = MHD_queue_response(connection, code, result);
    MHD_destroy_response(result);
    return ret == MHD_YES;
}


bool send_response(MHD_Connection* connection, int code, const std::string& response) {

    struct MHD_Response* result = MHD_create_response_from_buffer(response.size(),
                                                                  const_cast<char*>(response.c_str()),
                                                                  MHD_RESPMEM_MUST_COPY);

    MHD_add_response_header(result, "Content-Type", "application/json");
    MHD_add_response_header(result, "Access-Control-Allow-Origin", "*");

    int ret = MHD_queue_response(connection, code, result);
    MHD_destroy_response(result);
    return ret == MHD_YES;
}


struct ConnectionInfo {
    struct MHD_PostProcessor* post_processor{};
    MHD_Connection* connection{};
    HttpServerConnector* server{};
    std::stringstream data{};
};


int callback(void* cls, MHD_Connection* connection, const char* url, const char* method, const char*,
             const char* upload_data, size_t* upload_data_size, void** con_cls) {

    auto deleter = [&con_cls](ConnectionInfo* info) {
        delete info;
        *con_cls = nullptr;
    };
    std::unique_ptr<ConnectionInfo, decltype(deleter)> info{static_cast<ConnectionInfo*>(*con_cls), deleter};

    // handle new request
    if (!info) {
        info.reset(new ConnectionInfo());
        info->connection = connection;
        info->server = reinterpret_cast<HttpServerConnector*>(cls);
        *con_cls = info.release();
        return MHD_YES;
    }

    // handle incoming POST data
    if (std::string{"POST"} == method && 0 != *upload_data_size) {
        info->data.write(upload_data, *upload_data_size);
        *upload_data_size = 0;
        info.release();
        return MHD_YES;
    }

    // handle other requests (including POSTs without data)
    if (std::string{"POST"} == method) {
        auto handler = info->server->get_url_handler(url);
        if (handler) {
            send_response(info->connection, MHD_HTTP_OK, handler->handle(info->data.str()));
        }
        else {
            send_response(info->connection, MHD_HTTP_NOT_FOUND, "Page not found");
        }
    }
    else if (std::string{"OPTIONS"} == method) {
        send_options_response(info->connection, MHD_HTTP_OK);
    }
    else {
        send_response(info->connection, MHD_HTTP_METHOD_NOT_ALLOWED, "Not allowed HTTP Method");
    }
    return MHD_YES;
}


struct sockaddr_in make_socket_address(const std::string& ipv4_address, const std::uint16_t port) {
    struct sockaddr_in socket_address{};
    std::string dot_address = (ipv4_address == "localhost") ? "127.0.0.1" : ipv4_address;
    std::memset(&socket_address, 0, sizeof(socket_address));
    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(port);
    if (inet_pton(AF_INET, dot_address.c_str(), &(socket_address.sin_addr.s_addr)) <= 0) {
        throw JsonRpcException(common::ERROR_SERVER_CONNECTOR, "Could not convert IP address to bytes representation");
    }
    return socket_address;
}

}


HttpServerConnector::HttpServerConnector(unsigned short port, unsigned threads) :
    m_port(port), m_threads(threads), m_is_running(false), m_daemon{nullptr, &MHD_stop_daemon} {
}


HttpServerConnector::HttpServerConnector(unsigned short port, const std::string& address, unsigned threads) :
    m_port(port), m_address(address), m_threads(threads), m_is_running(false), m_daemon{nullptr, &MHD_stop_daemon} {
}


HttpServerConnector::~HttpServerConnector() {
}


bool HttpServerConnector::start_listening() {
    try {
        if (!m_is_running) {
            if (m_address.empty()) {
                m_daemon.reset(MHD_start_daemon(::get_mhd_flags(), m_port, nullptr, nullptr, callback, this,
                                                MHD_OPTION_THREAD_POOL_SIZE, m_threads,
                                                MHD_OPTION_END));
            }
            else {
                auto address = ::make_socket_address(m_address, m_port);
                m_daemon.reset(MHD_start_daemon(::get_mhd_flags(), m_port, nullptr, nullptr, callback, this,
                                                MHD_OPTION_THREAD_POOL_SIZE, m_threads,
                                                MHD_OPTION_SOCK_ADDR, &address,
                                                MHD_OPTION_END));
            }
        }
    }
    catch (const json_rpc::JsonRpcException&) {
        return false;
    }

    if (m_daemon != nullptr) {
        m_is_running = true;
        return true;
    }
    return false;
}


void HttpServerConnector::stop_listening() {
    if (m_is_running) {
        m_daemon.reset();
        m_is_running = false;
    }
}
