/*!
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
 * */

#pragma once

#include "ipmi/management_controller.hpp"
#include "ipmi/request.hpp"
#include "ipmi/response.hpp"
#include "ipmi/command/generic/get_device_id.hpp"
#include "ipmi/command/sdv/get_bios_version.hpp"
#include "ipmi/command/sdv/get_cable_id.hpp"
#include "ipmi/command/sdv/get_processor_info.hpp"
#include "ipmi/command/sdv/get_dimm_info.hpp"
#include "ipmi/command/sdv/get_system_mac_address_haswell.hpp"
#include "ipmi/command/sdv/get_mdr_data_region_status.hpp"
#include "mock_const.hpp"

namespace ipmi {

/*! Checks request type and return filled response object with fake data. */
class MockManagementController : public ManagementController {
public:

    enum class TestCase {
        TIMEOUT = 0,
        DEFAULT = TIMEOUT,
        INVALID_REGION = 1,
        EMPTY_REGION = 2,
        LOCKED_REGION = 3,
        EXCEPTION = 4
    };

    /*! Default constructor */
    MockManagementController() = default;

    /*! Default destructor */
    virtual ~MockManagementController();

    void send(const Request&, const BridgeInfo&, Response&) override {
        /* not to be called, send(req, resp) is only allowed */
        throw std::exception();
    }

    virtual void send(const ipmi::Request& request, ipmi::Response& response) override {

        try_send<ipmi::command::generic::request::GetDeviceId,
                ipmi::command::generic::response::GetDeviceId>(request, response);

        try_send<ipmi::command::sdv::request::GetBiosVersion,
                ipmi::command::sdv::response::GetBiosVersion>(request, response);

        try_send<ipmi::command::sdv::request::GetProcessorInfo,
                ipmi::command::sdv::response::GetProcessorInfo>(request, response);

        try_send<ipmi::command::sdv::request::GetDimmInfo,
                ipmi::command::sdv::response::GetDimmInfo>(request, response);

        try_send<ipmi::command::sdv::request::GetCableId,
                ipmi::command::sdv::response::GetCableId>(request, response);

        try_send<ipmi::command::sdv::request::GetSystemMacAddressHaswell,
                ipmi::command::sdv::response::GetSystemMacAddressHaswell>(request, response);

        try_send<ipmi::command::sdv::request::GetMdrDataRegionStatus,
                ipmi::command::sdv::response::GetMdrDataRegionStatus>(request, response);
    }

    virtual void send(const ipmi::command::generic::request::GetDeviceId&,
                      ipmi::command::generic::response::GetDeviceId& response) {

        std::vector<std::uint8_t> device_id_data = {
            mock::CC_OK,
            0x00, // Device ID unspecified
            0x00, // Device revision

            mock::FIRMWARE_VERSION_MAJOR,
            mock::FIRMWARE_VERSION_MINOR,
            0x02, // IPMI v2.0
            0x00, // Device support

            // Manufacturer ID (Intel Corporation)
            0x57,
            0x01,
            0x00,

            // Product ID (Intel Atom)
            0x46,
            0x31,

            // Firmware revision
            0x00,
            0x00,
            0x00,
            0x00,
        };

        response.unpack(device_id_data);
    }

    virtual void send(const ipmi::command::sdv::request::GetBiosVersion&,
                      ipmi::command::sdv::response::GetBiosVersion& response) {

        std::vector<std::uint8_t> bios_version_data = {
            mock::CC_OK,
            // ASCII code for BIOS prefix
            'A', '2', '0', '_',
            // BIOS Version
            '2', '1', 'A', '0',
            // Reserved
            0, 0, 0, 0, 0, 0, 0, 0
        };
        response.unpack(bios_version_data);
    }

    virtual void send(const ipmi::command::sdv::request::GetProcessorInfo&,
                      ipmi::command::sdv::response::GetProcessorInfo& response) {

        std::vector<std::uint8_t> processor_info_data = {
            mock::CC_OK,
            // CPU type
            ipmi::command::sdv::response::GetProcessorInfo::CPU_TYPE::CPU_TYPE_XEON,
            // Frequency
            0x74, 0x0E,
            // Presence
            0x01
        };

        response.unpack(processor_info_data);
    }

    virtual void send(const ipmi::command::sdv::request::GetDimmInfo&,
                      ipmi::command::sdv::response::GetDimmInfo& response) {

        std::vector<std::uint8_t> dimm_info_data = {
            mock::CC_OK,
            // Type and voltage
            ipmi::command::sdv::response::GetDimmInfo::DIMM_TYPE::DIMM_TYPE_DDR3 +
                (ipmi::command::sdv::response::GetDimmInfo::DIMM_VOLTAGE::DIMM_VOLTAGE_NORMAL << 6),
            // DIMM speed MHz
            0x40, 0x06,
            // DIMM size MB
            0x00, 0x04,
            // Presence
            ipmi::command::sdv::response::GetDimmInfo::DIMM_PRESENCE::DIMM_PRESENCE_PRESENT
        };

        response.unpack(dimm_info_data);
    }

    virtual void send(const ipmi::command::sdv::request::GetSystemMacAddressHaswell&,
                      ipmi::command::sdv::response::GetSystemMacAddressHaswell& response) {

        std::vector<std::uint8_t> system_mac_data = {
            mock::CC_OK, 0, 0,
            // MAC address
            0x75, 0xd5, 0x35, 0xe5, 0xd5, 0x35
        };

        response.unpack(system_mac_data);
    }

    virtual void send(const ipmi::command::sdv::request::GetCableId&,
                      ipmi::command::sdv::response::GetCableId& response) {

        std::vector<std::uint8_t> cable_id_data = {
            mock::CC_OK,
            // Cable ID data
            '1', '2', '3', 'A', 'B', 'C', '9', '8',
            '7', ' ', ' ', ' ', ' ', ' ', ' ', ' '
        };

        response.unpack(cable_id_data);
    }

    virtual void send(const ipmi::command::sdv::request::GetMdrDataRegionStatus&,
                      ipmi::command::sdv::response::GetMdrDataRegionStatus& response) {

        std::vector<std::uint8_t> mdr_region_data{};
        switch(mdr_test_case) {
            case TestCase::INVALID_REGION:
                mdr_region_data = {
                        mock::CC_OK,
                        mock::MDR_VERSION,
                        ipmi::command::sdv::DataRegionId::SMBIOS_TABLE,
                        ipmi::command::sdv::DataValidation::INVALID,
                        mock::MDR_DATA_UPDATE_COUNT,
                        ipmi::command::sdv::LockStatus::UNLOCKED,
                        std::uint8_t(mock::MDR_REGION_SIZE & 0xff),
                        std::uint8_t((mock::MDR_REGION_SIZE >> 8) & 0xff),
                        std::uint8_t(mock::MDR_REGION_SIZE_USED & 0x00ff),
                        std::uint8_t((mock::MDR_REGION_SIZE_USED & 0xff00) >> 8),
                        mock::MDR_CHECKSUM
                };
                break;
            case TestCase::EMPTY_REGION:
                mdr_region_data = {
                        mock::CC_OK,
                        mock::MDR_VERSION,
                        ipmi::command::sdv::DataRegionId::SMBIOS_TABLE,
                        ipmi::command::sdv::DataValidation::VALID,
                        mock::MDR_DATA_UPDATE_COUNT,
                        ipmi::command::sdv::LockStatus::UNLOCKED,
                        std::uint8_t(mock::MDR_REGION_SIZE & 0xff),
                        std::uint8_t((mock::MDR_REGION_SIZE >> 8) & 0xff),
                        0,
                        0,
                        mock::MDR_CHECKSUM
                };
                break;
            case TestCase::LOCKED_REGION:
                mdr_region_data = {
                        mock::CC_OK,
                        mock::MDR_VERSION,
                        ipmi::command::sdv::DataRegionId::SMBIOS_TABLE,
                        ipmi::command::sdv::DataValidation::VALID,
                        mock::MDR_DATA_UPDATE_COUNT,
                        ipmi::command::sdv::LockStatus::STRICT_LOCK,
                        std::uint8_t(mock::MDR_REGION_SIZE & 0xff),
                        std::uint8_t((mock::MDR_REGION_SIZE >> 8) & 0xff),
                        std::uint8_t(mock::MDR_REGION_SIZE_USED & 0x00ff),
                        std::uint8_t((mock::MDR_REGION_SIZE_USED & 0xff00) >> 8),
                        mock::MDR_CHECKSUM
                };
                break;
            case TestCase::EXCEPTION:
                throw std::runtime_error("Catch exception test.");

            case TestCase::TIMEOUT:
            default:
                mdr_region_data = {mock::CC_TIMEOUT};
                break;
        }
        response.do_unpack(mdr_region_data);
    }

    bool is_connected() const {
        return true;
    }

    void set_mdr_test_case(TestCase test_case) {
        mdr_test_case = test_case;
    }

private:
    TestCase mdr_test_case{TestCase::DEFAULT}; // If not set, default test case.

    template <typename DynamicRequest, typename DynamicResponse>
    void try_send(const ipmi::Request& request, ipmi::Response& response) {
        try {
            send(dynamic_cast<const DynamicRequest&>(request),
                 dynamic_cast<DynamicResponse&>(response));
            return;
        }
        catch (const std::bad_cast&) { }
    }
};

}
