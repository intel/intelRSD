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
 * @file tests/nvme_interface.cpp
 */

#include "nvme/nvme_interface.hpp"
#include "nvme/nvme_exception.hpp"

#include "gtest/gtest.h"

#include <functional>



using namespace nvme;
using namespace nvme::commands;

namespace {

class MockNvmeInvoker : public AbstractNvmeInvoker {
public:

    using Handler = std::function<void(const std::string&, GenericNvmeCommand& command)>;

    MockNvmeInvoker(const Handler& handler): m_handler(handler) {}
    virtual ~MockNvmeInvoker() {}

    /* Mocked functionality */
    virtual void execute(const std::string& target, GenericNvmeCommand& command) const override {
        m_handler(target, command);
    }

private:
    Handler m_handler{};
};

}

namespace testing {

TEST(NvmeInterfaceTest, TargetIsUsedAndMethodsAreCalled) {
    bool called = false;
    auto invoker = std::make_shared<MockNvmeInvoker>(
        [&called] (const std::string& target, GenericNvmeCommand&) {
            ASSERT_EQ(target, "test");
            called = true;
        }
    );
    NvmeInterface interface(invoker);
    interface.reset("test");
    ASSERT_EQ(called, true);
}

TEST(NvmeInterfaceTest, GenericExceptionsAreForwarded) {
    auto invoker = std::make_shared<MockNvmeInvoker>(
        [] (const std::string& target, GenericNvmeCommand&) {
            ASSERT_EQ(target, "test");
            throw NvmeException("throw");
        }
    );
    NvmeInterface interface(invoker);
    try {
        interface.reset("test");
        FAIL() << "Exception expected";
    }
    catch (const NvmeException& e) {
        EXPECT_EQ(e.is_nvme(), false);
        EXPECT_TRUE(std::string{"throw"} == e.what());
    }
    catch (...) {
        FAIL() << "Invalid exception thrown";
    }
}

TEST(NvmeInterfaceTest, NvmeExceptionsAreForwarded) {
    auto invoker = std::make_shared<MockNvmeInvoker>(
        [] (const std::string& target, GenericNvmeCommand&) {
            ASSERT_EQ(target, "test");
            throw NvmeException(0xF1FF);
        }
    );
    NvmeInterface interface(invoker);
    try {
        interface.reset("test");
        FAIL() << "Exception expected";
    }
    catch (const NvmeException& e) {
        EXPECT_EQ(e.is_nvme(), true);
    }
    catch (...) {
        FAIL() << "Invalid exception thrown";
    }
}

TEST(NvmeInterfaceTest, ResetCommand) {
    bool called = false;
    auto invoker = std::make_shared<MockNvmeInvoker>(
        [&called] (const std::string& target, GenericNvmeCommand& cmd) {
            ASSERT_EQ(cmd.get_type(), NvmeCommandType::ControllerReset);
            ASSERT_EQ(target, "test");
            called = true;
        }
    );
    NvmeInterface interface(invoker);
    ASSERT_NO_THROW(interface.reset("test"));
    ASSERT_EQ(called, true);
}

TEST(NvmeInterfaceTest, FlushCommand) {
    bool called = false;
    auto invoker = std::make_shared<MockNvmeInvoker>(
        [&called] (const std::string& target, GenericNvmeCommand& cmd) {
            ASSERT_EQ(cmd.get_type(), NvmeCommandType::NvmCommand);
            ASSERT_EQ(cmd.get_data().cmd.opcode, uint8_t(NvmCommandOpcode::Flush));
            ASSERT_EQ(cmd.get_data().cmd.namespace_id, 123);
            ASSERT_EQ(target, "test");
            called = true;
        }
    );
    NvmeInterface interface(invoker);
    ASSERT_NO_THROW(interface.flush("test", 123));
    ASSERT_EQ(called, true);
}

TEST(NvmeInterfaceTest, FormatCommand) {
    bool called = false;
    auto invoker = std::make_shared<MockNvmeInvoker>(
        [&called] (const std::string& target, GenericNvmeCommand& cmd) {
            ASSERT_EQ(cmd.get_type(), NvmeCommandType::AdminCommand);
            ASSERT_EQ(cmd.get_data().cmd.opcode, uint8_t(AdminCommandOpcode::FormatNvm));
            ASSERT_EQ(cmd.get_data().cmd.namespace_id, 123);
            ASSERT_EQ(cmd.get_data().cmd.format_nvm.pil_ses, uint8_t(FormatNvmSes::CryptographicErase) << 1);
            ASSERT_EQ(target, "test");
            called = true;
        }
    );
    NvmeInterface interface(invoker);
    ASSERT_NO_THROW(interface.format("test", 123, FormatNvmSes::CryptographicErase));
    ASSERT_EQ(called, true);
}

TEST(NvmeInterfaceTest, GetSmartLogCommand) {
    bool called = false;
    auto invoker = std::make_shared<MockNvmeInvoker>(
        [&called] (const std::string& target, GenericNvmeCommand& cmd) {
            ASSERT_EQ(target, "test");
            ASSERT_EQ(cmd.get_type(), NvmeCommandType::AdminCommand);
            ASSERT_EQ(cmd.get_data().cmd.opcode, uint8_t(AdminCommandOpcode::GetLogPage));
            ASSERT_EQ(cmd.get_data().cmd.namespace_id, 123);
            ASSERT_EQ(cmd.get_data().cmd.get_log_page.log_page_id, uint8_t(LogPageId::Smart));
            ASSERT_FALSE(cmd.get_data().cmd.data_pointer.data.address == 0);
            *reinterpret_cast<uint8_t*>(cmd.get_data().cmd.data_pointer.data.address) = 234;
            called = true;
        }
    );
    NvmeInterface interface(invoker);
    auto ret = interface.get_smart_log("test", 123);
    ASSERT_EQ(*reinterpret_cast<uint8_t*>(&ret), 234);
    ASSERT_EQ(called, true);
}

TEST(NvmeInterfaceTest, GetFirmwareLogCommand) {
    bool called = false;
    auto invoker = std::make_shared<MockNvmeInvoker>(
        [&called] (const std::string& target, GenericNvmeCommand& cmd) {
            ASSERT_EQ(target, "test");
            ASSERT_EQ(cmd.get_type(), NvmeCommandType::AdminCommand);
            ASSERT_EQ(cmd.get_data().cmd.opcode, uint8_t(AdminCommandOpcode::GetLogPage));
            ASSERT_EQ(cmd.get_data().cmd.namespace_id, 123);
            ASSERT_EQ(cmd.get_data().cmd.get_log_page.log_page_id, uint8_t(LogPageId::Firmware));
            ASSERT_FALSE(cmd.get_data().cmd.data_pointer.data.address == 0);
            *reinterpret_cast<uint8_t*>(cmd.get_data().cmd.data_pointer.data.address) = 221;
            called = true;
        }
    );
    NvmeInterface interface(invoker);
    auto ret = interface.get_firmware_log("test", 123);
    ASSERT_EQ(*reinterpret_cast<uint8_t*>(&ret), 221);
    ASSERT_EQ(called, true);
}

TEST(NvmeInterfaceTest, GetWriteLatencyHistogram) {
    bool called = false;
    auto invoker = std::make_shared<MockNvmeInvoker>(
        [&called] (const std::string& target, GenericNvmeCommand& cmd) {
            ASSERT_EQ(target, "test");
            ASSERT_EQ(cmd.get_type(), NvmeCommandType::AdminCommand);
            ASSERT_EQ(cmd.get_data().cmd.opcode, uint8_t(AdminCommandOpcode::GetLogPage));
            ASSERT_EQ(cmd.get_data().cmd.namespace_id, 123);
            ASSERT_EQ(cmd.get_data().cmd.get_log_page.log_page_id, std::uint8_t(LogPageId::IntelVendorLatencyHistogramWrite));
            ASSERT_FALSE(cmd.get_data().cmd.data_pointer.data.address == 0);
            *reinterpret_cast<std::uint8_t*>(cmd.get_data().cmd.data_pointer.data.address) = 234;
            called = true;
        }
    );

    NvmeInterface interface(invoker);
    auto ret = interface.get_write_latency_histogram("test", 123);
    ASSERT_EQ(*reinterpret_cast<uint8_t*>(&ret), 234);
    ASSERT_EQ(called, true);
}

TEST(NvmeInterfaceTest, GetReadLatencyHistogram) {
    bool called = false;
    auto invoker = std::make_shared<MockNvmeInvoker>(
        [&called] (const std::string& target, GenericNvmeCommand& cmd) {
            ASSERT_EQ(target, "test");
            ASSERT_EQ(cmd.get_type(), NvmeCommandType::AdminCommand);
            ASSERT_EQ(cmd.get_data().cmd.opcode, uint8_t(AdminCommandOpcode::GetLogPage));
            ASSERT_EQ(cmd.get_data().cmd.namespace_id, 123);
            ASSERT_EQ(cmd.get_data().cmd.get_log_page.log_page_id, std::uint8_t(LogPageId::IntelVendorLatencyHistogramRead));
            ASSERT_FALSE(cmd.get_data().cmd.data_pointer.data.address == 0);
            *reinterpret_cast<std::uint8_t*>(cmd.get_data().cmd.data_pointer.data.address) = 234;
            called = true;
        }
    );

    NvmeInterface interface(invoker);
    auto ret = interface.get_read_latency_histogram("test", 123);
    ASSERT_EQ(*reinterpret_cast<uint8_t*>(&ret), 234);
    ASSERT_EQ(called, true);
}

TEST(NvmeInterfaceTest, GetControllerInfoCommand) {
    bool called = false;
    auto invoker = std::make_shared<MockNvmeInvoker>(
        [&called] (const std::string& target, GenericNvmeCommand& cmd) {
            ASSERT_EQ(target, "test");
            ASSERT_EQ(cmd.get_type(), NvmeCommandType::AdminCommand);
            ASSERT_EQ(cmd.get_data().cmd.opcode, uint8_t(AdminCommandOpcode::Identify));
            ASSERT_EQ(cmd.get_data().cmd.namespace_id, 0);
            ASSERT_EQ(cmd.get_data().cmd.identify.controller_id, 123);
            ASSERT_EQ(cmd.get_data().cmd.identify.cns, uint8_t(IdentifyCns::Controller));
            ASSERT_FALSE(cmd.get_data().cmd.data_pointer.data.address == 0);
            *reinterpret_cast<uint8_t*>(cmd.get_data().cmd.data_pointer.data.address) = 221;
            called = true;
        }
    );
    NvmeInterface interface(invoker);
    auto ret = interface.get_controller_info("test", 123);
    ASSERT_EQ(*reinterpret_cast<uint8_t*>(&ret), 221);
    ASSERT_EQ(called, true);
}

TEST(NvmeInterfaceTest, GetNamespaceInfoCommand) {
    bool called = false;
    auto invoker = std::make_shared<MockNvmeInvoker>(
        [&called] (const std::string& target, GenericNvmeCommand& cmd) {
            ASSERT_EQ(target, "test");
            ASSERT_EQ(cmd.get_type(), NvmeCommandType::AdminCommand);
            ASSERT_EQ(cmd.get_data().cmd.opcode, uint8_t(AdminCommandOpcode::Identify));
            ASSERT_EQ(cmd.get_data().cmd.namespace_id, 1231);
            ASSERT_EQ(cmd.get_data().cmd.identify.controller_id, 0);
            ASSERT_EQ(cmd.get_data().cmd.identify.cns, uint8_t(IdentifyCns::Namespace));
            ASSERT_FALSE(cmd.get_data().cmd.data_pointer.data.address == 0);
            *reinterpret_cast<uint8_t*>(cmd.get_data().cmd.data_pointer.data.address) = 221;
            called = true;
        }
    );
    NvmeInterface interface(invoker);
    auto ret = interface.get_namespace_info("test", 1231);
    ASSERT_EQ(*reinterpret_cast<uint8_t*>(&ret), 221);
    ASSERT_EQ(called, true);
}

TEST(NvmeInterfaceTest, GetActiveNamespacesCommand) {
    bool called = false;
    auto invoker = std::make_shared<MockNvmeInvoker>(
        [&called] (const std::string& target, GenericNvmeCommand& cmd) {
            ASSERT_EQ(target, "test");
            ASSERT_EQ(cmd.get_type(), NvmeCommandType::AdminCommand);
            ASSERT_EQ(cmd.get_data().cmd.opcode, uint8_t(AdminCommandOpcode::Identify));
            ASSERT_EQ(cmd.get_data().cmd.namespace_id, 0);
            ASSERT_EQ(cmd.get_data().cmd.identify.controller_id, 0);
            ASSERT_EQ(cmd.get_data().cmd.identify.cns, uint8_t(IdentifyCns::NamespaceIdList));
            ASSERT_FALSE(cmd.get_data().cmd.data_pointer.data.address == 0);
            *reinterpret_cast<uint8_t*>(cmd.get_data().cmd.data_pointer.data.address) = 221;
            called = true;
        }
    );
    NvmeInterface interface(invoker);
    auto ret = interface.get_active_namespaces("test");
    ASSERT_EQ(*reinterpret_cast<uint8_t*>(&ret), 221);
    ASSERT_EQ(called, true);
}

TEST(NvmeInterfaceTest, GetAllocatedNamespacesCommand) {
    bool called = false;
    auto invoker = std::make_shared<MockNvmeInvoker>(
        [&called] (const std::string& target, GenericNvmeCommand& cmd) {
            ASSERT_EQ(target, "test");
            ASSERT_EQ(cmd.get_type(), NvmeCommandType::AdminCommand);
            ASSERT_EQ(cmd.get_data().cmd.opcode, uint8_t(AdminCommandOpcode::Identify));
            ASSERT_EQ(cmd.get_data().cmd.namespace_id, 0);
            ASSERT_EQ(cmd.get_data().cmd.identify.controller_id, 0);
            ASSERT_EQ(cmd.get_data().cmd.identify.cns, uint8_t(IdentifyCns::AllocatedNamespaceIdList));
            ASSERT_FALSE(cmd.get_data().cmd.data_pointer.data.address == 0);
            *reinterpret_cast<uint8_t*>(cmd.get_data().cmd.data_pointer.data.address) = 221;
            called = true;
        }
    );
    NvmeInterface interface(invoker);
    auto ret = interface.get_allocated_namespaces("test");
    ASSERT_EQ(*reinterpret_cast<uint8_t*>(&ret), 221);
    ASSERT_EQ(called, true);
}

TEST(NvmeInterfaceTest, GetControllersCommand) {
    bool called = false;
    auto invoker = std::make_shared<MockNvmeInvoker>(
        [&called] (const std::string& target, GenericNvmeCommand& cmd) {
            ASSERT_EQ(target, "test");
            ASSERT_EQ(cmd.get_type(), NvmeCommandType::AdminCommand);
            ASSERT_EQ(cmd.get_data().cmd.opcode, uint8_t(AdminCommandOpcode::Identify));
            ASSERT_EQ(cmd.get_data().cmd.namespace_id, 0);
            ASSERT_EQ(cmd.get_data().cmd.identify.controller_id, 0);
            ASSERT_EQ(cmd.get_data().cmd.identify.cns, uint8_t(IdentifyCns::ControllerIdList));
            ASSERT_FALSE(cmd.get_data().cmd.data_pointer.data.address == 0);
            *reinterpret_cast<uint8_t*>(cmd.get_data().cmd.data_pointer.data.address) = 221;
            called = true;
        }
    );
    NvmeInterface interface(invoker);
    auto ret = interface.get_controllers("test");
    ASSERT_EQ(*reinterpret_cast<uint8_t*>(&ret), 221);
    ASSERT_EQ(called, true);
}

TEST(NvmeInterfaceTest, GetNamespaceControllersCommand) {
    bool called = false;
    auto invoker = std::make_shared<MockNvmeInvoker>(
        [&called] (const std::string& target, GenericNvmeCommand& cmd) {
            ASSERT_EQ(target, "test");
            ASSERT_EQ(cmd.get_type(), NvmeCommandType::AdminCommand);
            ASSERT_EQ(cmd.get_data().cmd.opcode, uint8_t(AdminCommandOpcode::Identify));
            ASSERT_EQ(cmd.get_data().cmd.namespace_id, 1231);
            ASSERT_EQ(cmd.get_data().cmd.identify.controller_id, 0);
            ASSERT_EQ(cmd.get_data().cmd.identify.cns, uint8_t(IdentifyCns::NamespaceControllerIdList));
            ASSERT_FALSE(cmd.get_data().cmd.data_pointer.data.address == 0);
            *reinterpret_cast<uint8_t*>(cmd.get_data().cmd.data_pointer.data.address) = 221;
            called = true;
        }
    );
    NvmeInterface interface(invoker);
    auto ret = interface.get_namespace_controllers("test", 1231);
    ASSERT_EQ(*reinterpret_cast<uint8_t*>(&ret), 221);
    ASSERT_EQ(called, true);
}

TEST(NvmeInterfaceTest, AttachNamespaceCommand) {
    bool called = false;
    auto invoker = std::make_shared<MockNvmeInvoker>(
        [&called] (const std::string& target, GenericNvmeCommand& cmd) {
            ASSERT_EQ(target, "test");
            ASSERT_EQ(cmd.get_type(), NvmeCommandType::AdminCommand);
            ASSERT_EQ(cmd.get_data().cmd.opcode, uint8_t(AdminCommandOpcode::NamespaceAttachment));
            ASSERT_EQ(cmd.get_data().cmd.namespace_id, 1231);
            ASSERT_EQ(cmd.get_data().cmd.namespace_attachment.sel, uint8_t(NamespaceAttachmentSel::Attach));
            ASSERT_FALSE(cmd.get_data().cmd.data_pointer.data.address == 0);
            ControllerIdList* cil = reinterpret_cast<ControllerIdList*>(cmd.get_data().cmd.data_pointer.data.address);
            ASSERT_EQ(cil->size, 1);
            ASSERT_EQ(cil->controller_id[0], 11);
            called = true;
        }
    );
    NvmeInterface interface(invoker);
    interface.attach_namespace("test", 11, 1231);
    ASSERT_EQ(called, true);
}

TEST(NvmeInterfaceTest, DetachNamespaceCommand) {
    bool called = false;
    auto invoker = std::make_shared<MockNvmeInvoker>(
        [&called] (const std::string& target, GenericNvmeCommand& cmd) {
            ASSERT_EQ(target, "test");
            ASSERT_EQ(cmd.get_type(), NvmeCommandType::AdminCommand);
            ASSERT_EQ(cmd.get_data().cmd.opcode, uint8_t(AdminCommandOpcode::NamespaceAttachment));
            ASSERT_EQ(cmd.get_data().cmd.namespace_id, 1231);
            ASSERT_EQ(cmd.get_data().cmd.namespace_attachment.sel, uint8_t(NamespaceAttachmentSel::Detach));
            ASSERT_FALSE(cmd.get_data().cmd.data_pointer.data.address == 0);
            ControllerIdList* cil = reinterpret_cast<ControllerIdList*>(cmd.get_data().cmd.data_pointer.data.address);
            ASSERT_EQ(cil->size, 1);
            ASSERT_EQ(cil->controller_id[0], 11);
            called = true;
        }
    );
    NvmeInterface interface(invoker);
    interface.detach_namespace("test", 11, 1231);
    ASSERT_EQ(called, true);
}

TEST(NvmeInterfaceTest, DeleteNamespaceCommand) {
    bool called = false;
    auto invoker = std::make_shared<MockNvmeInvoker>(
        [&called] (const std::string& target, GenericNvmeCommand& cmd) {
            ASSERT_EQ(target, "test");
            ASSERT_EQ(cmd.get_type(), NvmeCommandType::AdminCommand);
            ASSERT_EQ(cmd.get_data().cmd.opcode, uint8_t(AdminCommandOpcode::NamespaceManagement));
            ASSERT_EQ(cmd.get_data().cmd.namespace_id, 1231);
            ASSERT_EQ(cmd.get_data().cmd.namespace_attachment.sel, uint8_t(NamespaceManagementSel::Delete));
            called = true;
        }
    );
    NvmeInterface interface(invoker);
    interface.delete_namespace("test", 1231);
    ASSERT_EQ(called, true);
}

TEST(NvmeInterfaceTest, CreateNamespaceCommand) {
    bool called = false;
    auto invoker = std::make_shared<MockNvmeInvoker>(
        [&called] (const std::string& target, GenericNvmeCommand& cmd) {
            ASSERT_EQ(target, "test");
            ASSERT_EQ(cmd.get_type(), NvmeCommandType::AdminCommand);
            ASSERT_EQ(cmd.get_data().cmd.opcode, uint8_t(AdminCommandOpcode::NamespaceManagement));
            ASSERT_EQ(cmd.get_data().cmd.namespace_id, 0);
            ASSERT_EQ(cmd.get_data().cmd.namespace_attachment.sel, uint8_t(NamespaceManagementSel::Create));
            ASSERT_FALSE(cmd.get_data().cmd.data_pointer.data.address == 0);
            NamespaceData* nd = reinterpret_cast<NamespaceData*>(cmd.get_data().cmd.data_pointer.data.address);
            ASSERT_EQ(nd->size, 123123);
            ASSERT_EQ(nd->capacity, 234234);
            ASSERT_EQ(nd->nmic, 1);
            cmd.get_data().result = 5511;
            called = true;
        }
    );
    NvmeInterface interface(invoker);
    auto ret = interface.create_namespace("test", 123123, 234234, true);
    ASSERT_EQ(ret, 5511);
    ASSERT_EQ(called, true);
}

TEST(NvmeInterfaceTest, GetLatencyFeature) {
    bool called = false;
    auto invoker = std::make_shared<MockNvmeInvoker>(
        [&called] (const std::string& target, GenericNvmeCommand& cmd) {
            ASSERT_EQ(target, "test");
            ASSERT_EQ(cmd.get_type(), NvmeCommandType::AdminCommand);
            ASSERT_EQ(cmd.get_data().cmd.opcode, std::uint8_t(AdminCommandOpcode::GetFeatures));
            ASSERT_EQ(cmd.get_data().cmd.namespace_id, 123);
            called = true;
            cmd.get_data().result = 1;
        }
    );

    NvmeInterface interface(invoker);
    auto result = interface.get_latency_tracking_feature("test", 123);
    ASSERT_EQ(called, true);
    ASSERT_EQ(result, 1);
}


TEST(NvmeInterfaceTest, SetLatencyFeature) {
    bool called = false;
    auto invoker = std::make_shared<MockNvmeInvoker>(
        [&called] (const std::string& target, GenericNvmeCommand& cmd) {
            ASSERT_EQ(target, "test");
            ASSERT_EQ(cmd.get_type(), NvmeCommandType::AdminCommand);
            ASSERT_EQ(cmd.get_data().cmd.opcode, std::uint8_t(AdminCommandOpcode::SetFeatures));
            ASSERT_EQ(cmd.get_data().cmd.namespace_id, 123);
            ASSERT_EQ(cmd.get_data().cmd.set_features.value, 1);
            called = true;
        }
    );

    NvmeInterface interface(invoker);
    interface.enable_latency_tracking_feature("test", 123);
    ASSERT_EQ(called, true);
}

}
