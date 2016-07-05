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
 * */

#ifndef AGENT_COMPUTE_TESTS_MOCK_MANAGEMENT_CONTROLLER_HPP
#define	AGENT_COMPUTE_TESTS_MOCK_MANAGEMENT_CONTROLLER_HPP

#include "ipmi/management_controller.hpp"
#include "ipmi/request.hpp"
#include "ipmi/response.hpp"
#include "ipmi/command/generic/get_device_id.hpp"
#include "ipmi/command/sdv/get_bios_version.hpp"
#include "ipmi/command/sdv/get_processor_info.hpp"
#include "ipmi/command/sdv/get_dimm_info.hpp"
#include "ipmi/command/sdv/get_system_mac_address_haswell.hpp"
#include "mock_const.hpp"

/*! Checks request type and return filled response object with fake data. */
class MockManagementController: public ipmi::ManagementController {
public:
    /*! Default constructor */
    MockManagementController() = default;
    /*! Default destrutor */
    virtual ~MockManagementController();

    virtual void send(const ipmi::Request& request, ipmi::Response& response) {
        try {
	    send(dynamic_cast<const ipmi::command::generic::request::GetDeviceId&>(request),
                 dynamic_cast<ipmi::command::generic::response::GetDeviceId&>(response));
            return;
        } catch (const std::bad_cast&) {}
        try {
           send(dynamic_cast<const ipmi::command::sdv::request::GetBiosVersion&>(request),
                dynamic_cast<ipmi::command::sdv::response::GetBiosVersion&>(response));
        } catch (...) {}
        try {
           send(dynamic_cast<const ipmi::command::sdv::request::GetProcessorInfo&>(request),
                dynamic_cast<ipmi::command::sdv::response::GetProcessorInfo&>(response));
        } catch (...) {}
       try {
	   send(dynamic_cast<const ipmi::command::sdv::request::GetDimmInfo&>(request),
                dynamic_cast<ipmi::command::sdv::response::GetDimmInfo&>(response));
       } catch (...) {}

       try {
           send(dynamic_cast<const ipmi::command::sdv::request::GetSystemMacAddressHaswell&>(request),
                dynamic_cast<ipmi::command::sdv::response::GetSystemMacAddressHaswell&>(response));
       } catch (...) {
       // probably error in tests
       }


    }

    virtual void send(const ipmi::command::generic::request::GetDeviceId&,
        ipmi::command::generic::response::GetDeviceId& response) {

        response.set_firmware_version(mock_const::FIRMWARE_VERSION);
        response.set_manufacturer_id(ipmi::command::generic::response::GetDeviceId::MANUFACTURER_ID_INTEL);
        response.set_product_id(ipmi::command::generic::response::GetDeviceId::PRODUCT_ID_INTEL_ATOM);
    }

    virtual void send(const ipmi::command::sdv::request::GetBiosVersion&,
        ipmi::command::sdv::response::GetBiosVersion& response) {

        response.set_version(mock_const::FULL_VERSION);
    }

    virtual void send(const ipmi::command::sdv::request::GetProcessorInfo&,
        ipmi::command::sdv::response::GetProcessorInfo& response) {

        response.set_cpu_frequency(mock_const::CPU_FREQUENCY);
        response.set_cpu_type(ipmi::command::sdv::response::GetProcessorInfo::CPU_TYPE_XEON);
        response.set_presence(true);
    }

    virtual void send(const ipmi::command::sdv::request::GetDimmInfo&,
        ipmi::command::sdv::response::GetDimmInfo& response) {

        response.set_dimm_type(mock_const::DIMM_TYPE);
        response.set_dimm_voltage(mock_const::DIMM_VOLTAGE);
        response.set_dimm_speed_mhz(mock_const::DIMM_SPEED_MHZ);
        response.set_dimm_size_mbytes(mock_const::DIMM_SIZE_MB);
        response.set_dimm_presence(true);
    }

    virtual void send(const ipmi::command::sdv::request::GetSystemMacAddressHaswell&,
        ipmi::command::sdv::response::GetSystemMacAddressHaswell& response) {

        response.set_system_mac_address(mock_const::SYSTEM_MAC_ADDRESS_HASWELL);
    }

    bool is_connected() const {
        return true;
    }
};

// To avoid vtable warning.
MockManagementController::~MockManagementController(){}

#endif	/* AGENT_COMPUTE_TESTS_MOCK_MANAGEMENT_CONTROLLER_HPP */
