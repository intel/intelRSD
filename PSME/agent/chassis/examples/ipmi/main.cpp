/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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
 * @file main.cpp
 *
 * @brief Example of IPMI lib use.
 * */
#include "ipmi/manager/ipmitool/management_controller.hpp"
#include "ipmi/command/generic/get_device_id.hpp"
#include "ipmi/command/sdv/get_node_info.hpp"
#include "ipmi/command/sdv/get_bios_version.hpp"
#include "ipmi/command/sdv/get_dimm_info.hpp"

#include <iostream>
#include <string>

using namespace std;
using namespace ipmi;
using namespace ipmi::manager;
using namespace ipmi::command;

int main(int argc, char** argv) {

    constexpr int ARGS_NO = 3;
    ipmitool::ManagementController mc{};

    if (argc < ARGS_NO) {
        cerr << "error: to few arguments." << endl;
        cerr << "usage: " << argv[0] << " <ip_address> <port_number>" << endl;
        return -1;
    }

    mc.set_ip(argv[1]);
    mc.set_port(std::uint32_t(std::stoul(argv[2])));
    mc.set_username("TEST");
    mc.set_password("TEST");

    generic::request::GetDeviceId device_req;
    generic::response::GetDeviceId device_rsp;

    try {
        cout << "Started." << endl;

        cout << "Before send." << endl;
        cout << "Get Device ID." << endl;
        try {
            mc.send(device_req, device_rsp);
            cout << "FW Version: " << device_rsp.get_firmware_version() << endl;
            cout << "Product ID: " << uint32_t(device_rsp.get_product_id()) << endl;
        }
        catch (const ipmi::ResponseError& error) {
            cout << "Completion code: " << uint32_t(error.get_completion_code()) << endl;
        }

        sdv::request::GetBiosVersion bios_req;
        sdv::response::GetBiosVersion bios_rsp;
        cout << "Get BIOS Version." << endl;
        try {
            mc.send(bios_req, bios_rsp);
            cout << "BIOS version: " << bios_rsp.get_version() << endl;
        }
        catch (const ipmi::ResponseError& error) {
            cout << "Completion code: " << uint32_t(error.get_completion_code()) << endl;
        }


        sdv::request::GetDimmInfo dimm_req;
        sdv::response::GetDimmInfo dimm_rsp;

        // Sets DIMM slot number.
        dimm_req.set_dimm_index(1);

        cout << "Get DIMM Info." << endl;
        try {
            mc.send(dimm_req, dimm_rsp);
            cout << "Speed [MHz]: " << dimm_rsp.get_dimm_speed_mhz() << endl;
            cout << "Size [MiB]: " << dimm_rsp.get_dimm_size_mbytes() << endl;
            cout << "Voltage [V]: " << dimm_rsp.get_dimm_voltage() << endl;
            cout << "Type: " << dimm_rsp.get_dimm_type() << endl;
        }
        catch (const ipmi::ResponseError& error) {
            cout << "Completion code: " << uint32_t(error.get_completion_code()) << endl;
        }

        sdv::request::GetNodeInfo node_req;
        sdv::response::GetNodeInfo node_rsp;
        try {
            mc.send(node_req, node_rsp);
            cout << "Number of nodes avaible: " << uint32_t(node_rsp.get_nodes_number()) << endl;
        }
        catch (const ipmi::ResponseError& error) {
            cout << "Completion code: " << uint32_t(error.get_completion_code()) << endl;
        }
    } catch (runtime_error & error) {
        cout << "error: " << error.what() << endl;
    }

    cout << "Done." << endl;
    return 0;
}
