/*!
 * @brief Generic IPMI controller
 *
 * Generic "interface" class. It should be used as a reference to "real" controller.
 * Handle all passed requests. Connection data is to be passed as shared ptr,
 * no references are implemented (memory management actually is to be done in
 * the code)
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file .hpp
 */

#include "ipmi/ipmi_controller.hpp"
#include "ipmi/ipmi_interface_factory.hpp"
#include "ipmi/request.hpp"
#include "ipmi/response.hpp"

using namespace ipmi;

IpmiController::IpmiController(ConnectionData::Ptr _connection_data) :
    connection_data(_connection_data) { }

IpmiController::~IpmiController() {
    close();
}

void IpmiController::send(const Request& request, Response& response) {
    static const BridgeInfo default_bridge{};
    do_send(request, default_bridge, response);
}

void IpmiController::send(const Request& request, const BridgeInfo& via, Response& response) {
    do_send(request, via, response);
}

void IpmiController::send_unlocked(const Request& request, Response& response) {
    static const BridgeInfo default_bridge{};
    send_unlocked(request, default_bridge, response);
}

void IpmiController::send_unlocked(const Request& request, const BridgeInfo& via, Response& response) {
    /*
     * A vanilla controller will not have an interface prior to a lock() call. No runtime interface check and refresh
     * can be made, because the interface needs to be unlocked. Therefore the send_unlocked may fail indefinitely.
     */
    if (!interface) {
        throw std::runtime_error("Trying to perform an unlocked send without calling lock() first");
    }

    IpmiInterface::ByteBuffer resp_buffer{};
    const auto req_buffer = request.do_pack();
    interface->send_unlocked(request.get_network_function(), request.get_command(), request.get_lun(),
        via, req_buffer, resp_buffer);
    response.do_unpack(resp_buffer);
}

void IpmiController::do_send(const Request& request, const BridgeInfo& via, Response& response) {
    /*
     * if exception is thrown, interface is rechecked on next request.
     * No validation if connection data is correct is done here. If dumb args are passed,
     * it will try forever.
     * Appropriate exception should be thrown if (temporarily) connection cannot be made
     * and handling should be done
     */
    check_interface_refresh();
    try {
        IpmiInterface::ByteBuffer resp_buffer{};
        const auto req_buffer = request.do_pack();
        interface->send(request.get_network_function(), request.get_command(), request.get_lun(),
            via, req_buffer, resp_buffer);
        response.do_unpack(resp_buffer);
    }
    catch (...) {
        check_interface = true;
        throw;
    }
}

void IpmiController::lock() {
    check_interface_refresh();
    interface->lock();
}

void IpmiController::unlock() {
    if (interface) {
        interface->unlock();
    }
}

void IpmiController::check_interface_refresh() {
    if (check_interface) {
        /* if "primary" data was changed, new interface is to be get */
        if (interface && (!interface->matches(data()))) {
            IpmiInterfaceFactory::get_instance().release_interface(std::move(interface));
        }
        /* get new interface */
        if (!interface) {
            interface = IpmiInterfaceFactory::get_instance().get_interface(data());
            if (!interface) {
                throw std::runtime_error("Interface " + data()->get_interface_type() + " cannot be built!");
            }
        }
        /* Interface matches the connection data, config settings for both must match */
        if (!interface->config_equals(data())) {
            IpmiInterfaceFactory::get_instance().release_interface(std::move(interface));
            throw std::runtime_error("Interface " + data()->get_interface_type() + " with different config!");
        }
        check_interface = false;
    }
}

void IpmiController::close() {
    IpmiInterfaceFactory::get_instance().release_interface(std::move(interface));
    check_interface = true;
}
