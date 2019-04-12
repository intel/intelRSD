/*!
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
 * @file tests/commands.cpp
 */

#include "nvme/commands/controller_reset.hpp"
#include "nvme/commands/flush.hpp"
#include "nvme/commands/format_nvm.hpp"
#include "nvme/commands/get_log_page.hpp"
#include "nvme/commands/identify.hpp"
#include "nvme/commands/set_features.hpp"
#include "nvme/commands/get_features.hpp"
#include "nvme/commands/namespace_attachment.hpp"
#include "nvme/commands/namespace_management.hpp"

#include "gtest/gtest.h"

using namespace nvme;
using namespace nvme::commands;

namespace testing {

TEST(NvmeCommandsTest, ControllerReset) {
    ControllerReset cmd{};
    ASSERT_EQ(cmd.get_type(), NvmeCommandType::ControllerReset);
}

TEST(NvmeCommandsTest, Flush) {
    Flush cmd(123);
    ASSERT_EQ(cmd.get_type(), NvmeCommandType::NvmCommand);
    ASSERT_EQ(cmd.get_data().cmd.opcode, uint8_t(NvmCommandOpcode::Flush));
    ASSERT_EQ(cmd.get_data().cmd.namespace_id, 123);
}

TEST(NvmeCommandsTest, FormatNvm) {
    FormatNvm cmd(123, FormatNvmSes::CryptographicErase);
    ASSERT_EQ(cmd.get_type(), NvmeCommandType::AdminCommand);
    ASSERT_EQ(cmd.get_data().cmd.opcode, uint8_t(AdminCommandOpcode::FormatNvm));
    ASSERT_EQ(cmd.get_data().cmd.namespace_id, 123);
    ASSERT_EQ(cmd.get_data().cmd.format_nvm.pil_ses, (uint8_t(FormatNvmSes::CryptographicErase) << 1));
}

TEST(NvmeCommandsTest, GetLogPage) {
    GetLogPage cmd(123, LogPageId::CommandsAndEffects);
    ASSERT_EQ(cmd.get_type(), NvmeCommandType::AdminCommand);
    ASSERT_EQ(cmd.get_data().cmd.opcode, uint8_t(AdminCommandOpcode::GetLogPage));
    ASSERT_EQ(cmd.get_data().cmd.namespace_id, 123);
    ASSERT_EQ(cmd.get_data().cmd.get_log_page.log_page_id, uint8_t(LogPageId::CommandsAndEffects));
    ASSERT_EQ(cmd.get_data().cmd.data_pointer.data.address, reinterpret_cast<uint64_t>(&cmd.get_smart_log()));
    ASSERT_EQ(cmd.get_data().cmd.data_pointer.data.address, reinterpret_cast<uint64_t>(&cmd.get_firmware_log()));
    static constexpr unsigned EXPECTED_SIZE = 1024;
    static constexpr unsigned EXPECTED_DWORDS = EXPECTED_SIZE / 4 - 1;
    ASSERT_EQ(cmd.get_data().cmd.data_pointer.data.data_length, EXPECTED_SIZE);
    ASSERT_EQ(cmd.get_data().cmd.get_log_page.num_of_dwords, EXPECTED_DWORDS);
}

TEST(NvmeCommandsTest, NamespaceAttachment) {
    NamespaceAttachment cmd(123, NamespaceAttachmentSel::Detach);
    ASSERT_EQ(cmd.get_type(), NvmeCommandType::AdminCommand);
    ASSERT_EQ(cmd.get_data().cmd.opcode, uint8_t(AdminCommandOpcode::NamespaceAttachment));
    ASSERT_EQ(cmd.get_data().cmd.namespace_id, 123);
    ASSERT_EQ(cmd.get_data().cmd.namespace_attachment.sel, uint8_t(NamespaceAttachmentSel::Detach));
    static constexpr unsigned EXPECTED_SIZE = 4096;
    ASSERT_EQ(cmd.get_data().cmd.data_pointer.data.data_length, EXPECTED_SIZE);

    const ControllerIdList* cil = reinterpret_cast<const ControllerIdList*>(cmd.get_data().cmd.data_pointer.data.address);
    ASSERT_EQ(cil->size, 0);
    cmd.add_controller(12);
    ASSERT_EQ(cil->size, 1);
    ASSERT_EQ(cil->controller_id[0], 12);
    cmd.add_controller(23);
    ASSERT_EQ(cil->size, 2);
    ASSERT_EQ(cil->controller_id[0], 12);
    ASSERT_EQ(cil->controller_id[1], 23);
}

TEST(NvmeCommandsTest, NamespaceManagementDelete) {
    NamespaceManagement cmd(NamespaceManagementSel::Delete);
    ASSERT_EQ(cmd.get_type(), NvmeCommandType::AdminCommand);
    ASSERT_EQ(cmd.get_data().cmd.opcode, uint8_t(AdminCommandOpcode::NamespaceManagement));
    ASSERT_EQ(cmd.get_data().cmd.namespace_management.sel, uint8_t(NamespaceManagementSel::Delete));
    static constexpr unsigned EXPECTED_SIZE = 0;
    ASSERT_EQ(cmd.get_data().cmd.data_pointer.data.data_length, EXPECTED_SIZE);
    ASSERT_EQ(cmd.get_data().cmd.data_pointer.data.address, 0);
    ASSERT_EQ(cmd.get_data().cmd.namespace_id, 0);
    cmd.set_namespace_id(123);
    ASSERT_EQ(cmd.get_data().cmd.namespace_id, 123);
}

TEST(NvmeCommandsTest, NamespaceManagementCreate) {
    NamespaceManagement cmd(NamespaceManagementSel::Create);
    ASSERT_EQ(cmd.get_type(), NvmeCommandType::AdminCommand);
    ASSERT_EQ(cmd.get_data().cmd.opcode, uint8_t(AdminCommandOpcode::NamespaceManagement));
    ASSERT_EQ(cmd.get_data().cmd.namespace_management.sel, uint8_t(NamespaceManagementSel::Create));
    static constexpr unsigned EXPECTED_SIZE = 4096;
    ASSERT_EQ(cmd.get_data().cmd.data_pointer.data.data_length, EXPECTED_SIZE);

    ASSERT_EQ(cmd.get_namespace_id(), 0);
    cmd.get_data().result = 1020;
    ASSERT_EQ(cmd.get_namespace_id(), 1020);

    const NamespaceData* nd = reinterpret_cast<const NamespaceData*>(cmd.get_data().cmd.data_pointer.data.address);

    ASSERT_EQ(nd->nmic, 0);
    cmd.set_as_private();
    ASSERT_EQ(nd->nmic, 1);
    cmd.set_as_public();
    ASSERT_EQ(nd->nmic, 0);

    ASSERT_EQ(nd->size, 0);
    cmd.set_size(123123);
    ASSERT_EQ(nd->size, 123123);

    ASSERT_EQ(nd->capacity, 0);
    cmd.set_capacity(456456);
    ASSERT_EQ(nd->capacity, 456456);
}

TEST(NvmeCommandsTest, Identify) {
    Identify cmd(123, 456, IdentifyCns::ControllerIdList);
    ASSERT_EQ(cmd.get_type(), NvmeCommandType::AdminCommand);
    ASSERT_EQ(cmd.get_data().cmd.opcode, uint8_t(AdminCommandOpcode::Identify));
    ASSERT_EQ(cmd.get_data().cmd.namespace_id, 456);
    ASSERT_EQ(cmd.get_data().cmd.identify.cns, uint8_t(IdentifyCns::ControllerIdList));
    ASSERT_EQ(cmd.get_data().cmd.identify.controller_id, 123);
    static constexpr unsigned EXPECTED_SIZE = 4096;
    ASSERT_EQ(cmd.get_data().cmd.data_pointer.data.data_length, EXPECTED_SIZE);

    ASSERT_EQ(cmd.get_data().cmd.data_pointer.data.address, reinterpret_cast<uint64_t>(&cmd.get_controller_data()));
    ASSERT_EQ(cmd.get_data().cmd.data_pointer.data.address, reinterpret_cast<uint64_t>(&cmd.get_namespace_data()));
    ASSERT_EQ(cmd.get_data().cmd.data_pointer.data.address, reinterpret_cast<uint64_t>(&cmd.get_controller_id_list()));
    ASSERT_EQ(cmd.get_data().cmd.data_pointer.data.address, reinterpret_cast<uint64_t>(&cmd.get_namespace_id_list()));
}


TEST(NvmeCommandsTest, GetFeatures) {
    GetFeatures cmd(456, 123);
    ASSERT_EQ(cmd.get_type(), NvmeCommandType::AdminCommand);
    ASSERT_EQ(cmd.get_data().cmd.opcode, std::uint8_t(AdminCommandOpcode::GetFeatures));
    ASSERT_EQ(cmd.get_data().cmd.namespace_id, 456);
    ASSERT_EQ(cmd.get_data().cmd.get_features.fid, 123);
    ASSERT_EQ(cmd.get_data().cmd.get_features.sel, std::uint8_t(GetFeaturesSel::Current));
}


TEST(NvmeCommandsTest, SetFeatures) {
    SetFeatures cmd(456, 123, 99);
    ASSERT_EQ(cmd.get_type(), NvmeCommandType::AdminCommand);
    ASSERT_EQ(cmd.get_data().cmd.opcode, uint8_t(AdminCommandOpcode::SetFeatures));
    ASSERT_EQ(cmd.get_data().cmd.namespace_id, 456);
    ASSERT_EQ(cmd.get_data().cmd.set_features.fid, 123);
    ASSERT_EQ(cmd.get_data().cmd.set_features.save.sv, 1);
    ASSERT_EQ(cmd.get_data().cmd.set_features.value, 99);
}

}
