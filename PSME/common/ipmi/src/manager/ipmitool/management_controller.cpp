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
 * @file /ipmi/ipmitool/management_controller.cpp
 *
 * @brief Implementation of IPMI interface using ipmitool application source
 * code. Sends synchronus messages to MC.
 * */
#include "ipmi/manager/ipmitool/management_controller.hpp"
#include "ipmi/request.hpp"
#include "ipmi/response.hpp"

extern "C" {
    #include <ipmitool/ipmi_sol.h>
    #include <ipmitool/ipmi_intf.h>
}

#include <cstdint>
#include <mutex>
#include <stdexcept>
#include <thread>

using namespace ipmi::manager::ipmitool;
static constexpr uint8_t CFG_LOOUPBIT_DEFAULT        = 0x10;
static constexpr uint8_t CFG_CIPHER_SUITE_ID_DEFAULT = 0x03;
static constexpr uint8_t CFG_LUN_DEFAULT             = 0x00;
static std::mutex send_mutex;

namespace {

    /*!
     * @brief Unpacks buffer to DTO.
     */
    void process_response(const ipmi_rs* rsp, ipmi::Response& response) {
        vector<uint8_t> data_buffer{};
        data_buffer.push_back(rsp->ccode);

        for (int index = 0; index < rsp->data_len; index++) {
            data_buffer.push_back(rsp->data[index]);
        }
        response.unpack(data_buffer);
    }

    /*!
     * @brief Packs DTO to buffer.
     */
    void process_request(ipmi_rq& req,
                         std::vector<std::uint8_t>& data,
                         const ipmi::Request& request) {
        req.msg.netfn    = 0x3F & uint8_t(request.get_network_function());
        req.msg.cmd      = uint8_t(request.get_command());
        req.msg.data_len = uint16_t(data.size());
        req.msg.lun      = CFG_LUN_DEFAULT;
        req.msg.data     = data.data();

    }

    /*!
     * Returns pointer to IPMI interface to be managed by smart pointers.
     */
    inline ipmi_intf* create_interface() {
        return ipmi_intf_load(const_cast<char*>("lan"));
    }

    /*!
     * Cleans up interface pointer allocated by IPMItool.
     * Used in exit destructor and inside disconnect function.
     */
    void delete_interface(ipmi_intf* interface) {
        ipmi_cleanup(interface);

        if ((interface->opened > 0) && (nullptr != interface->close)) {
            interface->close(interface);
        }
    }
}

void ManagementController::init_interface(ipmi_intf* interface) {
    using std::vector;

    // Agnostic method to convert std::string to mutable char*.
    vector<char> hostname(get_ip().begin(), get_ip().end());
    hostname.push_back('\0');

    vector<char> username(get_username().begin(), get_username().end());
    username.push_back('\0');

    vector<char> password(get_password().begin(), get_password().end());
    password.push_back('\0');

    ipmi_intf_session_set_hostname(interface, &hostname[0]);
    ipmi_intf_session_set_username(interface, &username[0]);
    ipmi_intf_session_set_password(interface, &password[0]);
    ipmi_intf_session_set_port(interface, int(get_port()));

    ipmi_intf_session_set_lookupbit(interface, CFG_LOOUPBIT_DEFAULT);
    ipmi_intf_session_set_sol_escape_char(interface,
                                          SOL_ESCAPE_CHARACTER_DEFAULT);
    ipmi_intf_session_set_cipher_suite_id(interface,
                                          CFG_CIPHER_SUITE_ID_DEFAULT);
    ipmi_intf_session_set_timeout(interface, m_timeout_seconds);
    ipmi_intf_session_set_retry(interface, m_retry_no);

    interface->my_addr = IPMI_BMC_SLAVE_ADDR;
    interface->devnum = 0;
    interface->devfile = nullptr;
}

void
ManagementController::send(const Request& request, Response& response) {
    using namespace std;
    lock_guard<mutex> lock(send_mutex);

    using IpmiInterface = std::unique_ptr<ipmi_intf, void(*)(ipmi_intf*)>;
    IpmiInterface interface(create_interface(), delete_interface);
    init_interface(interface.get());

    vector<uint8_t> data_buffer{};
    ipmi_rq req {{ 0, 0, 0, 0, 0, nullptr }};
    request.pack(data_buffer);

    process_request(req, data_buffer, request);

    if (!interface) {
        throw runtime_error("IPMI interface pointer is null.");
    }

    ipmi_rs* rsp = interface->sendrecv(interface.get(), &req);

    if (!rsp) {
        throw runtime_error("Received null response from IPMI.");
    }

    process_response(rsp, response);
}
