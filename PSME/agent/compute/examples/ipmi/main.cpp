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
 * @file main.cpp
 *
 * @brief Example of IPMI lib use.
 * */
#include "ipmi/manager/ipmitool/management_controller.hpp"
#include "ipmi/command/generic/get_device_id.hpp"
#include "ipmi/command/sdv/get_node_info.hpp"
#include "ipmi/command/sdv/get_bios_version.hpp"
#include "ipmi/command/sdv/get_dimm_info.hpp"
#include "ipmi/command/sdv/get_cable_id.hpp"

#include <iostream>
#include <string>

using namespace std;
using namespace ipmi;
using namespace ipmi::manager;
using namespace ipmi::command;

/*
 *
 */
int main(int argc, char ** argv) {

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
        mc.send(device_req, device_rsp);
        cout << "Get Device ID." << endl;
        cout << "Completion code: " << uint32_t(device_rsp.get_completion_code()) << endl;
        cout << "FW Version: " << device_rsp.get_firmware_version() << endl;
        cout << "Product ID: " << uint32_t(device_rsp.get_product_id()) << endl;

        sdv::request::GetBiosVersion bios_req;
        sdv::response::GetBiosVersion bios_rsp;
        mc.send(bios_req, bios_rsp);
        cout << "Get BIOS Version." << endl;
        cout << "Completion code: " << uint32_t(bios_rsp.get_completion_code()) << endl;
        cout << "BIOS version: " << bios_rsp.get_version() << endl;

        sdv::request::GetDimmInfo dimm_req;
        sdv::response::GetDimmInfo dimm_rsp;

        // Sets DIMM slot number.
        dimm_req.set_dimm_index(1);

        mc.send(dimm_req, dimm_rsp);
        cout << "Get DIMM Info." << endl;
        cout << "Completion code: " << uint32_t(dimm_rsp.get_completion_code()) << endl;
        cout << "Configured speed [MHz]: " << dimm_rsp.get_dimm_speed_mhz() << endl;
        cout << "Size [MiB]: " << dimm_rsp.get_dimm_size_mbytes() << endl;
        cout << "Voltage [V]: " << dimm_rsp.get_dimm_voltage() << endl;
        cout << "Type: " << dimm_rsp.get_dimm_type() << endl;

        sdv::request::GetNodeInfo node_req;
        sdv::response::GetNodeInfo node_rsp;
        mc.send(node_req, node_rsp);
        cout << "Completion code: " << uint32_t(node_rsp.get_completion_code()) << endl;
        cout << "Number of nodes available: " << uint32_t(node_rsp.get_nodes_number()) << endl;

        sdv::request::GetCableId cable_id_req;
        sdv::response::GetCableId cable_id_res;
        mc.send(cable_id_req, cable_id_res);
        cout << "Completion code: " << uint32_t(cable_id_res.get_completion_code()) << endl;
        cout << "Cable id received: " << cable_id_res.get_cable_id() << endl;
    } catch (runtime_error & error) {
        cout << "error: " << error.what() << endl;
    }

    cout << "Done." << endl;
    return 0;
}
