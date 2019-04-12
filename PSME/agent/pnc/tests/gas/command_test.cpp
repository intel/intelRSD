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
 * */

#include "safe-string/safe_lib.hpp"

#include "gas/access_interface_factory.hpp"
#include "gas/pcie_access_interface.hpp"
#include "gas/global_address_space_registers.hpp"
#include "gas/mrpc/bind_port.hpp"
#include "gas/mrpc/unbind_port.hpp"
#include "gas/mrpc/twi_access_write.hpp"
#include "gas/mrpc/twi_access_read.hpp"
#include "gas/mrpc/link_status_retrieve.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <fstream>


using namespace agent::pnc::gas;
using namespace agent::pnc::gas::mrpc;
using agent::pnc::gas::AccessInterface;
using ::testing::ElementsAre;

class CommandTest: public ::testing::Test {

protected:
    std::fstream memory_file;
    void SetUp() {
        char data[MRPC_REG_SIZE];
        for(size_t i = 0; i < (MRPC_REG_SIZE); i++) {
            data[i] = 0x00;
        }
        write_file(data, "resource0", MRPC_REG_SIZE, MRPC_INPUT_DATA_REG_OFFSET);
    }

public:
    CommandTest() {}
    virtual ~CommandTest();
    void read_file(char* data, const std::string& path, uint32_t size, uint32_t offset);
    void write_file(char* data, const std::string& path, uint32_t size, uint32_t offset);
};

CommandTest::~CommandTest() { }

void CommandTest::read_file(char* data, const std::string& path, uint32_t size, uint32_t offset) {
    memory_file.open(path,  std::ios::in | std::ios::binary | std::ios::ate);
    if (memory_file.is_open()) {
        memory_file.seekg(offset, std::ios::beg);
        memory_file.read(data, size);
        memory_file.close();
    }
}

void CommandTest::write_file(char* data, const std::string& path, uint32_t size, uint32_t offset) {
    memory_file.open(path,  std::ios::out | std::ios::in | std::ios::binary);
    if (memory_file.is_open()) {
        memory_file.seekg(offset, std::ios::beg);
        memory_file.write(data, size);
        memory_file.close();
    }
}

TEST_F(CommandTest, Commad_BindPortPositive) {

    char bind_data[4];
    char cmd_status[MRPC_STATUS_REG_SIZE]
            = { char(CommandStatus::DONE), 0x00, 0x00, 0x00 };
    char cmd_ret_val[MRPC_COMMAND_RETURN_VALUE_REG_SIZE]
            = { char(PortPartitionP2PBindingReturnValue::COMMAND_SUCCEED), 0x00, 0x00, 0x00};

    //Prepare MRPC region
    write_file(cmd_status, "resource0", MRPC_STATUS_REG_SIZE, MRPC_STATUS_REG_OFFSET);
    write_file(cmd_ret_val, "resource0", MRPC_COMMAND_RETURN_VALUE_REG_SIZE, MRPC_COMMAND_RETURN_VALUE_REG_OFFSET);

    AccessInterfaceFactory aif{};
    AccessInterface* iface = aif.get_interface();
    iface->init("resource0");

    GlobalAddressSpaceRegisters gas;

    BindPort cmd {iface};
    cmd.input.fields.logical_bridge_id = 5;
    cmd.input.fields.partition_id = 7;
    cmd.input.fields.phy_port_id = 34;

    auto status = gas.execute_cmd(cmd);

    iface->deinit();

    read_file(bind_data, "resource0", 4, MRPC_INPUT_DATA_REG_OFFSET);

    ASSERT_EQ(status, CommandStatus::DONE);
    ASSERT_EQ(cmd.output.fields.ret_value, PortPartitionP2PBindingReturnValue::COMMAND_SUCCEED);
    ASSERT_EQ(bind_data[3], cmd.input.fields.phy_port_id);
    ASSERT_EQ(bind_data[2], cmd.input.fields.logical_bridge_id);
    ASSERT_EQ(bind_data[1], cmd.input.fields.partition_id);
    ASSERT_EQ(bind_data[0], char(cmd.sub_command));
}


TEST_F(CommandTest, Commad_UnbindPortPositive) {
    char unbind_data[4];
    char cmd_status[MRPC_STATUS_REG_SIZE]
            = { char(CommandStatus::DONE), 0x00, 0x00, 0x00};
    char cmd_ret_val[MRPC_COMMAND_RETURN_VALUE_REG_SIZE]
            = { char(PortPartitionP2PBindingReturnValue::COMMAND_SUCCEED), 0x00, 0x00, 0x00};

    //Prepare MRPC region
    write_file(cmd_status, "resource0", MRPC_STATUS_REG_SIZE, MRPC_STATUS_REG_OFFSET);
    write_file(cmd_ret_val, "resource0", MRPC_COMMAND_RETURN_VALUE_REG_SIZE, MRPC_COMMAND_RETURN_VALUE_REG_OFFSET);

    AccessInterfaceFactory aif{};
    AccessInterface* iface = aif.get_interface();
    iface->init("resource0");

    GlobalAddressSpaceRegisters gas;

    UnbindPort cmd {iface};
    cmd.input.fields.logical_bridge_id = 5;
    cmd.input.fields.partition_id = 7;
    cmd.input.fields.option = UnbindPortOption::IF_PHY_PORT_IS_L0_OR_L1_SIM_MANAGED_HOT_REMOVE;

    auto status = gas.execute_cmd(cmd);

    iface->deinit();

    read_file(unbind_data, "resource0", 4, MRPC_INPUT_DATA_REG_OFFSET);

    ASSERT_EQ(status, CommandStatus::DONE);
    ASSERT_EQ(cmd.output.fields.ret_value, PortPartitionP2PBindingReturnValue::COMMAND_SUCCEED);
    ASSERT_EQ(unbind_data[3], char(cmd.input.fields.option));
    ASSERT_EQ(unbind_data[2], cmd.input.fields.logical_bridge_id);
    ASSERT_EQ(unbind_data[1], cmd.input.fields.partition_id);
    ASSERT_EQ(unbind_data[0], char(cmd.sub_command));
}

TEST_F(CommandTest, Commad_TwiAccessWritePositive) {

    static constexpr uint8_t DATA_IN_SIZE = 4;
    static constexpr uint8_t TWI_WRITE_DATA_SIZE = 12 + DATA_IN_SIZE;

    uint8_t data_in[DATA_IN_SIZE] = { 0x01, 0x02, 0x03, 0x04 };
    char twi_write_data[TWI_WRITE_DATA_SIZE];
    char cmd_status[MRPC_STATUS_REG_SIZE]
            = { char(CommandStatus::DONE), 0x00, 0x00, 0x00 };
    char cmd_ret_val[MRPC_COMMAND_RETURN_VALUE_REG_SIZE]
            = { char(PortPartitionP2PBindingReturnValue::COMMAND_SUCCEED), 0x00, 0x00, 0x00};

    //Prepare MRPC region
    write_file(cmd_status, "resource0", MRPC_STATUS_REG_SIZE, MRPC_STATUS_REG_OFFSET);
    write_file(cmd_ret_val, "resource0", MRPC_COMMAND_RETURN_VALUE_REG_SIZE, MRPC_COMMAND_RETURN_VALUE_REG_OFFSET);

    AccessInterfaceFactory aif{};
    AccessInterface* iface = aif.get_interface();
    iface->init("resource0");

    GlobalAddressSpaceRegisters gas;

    TwiAccessWrite cmd {iface};
    cmd.input.fields.twi_port = PM85X6TwiPort::PORT0;
    cmd.input.fields.slave_addr = 0x00a0;
    cmd.input.fields.offset = 0;
    cmd.input.fields.bytes_num = DATA_IN_SIZE;
    cmd.input.fields.slave_addr_size = TwiSlaveAddressSize::ADDRESS_7_BIT;
    cmd.input.fields.offset_size = TwiOffsetSize::OFFSET_8_BIT;
    cmd.input.fields.data = data_in;

    auto status = gas.execute_cmd(cmd);

    iface->deinit();

    read_file(twi_write_data, "resource0", TWI_WRITE_DATA_SIZE, MRPC_INPUT_DATA_REG_OFFSET);

    ASSERT_EQ(status, CommandStatus::DONE);
    ASSERT_EQ(cmd.output.fields.ret_value, TwiAccessReturnValue::SUCCESS);
    ASSERT_EQ(twi_write_data[0], char(cmd.sub_command));
    ASSERT_EQ(twi_write_data[1], cmd.input.fields.twi_port);
    ASSERT_EQ(twi_write_data[2], char(cmd.input.fields.slave_addr & 0xff));
    ASSERT_EQ(twi_write_data[3], char((cmd.input.fields.slave_addr >> 8) & 0xff));
    ASSERT_EQ(twi_write_data[4], char(cmd.input.fields.offset & 0xff));
    ASSERT_EQ(twi_write_data[5], char((cmd.input.fields.offset >> 8) & 0xff));
    ASSERT_EQ(twi_write_data[6], char((cmd.input.fields.offset >> 16) & 0xff));
    ASSERT_EQ(twi_write_data[7], char((cmd.input.fields.offset >> 24) & 0xff));
    ASSERT_EQ(twi_write_data[8], char(cmd.input.fields.bytes_num & 0xff));
    ASSERT_EQ(twi_write_data[9], char((cmd.input.fields.bytes_num >> 8) & 0xff));
    ASSERT_EQ(twi_write_data[10], char(cmd.input.fields.slave_addr_size));
    ASSERT_EQ(twi_write_data[11], char(cmd.input.fields.offset_size));
    ASSERT_EQ(twi_write_data[12], char(cmd.input.fields.data[0]));
    ASSERT_EQ(twi_write_data[13], char(cmd.input.fields.data[1]));
    ASSERT_EQ(twi_write_data[14], char(cmd.input.fields.data[2]));
    ASSERT_EQ(twi_write_data[15], char(cmd.input.fields.data[3]));
}

TEST_F(CommandTest, Commad_TwiAccessWriteDataTooLarge) {
    AccessInterfaceFactory aif{};
    AccessInterface* iface = aif.get_interface();
    iface->init("resource0");

    GlobalAddressSpaceRegisters gas;

    TwiAccessWrite cmd {iface};
    cmd.input.fields.twi_port = PM85X6TwiPort::PORT0;
    cmd.input.fields.slave_addr = 0x00a0;
    cmd.input.fields.offset = 0;
    cmd.input.fields.bytes_num = TwiAccessWrite::TWI_WRITE_DATA_MAX_SIZE + 10;
    cmd.input.fields.slave_addr_size = TwiSlaveAddressSize::ADDRESS_7_BIT;
    cmd.input.fields.offset_size = TwiOffsetSize::OFFSET_8_BIT;
    EXPECT_ANY_THROW({
                         gas.execute_cmd(cmd);
                     });

    iface->deinit();
}

TEST_F(CommandTest, Commad_TwiAccessReadPositive) {

    static constexpr uint8_t DATA_OUT_SIZE = 4;
    static constexpr uint8_t TWI_READ_DATA_SIZE = 12;

    char data_out[DATA_OUT_SIZE] = { 0x01, 0x02, 0x03, 0x04 };
    char twi_read_data[TWI_READ_DATA_SIZE];
    char cmd_status[MRPC_STATUS_REG_SIZE]
            = {char(CommandStatus::DONE), 0x00, 0x00, 0x00};
    char cmd_ret_val[MRPC_COMMAND_RETURN_VALUE_REG_SIZE]
            = {char(PortPartitionP2PBindingReturnValue::COMMAND_SUCCEED), 0x00, 0x00, 0x00};

    //Prepare MRPC region
    write_file(cmd_status, "resource0", MRPC_STATUS_REG_SIZE, MRPC_STATUS_REG_OFFSET);
    write_file(cmd_ret_val, "resource0", MRPC_COMMAND_RETURN_VALUE_REG_SIZE, MRPC_COMMAND_RETURN_VALUE_REG_OFFSET);
    write_file(data_out, "resource0", DATA_OUT_SIZE, MRPC_OUTPUT_DATA_REG_OFFSET);

    AccessInterfaceFactory aif{};
    AccessInterface *iface = aif.get_interface();
    iface->init("resource0");

    GlobalAddressSpaceRegisters gas;

    TwiAccessRead cmd{iface};
    cmd.input.fields.twi_port = PM85X6TwiPort::PORT0;
    cmd.input.fields.slave_addr = 0x00a0;
    cmd.input.fields.offset = 0;
    cmd.input.fields.bytes_num = DATA_OUT_SIZE;
    cmd.input.fields.slave_addr_size = TwiSlaveAddressSize::ADDRESS_7_BIT;
    cmd.input.fields.offset_size = TwiOffsetSize::OFFSET_8_BIT;

    auto status = gas.execute_cmd(cmd);

    iface->deinit();

    read_file(twi_read_data, "resource0", TWI_READ_DATA_SIZE, MRPC_INPUT_DATA_REG_OFFSET);

    ASSERT_EQ(status, CommandStatus::DONE);
    ASSERT_EQ(cmd.output.fields.ret_value, TwiAccessReturnValue::SUCCESS);
    ASSERT_EQ(twi_read_data[0], char(cmd.sub_command));
    ASSERT_EQ(twi_read_data[1], cmd.input.fields.twi_port);
    ASSERT_EQ(twi_read_data[2], char(cmd.input.fields.slave_addr & 0xff));
    ASSERT_EQ(twi_read_data[3], char((cmd.input.fields.slave_addr >> 8) & 0xff));
    ASSERT_EQ(twi_read_data[4], char(cmd.input.fields.offset & 0xff));
    ASSERT_EQ(twi_read_data[5], char((cmd.input.fields.offset >> 8) & 0xff));
    ASSERT_EQ(twi_read_data[6], char((cmd.input.fields.offset >> 16) & 0xff));
    ASSERT_EQ(twi_read_data[7], char((cmd.input.fields.offset >> 24) & 0xff));
    ASSERT_EQ(twi_read_data[8], char(cmd.input.fields.bytes_num & 0xff));
    ASSERT_EQ(twi_read_data[9], char((cmd.input.fields.bytes_num >> 8) & 0xff));
    ASSERT_EQ(twi_read_data[10], char(cmd.input.fields.slave_addr_size));
    ASSERT_EQ(twi_read_data[11], char(cmd.input.fields.offset_size));
    ASSERT_EQ(cmd.output.fields.data[0], data_out[0]);
    ASSERT_EQ(cmd.output.fields.data[1], data_out[1]);
    ASSERT_EQ(cmd.output.fields.data[2], data_out[2]);
    ASSERT_EQ(cmd.output.fields.data[3], data_out[3]);
}


TEST_F(CommandTest, Commad_TwiAccessReadDataTooLarge) {
    AccessInterfaceFactory aif{};
    AccessInterface* iface = aif.get_interface();
    iface->init("resource0");

    GlobalAddressSpaceRegisters gas;

    TwiAccessRead cmd {iface};
    cmd.input.fields.twi_port = PM85X6TwiPort::PORT0;
    cmd.input.fields.slave_addr = 0x0a0;
    cmd.input.fields.offset = 0;
    cmd.input.fields.bytes_num = TwiAccessRead::TWI_READ_DATA_MAX_SIZE + 10;
    cmd.input.fields.slave_addr_size = TwiSlaveAddressSize::ADDRESS_7_BIT;
    cmd.input.fields.offset_size = TwiOffsetSize::OFFSET_8_BIT;
    EXPECT_ANY_THROW({
                         gas.execute_cmd(cmd);
                     });

    iface->deinit();
}

TEST_F(CommandTest, Commad_LinkStatusRetrievePositive) {

    //Prepare MRPC region
    char cmd_status[MRPC_STATUS_REG_SIZE]
            = { char(CommandStatus::DONE), 0x00, 0x00, 0x00 };
    char cmd_ret_val[MRPC_COMMAND_RETURN_VALUE_REG_SIZE]
            = { char(LinkStatusRetrieveReturnValue::COMMAND_SUCCEED), 0x00, 0x00, 0x00};
    char data_out[LinkStatusRetrieve::LINK_STATUS_RETRIEVE_OUTPUT_MEMORY_SIZE] = { 1, 1, 1, 0, 4, 4, 1, 1, 3, 0};

    write_file(cmd_status, "resource0", MRPC_STATUS_REG_SIZE, MRPC_STATUS_REG_OFFSET);
    write_file(cmd_ret_val, "resource0", MRPC_COMMAND_RETURN_VALUE_REG_SIZE, MRPC_COMMAND_RETURN_VALUE_REG_OFFSET);
    write_file(data_out, "resource0", LinkStatusRetrieve::LINK_STATUS_RETRIEVE_OUTPUT_MEMORY_SIZE,
               MRPC_OUTPUT_DATA_REG_OFFSET);

    char data_in[LinkStatusRetrieve::LINK_STATUS_RETRIEVE_INPUT_MEMORY_SIZE];

    AccessInterfaceFactory aif{};
    AccessInterface* iface = aif.get_interface();
    iface->init("resource0");

    GlobalAddressSpaceRegisters gas;

    LinkStatusRetrieve cmd{iface};
    cmd.input.fields.input_data = 0x1;

    auto status = gas.execute_cmd(cmd);

    iface->deinit();

    read_file(data_in, "resource0", LinkStatusRetrieve::LINK_STATUS_RETRIEVE_INPUT_MEMORY_SIZE,
              MRPC_INPUT_DATA_REG_OFFSET);

    ASSERT_EQ(status, CommandStatus::DONE);
    ASSERT_EQ(cmd.output.fields.ret_value, LinkStatusRetrieveReturnValue::COMMAND_SUCCEED);
    ASSERT_EQ(char(cmd.output.fields.port_id), data_out[0]);
    ASSERT_EQ(cmd.output.fields.partition_id, data_out[1]);
    ASSERT_EQ(cmd.output.fields.logical_bridge_id, data_out[2]);
    ASSERT_EQ(cmd.output.fields.stack_id_port_id, data_out[3]);
    ASSERT_EQ(cmd.output.fields.cfg_link_width, data_out[4]);
    ASSERT_EQ(cmd.output.fields.neg_link_width, data_out[5]);
    ASSERT_EQ(cmd.output.fields.ups_flag, data_out[6]);
    ASSERT_EQ(cmd.output.fields.linkup_link_rate, data_out[7]);
    ASSERT_EQ(cmd.output.fields.major, data_out[8]);
    ASSERT_EQ(cmd.output.fields.minor, data_out[9]);
}
