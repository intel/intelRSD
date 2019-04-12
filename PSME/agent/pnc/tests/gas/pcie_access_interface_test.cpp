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

#include "gas/pcie_access_interface.hpp"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <fstream>


using namespace agent::pnc::gas;
using agent::pnc::gas::AccessInterface;
using ::testing::ElementsAre;

class PcieAccessInterfaceTest: public ::testing::Test {

protected:
    std::fstream memory_file;
    void SetUp() {
        char data[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
        write_file(data, "resource0", 8, 0);
    }

public:
    PcieAccessInterfaceTest() {}
    virtual ~PcieAccessInterfaceTest();
    void read_file(char* data, const std::string& path, uint32_t size, uint32_t offset);
    void write_file(char* data, const std::string& path, uint32_t size, uint32_t offset);
};

PcieAccessInterfaceTest::~PcieAccessInterfaceTest() { }

void PcieAccessInterfaceTest::read_file(char* data, const std::string& path, uint32_t size, uint32_t offset) {
    memory_file.open(path,  std::ios::in | std::ios::binary | std::ios::ate);
    if (memory_file.is_open()) {
        memory_file.seekg(offset, std::ios::beg);
        memory_file.read(data, size);
        memory_file.close();
    }
}

void PcieAccessInterfaceTest::write_file(char* data, const std::string& path, uint32_t size, uint32_t offset) {
    memory_file.open(path,  std::ios::out |std::ios::binary | std::ios::trunc);
    if (memory_file.is_open()) {
        memory_file.seekg(offset, std::ios::beg);
        memory_file.write(data, size);
        memory_file.close();
    }
}


TEST_F(PcieAccessInterfaceTest, PcieAccessInterface_CreateInstanceNoThrow) {
    EXPECT_NO_THROW({
        auto iface = PcieAccessInterface::get_instance();
        iface->init("resource0");
        iface->deinit();
    });
}

TEST_F(PcieAccessInterfaceTest, PcieAccessInterface_WriteToMemoryPositive) {

    uint8_t data_in[4] = {0x0a, 0x0b, 0x0c, 0x0d};
    char data_out[4];

    auto iface = PcieAccessInterface::get_instance();

    // Offset = 0
    iface->init("resource0");
    iface->write(data_in, 4, 0);
    iface->deinit();
    read_file(data_out, "resource0", 4, 0);
    ASSERT_THAT(data_out,  ElementsAre(0x0a, 0x0b, 0x0c, 0x0d));

    // Offset = 3
    iface->init("resource0");
    iface->write(data_in, 4, 3);
    iface->deinit();
    read_file(data_out, "resource0", 4, 3);
    ASSERT_THAT(data_out,  ElementsAre(0x0a, 0x0b, 0x0c, 0x0d));
}

TEST_F(PcieAccessInterfaceTest, PcieAccessInterface_ReadMemoryPositive) {
    char data_in[8] = {0x0a, 0x0b, 0x0c, 0x0d, 0x2a, 0x2b, 0x2c, 0x2d};
    uint8_t data_out2[2];
    uint8_t data_out4[4];
    uint8_t data_out8[8];

    auto iface = PcieAccessInterface::get_instance();

    write_file(data_in, "resource0", 8, 0);

    iface->init("resource0");
    iface->read(data_out2, 2, 2);
    iface->deinit();
    ASSERT_THAT(data_out2,  ElementsAre(0x0c, 0x0d));

    iface->init("resource0");
    iface->read(data_out4, 4, 0);
    iface->deinit();
    ASSERT_THAT(data_out4,  ElementsAre(0x0a, 0x0b, 0x0c, 0x0d));

    iface->init("resource0");
    iface->read(data_out8, 8, 0);
    iface->deinit();
    ASSERT_THAT(data_out8,  ElementsAre(0x0a, 0x0b, 0x0c, 0x0d, 0x2a, 0x2b, 0x2c, 0x2d));

}

TEST_F(PcieAccessInterfaceTest, PcieAccessInterface_CreateInstanceEmptyPathThrows) {
    EXPECT_ANY_THROW({
        auto iface = PcieAccessInterface::get_instance();
        iface->init("");
    });
}

TEST_F(PcieAccessInterfaceTest, PcieAccessInterface_WriteAreaIsTooLargeThrows) {
    uint8_t data_wr[4] = {0x0a, 0x0b, 0x0c, 0x0d};
    char data_in[4] = {0x0a, 0x0b, 0x0c, 0x0d};

    auto iface = PcieAccessInterface::get_instance();

    write_file(data_in, "resource0", 4, 0);

    // size is greater that file memory region
    iface->init("resource0");
    EXPECT_ANY_THROW({
        iface->write(data_wr, 6, 0);
    });
    // offset is out of bound
    EXPECT_ANY_THROW({
        iface->write(data_wr, 0, 6);
    });
    iface->deinit();
}

TEST_F(PcieAccessInterfaceTest, PcieAccessInterface_WriteBufferNullPtrThrows) {
    uint8_t* data_wr = nullptr;
    char data_in[4] = {0x0a, 0x0b, 0x0c, 0x0d};

    auto iface = PcieAccessInterface::get_instance();

    write_file(data_in, "resource0", 4, 0);

    iface->init("resource0");
    EXPECT_ANY_THROW({
        iface->write(data_wr, 4, 0);
    });
    iface->deinit();
}

TEST_F(PcieAccessInterfaceTest, PcieAccessInterface_WriteIfaceNotInitThrows) {
    uint8_t* data_wr = nullptr;
    char data_in[4] = {0x0a, 0x0b, 0x0c, 0x0d};

    auto iface = PcieAccessInterface::get_instance();

    write_file(data_in, "resource0", 4, 0);

    EXPECT_ANY_THROW({
        iface->write(data_wr, 4, 0);
    });
}

TEST_F(PcieAccessInterfaceTest, PcieAccessInterface_ReadAreaIsTooLargeThrows) {
    uint8_t data_rd[4];
    char data_in[4] = {0x0a, 0x0b, 0x0c, 0x0d};

    auto iface = PcieAccessInterface::get_instance();

    write_file(data_in, "resource0", 4, 0);

    // size is greater that file memory region
    iface->init("resource0");
    EXPECT_ANY_THROW({
        iface->read(data_rd, 6, 0);
    });
    // offset is out of bound
    EXPECT_ANY_THROW({
        iface->read(data_rd, 0, 6);
    });
    iface->deinit();
}

TEST_F(PcieAccessInterfaceTest, PcieAccessInterface_ReadBufferNullPtrThrows) {
    uint8_t* data_rd = nullptr;
    char data_in[4] = {0x0a, 0x0b, 0x0c, 0x0d};

    auto iface = PcieAccessInterface::get_instance();

    write_file(data_in, "resource0", 4, 0);

    iface->init("resource0");
    EXPECT_ANY_THROW({
        iface->read(data_rd, 4, 0);
    });
    iface->deinit();
}

TEST_F(PcieAccessInterfaceTest, PcieAccessInterface_ReadIfaceNotInitThrows) {
    uint8_t* data_rd = nullptr;
    char data_in[4] = {0x0a, 0x0b, 0x0c, 0x0d};

    auto iface = PcieAccessInterface::get_instance();

    write_file(data_in, "resource0", 4, 0);

    EXPECT_ANY_THROW({
        iface->read(data_rd, 4, 0);
    });
}
