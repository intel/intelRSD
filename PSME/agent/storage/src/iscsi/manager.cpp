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
*/

#include "iscsi/manager.hpp"
#include "iscsi/request.hpp"
#include "iscsi/response.hpp"
#include "iscsi/socket.hpp"
#include "iscsi/errors.hpp"
#include "logger/logger_factory.hpp"
#include <stdexcept>
#include <cstring>

using namespace agent::storage::iscsi::tgt;

Response Manager::create_target(const std::int32_t target_id,
                                        const std::string& target_name) const {
    Response response;
    try {
        Socket socket;
        auto request = create_target_request(target_id, target_name);

        std::lock_guard<std::mutex> lock(m_mutex);

        socket.connect();
        socket.send(request.get_request_data());

        socket.recive(response.data(), response.get_response_pod_size());
    } catch (const std::runtime_error& e) {
        log_error(GET_LOGGER("tgt"), e.what());
    }
    return response;
}

Response Manager::create_lun(const std::int32_t target_id,
                                        const std::uint64_t lun_id,
                                        const std::string& device_path) const {
    Response response;
    try {
        Socket socket;
        auto request = create_lun_request(target_id, lun_id, device_path);

        std::lock_guard<std::mutex> lock(m_mutex);

        socket.connect();
        socket.send(request.get_request_data());

        socket.recive(response.data(), response.get_response_pod_size());
    } catch (const std::runtime_error& e) {
        log_error(GET_LOGGER("tgt"), e.what());
    }
    return response;
}

Response Manager::bind_target(const std::int32_t target_id,
                         const Manager::OptionMapper& options) const {
    Response response;
    try {
        Socket socket;
        auto request = bind_target_request(target_id, options);

        std::lock_guard<std::mutex> lock(m_mutex);

        socket.connect();
        socket.send(request.get_request_data());

        socket.recive(response.data(), response.get_response_pod_size());
    } catch (const std::runtime_error& e) {
        log_error(GET_LOGGER("tgt"), e.what());
    }
    return response;
}

Response Manager::unbind_target(const std::int32_t target_id,
                         const Manager::OptionMapper& options) const {
    Response response;
    try {
        Socket socket;
        auto request = unbind_target_request(target_id, options);

        std::lock_guard<std::mutex> lock(m_mutex);

        socket.connect();
        socket.send(request.get_request_data());

        socket.recive(response.data(), response.get_response_pod_size());
    } catch (const std::runtime_error& e) {
        log_error(GET_LOGGER("tgt"), e.what());
    }
    return response;
}

Response Manager::update_target(const std::int32_t target_id,
                         const Manager::OptionMapper& options) const {
    Response response;
    try {
        Socket socket;
        auto request = update_target_request(target_id, options);

        std::lock_guard<std::mutex> lock(m_mutex);

        socket.connect();
        socket.send(request.get_request_data());

        socket.recive(response.data(), response.get_response_pod_size());
    } catch (const std::runtime_error& e) {
        log_error(GET_LOGGER("tgt"), e.what());
    }
    return response;
}

Response Manager::destroy_target(const std::int32_t target_id) const {
    Response response;
    try {
        Socket socket;
        auto request = destroy_target_request(target_id);

        std::lock_guard<std::mutex> lock(m_mutex);

        socket.connect();
        socket.send(request.get_request_data());

        socket.recive(response.data(), response.get_response_pod_size());
    } catch (const std::runtime_error& e) {
        log_error(GET_LOGGER("tgt"), e.what());
    }
    return response;
}

Response Manager::show_targets() const {
    Response response;
    try {
        Socket socket;
        auto request = show_targets_request();

        std::lock_guard<std::mutex> lock(m_mutex);

        socket.connect();
        socket.send(request.get_request_data());

        socket.recive(response.data(), response.get_response_pod_size());
        if (response.is_valid() && response.get_length()) {
            auto& extra_data = response.get_extra_data();
            extra_data.resize(response.get_length());
            socket.read(extra_data);
        }
    } catch (const std::runtime_error& e) {
        log_error(GET_LOGGER("tgt"), e.what());
    }

    return response;
}

Request Manager::create_target_request(const std::int32_t target_id,
                                        const std::string& target_name) const {
    Request request;
    request.set_target_id(target_id);
    request.set_operation(Operation::NEW);
    request.set_mode(Mode::TARGET);
    request.set_lld("iscsi");
    request.add_extra("targetname=" + target_name);
    return request;
}

Request Manager::create_lun_request(const std::int32_t target_id,
                                    const std::uint64_t lun_id,
                                    const std::string& device_path) const {
    Request request;
    request.set_target_id(target_id);
    request.set_operation(Operation::NEW);
    request.set_mode(Mode::DEVICE);
    request.set_lld("iscsi");
    request.set_target_id(target_id);
    request.set_lun(lun_id);
    request.add_extra("path=" + device_path);
    return request;

}

Request Manager::destroy_target_request(const std::int32_t target_id) const {
    Request request;
    request.set_target_id(target_id);
    request.set_operation(Operation::DELETE);
    request.set_mode(Mode::TARGET);
    request.set_lld("iscsi");
    return request;
}

namespace {
void add_options(Request& request, const Manager::OptionMapper& options) {
    for (const auto& option : options) {
        request.add_extra(option.first + "=" + option.second);
    }
}
}

Request Manager::bind_target_request(const std::int32_t target_id,
                                   const OptionMapper& options) const {

    Request request;
    request.set_target_id(target_id);
    request.set_operation(Operation::BIND);
    request.set_mode(Mode::TARGET);
    request.set_lld("iscsi");
    add_options(request, options);
    return request;
}

Request Manager::unbind_target_request(const std::int32_t target_id,
                                   const OptionMapper& options) const {
    Request request;
    request.set_target_id(target_id);
    request.set_operation(Operation::UNBIND);
    request.set_mode(Mode::TARGET);
    request.set_lld("iscsi");
    add_options(request, options);
    return request;
}

Request Manager::update_target_request(const std::int32_t target_id,
                                   const OptionMapper& options) const {
    Request request;
    request.set_target_id(target_id);
    request.set_operation(Operation::UPDATE);
    request.set_mode(Mode::TARGET);
    request.set_lld("iscsi");
    add_options(request, options);
    return request;
}

Request Manager::show_targets_request() const {
    Request request;
    request.set_operation(Operation::SHOW);
    request.set_mode(Mode::TARGET);
    request.set_lld("iscsi");
    return request;
}
