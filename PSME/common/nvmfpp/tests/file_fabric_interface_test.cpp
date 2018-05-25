/*!
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation.
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
 * @header{Filesystem}
 * @file file_fabric_interface_test.cpp
 */

#include "nvmfpp/file.hpp"
#include "nvmfpp/file_factory.hpp"
#include "nvmfpp/file_fabric_interface.hpp"
#include "nvmfpp/base_file.hpp"
#include "nvmfpp/base_file_factory.hpp"
#include "nvmfpp/base_fabric_interface.hpp"
#include "nvmfpp/fabric_properties.hpp"


#include "gtest/gtest.h"


#include <memory>


using namespace nvmfpp;

namespace {

class MockFile final : public BaseFile {
public:
    MockFile(bool fail): m_fail_mode(fail) {}
    virtual ~MockFile() {}

    virtual std::string read_from_file() override {
        if (m_fail_mode) { // fail cases
            throw std::runtime_error("Could not open file.");
        } else {          // success cases
            return "nvme-subsystem-test";
        }
    }

    virtual void write_to_file(const std::string&) override {
        if (m_fail_mode) { // fail cases
            throw std::runtime_error("Could not open file.");
        }
    }

private:
    bool m_fail_mode{false};

};

class MockFileFactory final : public BaseFileFactory {
public:
    MockFileFactory(bool fail): m_fail_mode(fail) {}
    virtual ~MockFileFactory() {}

    std::unique_ptr<nvmfpp::BaseFile> create_file(const std::string&, FileMode) {
        return std::unique_ptr<nvmfpp::BaseFile>(new MockFile(m_fail_mode));
    }

private:
    bool m_fail_mode{false};

};

}

namespace testing {

TEST(MockFile, connect_success) {
    FabricProperties fabricProperties{"nvme-subsystem-test", "1.1.1.1", 4420, Transport::RDMA};
    fabricProperties.set_nr_io_queues(1);

    std::unique_ptr<nvmfpp::BaseFileFactory> factory = std::make_unique<::MockFileFactory>(false);

    std::unique_ptr<BaseFabricInterface> fabric = std::make_unique<FileFabricInterface>(std::move(factory));
    ASSERT_NO_THROW(fabric->connect(fabricProperties));
}

TEST(MockFile, connect_fail_lack_of_mandatory_field) {
    FabricProperties fabricProperties{"","1.1.1.1", 4420, Transport::RDMA};
    fabricProperties.set_nr_io_queues(1);

    std::unique_ptr<nvmfpp::BaseFileFactory> factory = std::make_unique<::MockFileFactory>(false);

    std::unique_ptr<BaseFabricInterface> fabric = std::make_unique<FileFabricInterface>(std::move(factory));
    ASSERT_THROW(fabric->connect(fabricProperties), std::invalid_argument);
}

TEST(MockFile, connect_fail_unable_to_write_to_file) {
    FabricProperties fabricProperties{"nvme-subsystem-test", "1.1.1.1", 4420, Transport::RDMA};
    fabricProperties.set_nr_io_queues(1);

    std::unique_ptr <nvmfpp::BaseFileFactory> factory = std::make_unique<::MockFileFactory>(true);

    std::unique_ptr <BaseFabricInterface> fabric = std::make_unique<FileFabricInterface>(std::move(factory));
    ASSERT_THROW(fabric->connect(fabricProperties), std::runtime_error);
}

TEST(MockFile, disconnect_by_nqn_success) {
    FabricProperties fabricProperties{};
    fabricProperties.set_nqn("nvme-subsystem-test");

    std::vector<std::string> drives{"nvme3","nvme4"};

    std::unique_ptr<nvmfpp::BaseFileFactory> factory = std::make_unique<::MockFileFactory>(false);

    std::unique_ptr<BaseFabricInterface> fabric = std::make_unique<FileFabricInterface>(std::move(factory));
    ASSERT_NO_THROW(fabric->disconnect_by_nqn(fabricProperties, drives));
}

TEST(MockFile, disconnect_by_drive_success) {
    FabricProperties fabricProperties{};
    fabricProperties.set_device("nvme1");

    std::unique_ptr<nvmfpp::BaseFileFactory> factory = std::make_unique<::MockFileFactory>(false);

    std::unique_ptr<BaseFabricInterface> fabric = std::make_unique<FileFabricInterface>(std::move(factory));
    ASSERT_NO_THROW(fabric->disconnect_by_drive(fabricProperties));
}

TEST(MockFile, disconnect_by_nqn_faile_wrong_nqn) {
    FabricProperties fabricProperties{};
    fabricProperties.set_nqn("test_nqn");

    std::vector<std::string> drives{"nvme0","nvme1","nvme2","nvme3"};

    std::unique_ptr<nvmfpp::BaseFileFactory> factory = std::make_unique<::MockFileFactory>(false);

    std::unique_ptr<BaseFabricInterface> fabric = std::make_unique<FileFabricInterface>(std::move(factory));
    ASSERT_THROW(fabric->disconnect_by_nqn(fabricProperties, drives), std::invalid_argument);
}

TEST(MockFile, disconnect_by_drive_fail_lack_of_mandatory_field) {
    FabricProperties fabricProperties{};

    std::unique_ptr<nvmfpp::BaseFileFactory> factory = std::make_unique<::MockFileFactory>(false);

    std::unique_ptr<BaseFabricInterface> fabric = std::make_unique<FileFabricInterface>(std::move(factory));
    ASSERT_THROW(fabric->disconnect_by_drive(fabricProperties), std::invalid_argument);
}

TEST(MockFile, disconnect_by_nqn_fail_no_nvme_drives) {
    FabricProperties fabricProperties{};

    std::vector<std::string> drives{};

    std::unique_ptr<nvmfpp::BaseFileFactory> factory = std::make_unique<::MockFileFactory>(false);

    std::unique_ptr<BaseFabricInterface> fabric = std::make_unique<FileFabricInterface>(std::move(factory));
    ASSERT_THROW(fabric->disconnect_by_nqn(fabricProperties, drives), std::invalid_argument);
}

TEST(MockFile, disconnect_by_nqn_fail_lack_of_mandatory_field_with_drives) {
    FabricProperties fabricProperties{};

    std::vector<std::string> drives{"nvme1"};

    std::unique_ptr<nvmfpp::BaseFileFactory> factory = std::make_unique<::MockFileFactory>(false);

    std::unique_ptr<BaseFabricInterface> fabric = std::make_unique<FileFabricInterface>(std::move(factory));
    ASSERT_THROW(fabric->disconnect_by_nqn(fabricProperties, drives), std::invalid_argument);
}

}
