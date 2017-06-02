/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @file /ipmi/ipmitool/management_controller.cpp
 *
 * @brief Implementation of IPMI interface using ipmitool application source
 * code. Sends synchronous messages to MC.
 * */
#include "ipmi/manager/ipmitool/management_controller.hpp"

extern "C" {
    #include <ipmitool/ipmi_sol.h>
    #include <ipmitool/ipmi_intf.h>
}

#include <cstdint>
#include <mutex>
#include <stdexcept>
#include <thread>

using namespace ipmi::manager::ipmitool;
static constexpr std::uint8_t CFG_LOOUPBIT_DEFAULT = 0x10;
static constexpr std::uint8_t CFG_CIPHER_SUITE_ID_DEFAULT = 0x03;
static constexpr std::uint8_t CFG_LUN_DEFAULT = 0x00;

namespace {

/*!
 * @brief Unpacks buffer to DTO.
 */
void process_response(const ipmi_rs* rsp, ipmi::Response& response) {
    std::vector<std::uint8_t> data_buffer{};
    data_buffer.push_back(rsp->ccode);

    for (int index = 0; index < rsp->data_len; index++) {
        data_buffer.push_back(rsp->data[index]);
    }
    response.unpack(data_buffer);
}

/*!
 * @brief Packs DTO to buffer.
 */
ipmi_rq create_ipmi_request(const ipmi::Request& request, std::vector<std::uint8_t>& data_buffer) {
    request.pack(data_buffer);

    ipmi_rq req{{ 0, 0, 0, 0, 0, nullptr}};
    req.msg.netfn = 0x3F & std::uint8_t(request.get_network_function());
    req.msg.cmd = std::uint8_t(request.get_command());
    req.msg.data_len = std::uint16_t(data_buffer.size());
    req.msg.lun = CFG_LUN_DEFAULT;
    req.msg.data = data_buffer.data();
    return req;
}

using IpmiInterfaceUPtr = std::unique_ptr<ipmi_intf, void(*)(ipmi_intf*)>;

/*!
 * Cleans up interface returned by IPMItool.
 * Used in exit destructor and inside disconnect function.
 */
void delete_interface(ipmi_intf* interface) {
    ipmi_cleanup(interface);

    if ((interface->opened > 0) && (nullptr != interface->close)) {
        interface->close(interface);
    }
}

void init_interface(const ManagementController& mc, ipmi_intf* interface) {
    // Agnostic method to convert std::string to mutable char*.
    std::vector<char> hostname(mc.get_ip().begin(), mc.get_ip().end());
    hostname.push_back('\0');

    std::vector<char> username(mc.get_username().begin(), mc.get_username().end());
    username.push_back('\0');

    std::vector<char> password(mc.get_password().begin(), mc.get_password().end());
    password.push_back('\0');

    ipmi_intf_session_set_hostname(interface, &hostname[0]);
    ipmi_intf_session_set_username(interface, &username[0]);
    ipmi_intf_session_set_password(interface, &password[0]);
    ipmi_intf_session_set_port(interface, int(mc.get_port()));

    ipmi_intf_session_set_lookupbit(interface, CFG_LOOUPBIT_DEFAULT);
    ipmi_intf_session_set_sol_escape_char(interface, SOL_ESCAPE_CHARACTER_DEFAULT);
    ipmi_intf_session_set_cipher_suite_id(interface, CFG_CIPHER_SUITE_ID_DEFAULT);

    interface->my_addr = IPMI_BMC_SLAVE_ADDR;
    interface->devnum = 0;
    interface->devfile = nullptr;
}

}

namespace ipmi {
namespace manager {
namespace ipmitool {

class IpmiInterfaceImpl: public IpmiInterface {
public:
    IpmiInterfaceImpl(const ManagementController& mc) {
        IpmiInterfaceUPtr interface(ipmi_intf_load(const_cast<char*>(mc.get_ipmi_interface_type().c_str())),
                                    delete_interface);
        if (!interface) {
            throw std::runtime_error("Can't create " + mc.get_ipmi_interface_type() + " IPMI interface.");
        }
        m_ipmi_intf = *interface;
        init_interface(mc, &m_ipmi_intf);
    }

    ~IpmiInterfaceImpl();

    void open() override {
        std::lock_guard<std::mutex> lock{m_mutex};
        if (m_ipmi_intf.open(&m_ipmi_intf) < 0) {
            throw std::runtime_error("Can't open IPMI connection.");
        }
    }

    void close() override {
        std::lock_guard<std::mutex> lock{m_mutex};
        ipmi_cleanup(&m_ipmi_intf);
        if ((m_ipmi_intf.opened > 0) && (nullptr != m_ipmi_intf.close)) {
            m_ipmi_intf.close(&m_ipmi_intf);
        }
    }

    void send(const Request& request, Response& response) override {
        std::vector<std::uint8_t> data_buffer{};
        auto req = create_ipmi_request(request, data_buffer);

        std::lock_guard<std::mutex> lock{m_mutex};

        ipmi_rs* rsp = m_ipmi_intf.sendrecv(&m_ipmi_intf, &req);

        if (!rsp) {
            throw std::runtime_error("Received null response from IPMI.");
        }

        process_response(rsp, response);
    }

    int send_keep_alive() override {
       std::lock_guard<std::mutex> lock{m_mutex};
       int rc = m_ipmi_intf.keepalive(&m_ipmi_intf);
       return rc;
    }

private:
    ipmi_intf m_ipmi_intf{};
    std::mutex m_mutex{};
};

IpmiInterfaceImpl::~IpmiInterfaceImpl() {
    IpmiInterfaceImpl::close();
}

IpmiInterface::UPtr create_ipmi_interface(const ManagementController& mc) {
    return IpmiInterface::UPtr(new IpmiInterfaceImpl(mc));
}

}
}
}

IpmiInterface::~IpmiInterface() {}

void ManagementController::send(const Request& request, Response& response) {
    auto interface = create_ipmi_interface(*this);

    interface->send(request, response);
}
