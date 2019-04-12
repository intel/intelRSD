/*!
 * @brief Mock for the NVME target handler.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file mock_nvme_target_handler.hpp
 */

#pragma once

#include "gmock/gmock.h"
#include "nvme-target/base_nvme_target_handler.hpp"

class MockNvmeTargetHandler : public nvme_target::BaseNvmeTargetHandler {
public:

    /* Host management commands */

    MOCK_CONST_METHOD0(get_hosts, nvme_target::IdList());
    MOCK_CONST_METHOD1(add_host, void(const nvme_target::Id&));
    MOCK_CONST_METHOD1(remove_host, void(const nvme_target::Id&));

    /* Subsystem management commands */

    MOCK_CONST_METHOD0(get_subsystems, nvme_target::IdList());
    MOCK_CONST_METHOD1(add_subsystem, void(const nvme_target::Id&));
    MOCK_CONST_METHOD1(remove_subsystem, void(const nvme_target::Id&));
    MOCK_CONST_METHOD2(set_subsystem_allow_all_hosts, void(const nvme_target::Id&, bool));

    /* Subsystem host management commands */

    MOCK_CONST_METHOD1(get_subsystem_hosts, nvme_target::IdList(const nvme_target::Id&));
    MOCK_CONST_METHOD2(add_subsystem_host, void(const nvme_target::Id&, const nvme_target::Id&));
    MOCK_CONST_METHOD2(remove_subsystem_host, void(const nvme_target::Id&, const nvme_target::Id&));

    /* Subsystem namespace management commands */

    MOCK_CONST_METHOD1(get_subsystem_namespaces, nvme_target::IdList(const nvme_target::Id&));
    MOCK_CONST_METHOD2(add_subsystem_namespace, void(const nvme_target::Id&, const nvme_target::Id&));
    MOCK_CONST_METHOD2(remove_subsystem_namespace, void(const nvme_target::Id&, const nvme_target::Id&));
    MOCK_CONST_METHOD3(set_subsystem_namespace_device, void(const nvme_target::Id&, const nvme_target::Id&, const std::string&));
    MOCK_CONST_METHOD2(get_subsystem_namespace_device, std::string(const nvme_target::Id&, const nvme_target::Id&));
    MOCK_CONST_METHOD3(set_subsystem_namespace_enable, void(const nvme_target::Id&, const nvme_target::Id&,  bool));

    /* Port management commands */

    MOCK_CONST_METHOD0(get_ports, nvme_target::IdList());
    MOCK_CONST_METHOD1(add_port, void(const nvme_target::Id&));
    MOCK_CONST_METHOD1(remove_port, void(const nvme_target::Id&));
    MOCK_CONST_METHOD2(set_port_params, void(const nvme_target::Id&, const nvme_target::PortParams&));
    MOCK_CONST_METHOD1(get_port_params, nvme_target::PortParams(const nvme_target::Id&));

    /* Port subsystem management commands */

    MOCK_CONST_METHOD1(get_port_subsystems, nvme_target::IdList(const nvme_target::Id&));
    MOCK_CONST_METHOD2(add_port_subsystem, void(const nvme_target::Id&, const nvme_target::Id&));
    MOCK_CONST_METHOD2(remove_port_subsystem, void(const nvme_target::Id&, const nvme_target::Id&));

    /* Port referrals management commands */

    MOCK_CONST_METHOD1(get_port_referrals, nvme_target::IdList(const nvme_target::Id&));
    MOCK_CONST_METHOD2(add_port_referral, void(const nvme_target::Id&, const nvme_target::Id&));
    MOCK_CONST_METHOD2(remove_port_referral, void(const nvme_target::Id&, const nvme_target::Id&));
    MOCK_CONST_METHOD3(set_port_referral_params, void(const nvme_target::Id&, const nvme_target::Id&, const nvme_target::PortParams&));
    MOCK_CONST_METHOD2(get_port_referral_params, nvme_target::PortParams(const nvme_target::Id&, const nvme_target::Id&));
    MOCK_CONST_METHOD3(set_port_referral_enable, void(const nvme_target::Id&, const nvme_target::Id&,  bool));
};
