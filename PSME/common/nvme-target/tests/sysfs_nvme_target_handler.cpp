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
 * @file tests/sysfs_nvme_target_handler.cpp
 */

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "nvme-target/sysfs_nvme_target_handler.hpp"
#include "sysfs/abstract_sysfs_interface.hpp"

using namespace sysfs;
using namespace nvme_target;

namespace {

const Path BASE_PATH = "/sys/kernel/config/nvmet/";
const std::string DIR_HOSTS = "hosts";
const std::string DIR_SUBSYSTEMS = "subsystems";
const std::string DIR_PORTS = "ports";
const std::string DIR_ALLOWED_HOSTS = "allowed_hosts";
const std::string DIR_NAMESPACES = "namespaces";
const std::string DIR_REFERRALS = "referrals";
const std::string FILE_ALLOW_ALL = "attr_allow_any_host";
const std::string FILE_DEVICE = "device_path";
const std::string FILE_ENABLE = "enable";
const std::string FILE_IP = "addr_traddr";
const std::string FILE_IP_TYPE = "addr_adrfam";
const std::string FILE_PORT = "addr_trsvcid";
const std::string FILE_TRANSPORT = "addr_trtype";

// required as all mock methods are required (parent class is abstract), but not all of them are used
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-member-function"
#endif


class MockSysfsInterface : public AbstractSysfsInterface {
public:

    MOCK_CONST_METHOD1(get_dir, SysfsDir(const Path& path));
    MOCK_CONST_METHOD2(add_dir, void(const Path& path, const std::string& name));
    MOCK_CONST_METHOD1(del_dir, void(const Path& path));
    MOCK_CONST_METHOD1(get_link, SysfsLink(const Path& path));
    MOCK_CONST_METHOD3(add_link, void(const Path& path, const std::string& name, const Path& value));
    MOCK_CONST_METHOD1(del_link, void(const Path& path));
    MOCK_CONST_METHOD1(get_file, SysfsFile(const Path& path));
    MOCK_CONST_METHOD3(add_file, void(const Path& path, const std::string& name, const std::string& value));
    MOCK_CONST_METHOD2(set_file, void(const Path& path, const std::string& value));
    MOCK_CONST_METHOD1(del_file, void(const Path& path));
    MOCK_CONST_METHOD1(dir_exists, bool(const Path& path));
    MOCK_CONST_METHOD1(link_exists, bool(const Path& path));
    MOCK_CONST_METHOD1(file_exists, bool(const Path& path));
    MOCK_CONST_METHOD1(get_mounts, std::vector<SysfsMount>(const Path& path));
    MOCK_CONST_METHOD1(get_absolute_path, Path(const Path& input));

};

#ifdef __clang__
#pragma clang diagnostic pop
#endif

void EXPECT_SAME_IDS(IdList list1, IdList list2) {
    std::sort(list1.begin(), list1.end());
    std::sort(list2.begin(), list2.end());
    EXPECT_EQ(list1, list2);
}

// gets on paths BASE_PATH / dir
void test_lvl1_get(const std::string& test_dir, IdList(SysfsNvmeTargetHandler::*method)() const) {

    using namespace testing;

    auto sysfs = std::make_shared<MockSysfsInterface>();
    SysfsNvmeTargetHandler handler(sysfs);

    SysfsDir dir{};
    dir.name = test_dir;
    dir.path = BASE_PATH / test_dir;

    // test with an empty directory
    EXPECT_CALL(*sysfs, get_dir(BASE_PATH / test_dir)).Times(1).WillOnce(Return(dir));
    EXPECT_EQ((handler.*method)().size(), 0);

    // test with a non empty directory
    dir.dirs = {Path{BASE_PATH / test_dir / "entry-1"}, Path{BASE_PATH / test_dir / "entry-2"}};
    EXPECT_CALL(*sysfs, get_dir(BASE_PATH / test_dir)).Times(1).WillOnce(Return(dir));
    EXPECT_SAME_IDS({"entry-1", "entry-2"}, (handler.*method)());
}

// adds on paths BASE_PATH / dir
void test_lvl1_add(const std::string& test_dir, void(SysfsNvmeTargetHandler::*method)(const std::string&) const) {
    auto sysfs = std::make_shared<MockSysfsInterface>();
    SysfsNvmeTargetHandler handler(sysfs);

    using namespace testing;

    EXPECT_CALL(*sysfs, add_dir(BASE_PATH / test_dir, "new-entry")).Times(1);
    (handler.*method)("new-entry");

    EXPECT_CALL(*sysfs, add_dir(BASE_PATH / test_dir, "existing-entry")).Times(1).WillOnce(Throw(std::runtime_error("")));
    EXPECT_THROW((handler.*method)("existing-entry"), std::runtime_error);
}

// removes on paths BASE_PATH / dir
void test_lvl1_remove(const std::string& test_dir, void(SysfsNvmeTargetHandler::*method)(const std::string&) const) {
    auto sysfs = std::make_shared<MockSysfsInterface>();
    SysfsNvmeTargetHandler handler(sysfs);

    using namespace testing;

    EXPECT_CALL(*sysfs, del_dir(BASE_PATH / test_dir / "existing-entry")).Times(1);
    (handler.*method)("existing-entry");

    EXPECT_CALL(*sysfs, del_dir(BASE_PATH / test_dir / "non-existing-entry")).Times(1).WillOnce(Throw(std::runtime_error("")));
    EXPECT_THROW((handler.*method)("non-existing-entry"), std::runtime_error);
}

// gets on paths BASE_PATH / dir / LVL1_ID / sub_dir
// gets operate on dirs or links - thus additional is_dir and link_container params
void test_lvl2_get(const std::string& test_dir, const std::string& test_sub_dir, IdList(SysfsNvmeTargetHandler::*method)(const Id&) const,
        bool is_dir, const std::string& link_container = {}) {

    using namespace testing;

    auto sysfs = std::make_shared<MockSysfsInterface>();
    SysfsNvmeTargetHandler handler(sysfs);

    SysfsDir dir{};
    dir.name = test_sub_dir;

    // test with invalid parent id
    dir.path = BASE_PATH / test_dir / "invalid_entry" / test_sub_dir;
    EXPECT_CALL(*sysfs, get_dir(BASE_PATH / test_dir / "invalid_entry" / test_sub_dir)).Times(1).WillOnce(Throw(std::runtime_error("")));
    EXPECT_THROW((handler.*method)("invalid_entry"), std::runtime_error);

    // test with an empty directory
    dir.path = BASE_PATH / test_dir / "empty" / test_sub_dir;
    EXPECT_CALL(*sysfs, get_dir(BASE_PATH / test_dir / "empty" / test_sub_dir)).Times(1).WillOnce(Return(dir));
    EXPECT_EQ((handler.*method)("empty").size(), 0);

    // test with a non empty directory
    dir.path = BASE_PATH / test_dir / "non-empty" / test_sub_dir;

    // depending on the param - directory should contain sub directories or links
    auto target_component = (is_dir ? &SysfsDir::dirs : &SysfsDir::links);
    dir.*target_component = {
        Path{BASE_PATH / test_dir / "non-empty" / test_sub_dir / "entry-1"},
        Path{BASE_PATH / test_dir / "non-empty" / test_sub_dir / "entry-2"}
    };
    EXPECT_CALL(*sysfs, get_dir(BASE_PATH / test_dir / "non-empty" / test_sub_dir)).Times(1).WillOnce(Return(dir));
    // in case of links - expect calls to absolute path
    if (!is_dir) {
        EXPECT_CALL(*sysfs, get_absolute_path(BASE_PATH / test_dir / "non-empty" / test_sub_dir / "entry-1")).Times(1).WillOnce(Return(BASE_PATH / link_container / "entry-1"));
        EXPECT_CALL(*sysfs, get_absolute_path(BASE_PATH / test_dir / "non-empty" / test_sub_dir / "entry-2")).Times(1).WillOnce(Return(BASE_PATH / link_container / "entry-2"));
    }
    EXPECT_SAME_IDS({"entry-1", "entry-2"}, (handler.*method)("non-empty"));
}

// adds on paths BASE_PATH / dir / LVL1_ID / sub_dir
// adds operate on dirs or links - thus additional is_dir and link_container params
void test_lvl2_add(const std::string& test_dir, const std::string& test_sub_dir, void(SysfsNvmeTargetHandler::*method)(const Id&, const Id&) const,
        bool is_dir, const std::string& link_container = {}) {

    using namespace testing;

    auto sysfs = std::make_shared<MockSysfsInterface>();
    SysfsNvmeTargetHandler handler(sysfs);

    if (is_dir) {
        EXPECT_CALL(*sysfs, add_dir(BASE_PATH / test_dir / "existing_parent" / test_sub_dir, "new-entry")).Times(1);
    }
    else {
        EXPECT_CALL(*sysfs, add_link(BASE_PATH / test_dir / "existing_parent" / test_sub_dir, "new-entry",
            BASE_PATH / link_container / "new-entry")).Times(1);
    }
    (handler.*method)("existing_parent", "new-entry");

    // test with non-existing parent parent id or if new-entry is duplicated
    if (is_dir) {
        EXPECT_CALL(*sysfs, add_dir(BASE_PATH / test_dir / "non-existing_parent" / test_sub_dir, "new-entry")).WillOnce(Throw(std::runtime_error("")));
    }
    else {
        EXPECT_CALL(*sysfs, add_link(BASE_PATH / test_dir / "non-existing_parent" / test_sub_dir, "new-entry",
            BASE_PATH / link_container / "new-entry")).WillOnce(Throw(std::runtime_error("")));
    }
    EXPECT_THROW((handler.*method)("non-existing_parent", "new-entry"), std::runtime_error);
}

// removes on paths BASE_PATH / dir / LVL1_ID / sub_dir
// removes operate on dirs or links - thus additional is_dir and link_container params
void test_lvl2_remove(const std::string& test_dir, const std::string& test_sub_dir, void(SysfsNvmeTargetHandler::*method)(const Id&, const Id&) const,
        bool is_dir) {

    using namespace testing;

    auto sysfs = std::make_shared<MockSysfsInterface>();
    SysfsNvmeTargetHandler handler(sysfs);

    if (is_dir) {
        EXPECT_CALL(*sysfs, del_dir(BASE_PATH / test_dir / "existing_parent" / test_sub_dir / "entry")).Times(1);
    }
    else {
        EXPECT_CALL(*sysfs, del_link(BASE_PATH / test_dir / "existing_parent" / test_sub_dir / "entry")).Times(1);
    }
    (handler.*method)("existing_parent", "entry");

    // test with non-existing parent parent or entry
    if (is_dir) {
        EXPECT_CALL(*sysfs, del_dir(BASE_PATH / test_dir / "non-existing_parent" / test_sub_dir / "entry")).WillOnce(Throw(std::runtime_error("")));
    }
    else {
        EXPECT_CALL(*sysfs, del_link(BASE_PATH / test_dir / "non-existing_parent" / test_sub_dir / "entry")).WillOnce(Throw(std::runtime_error("")));
    }
    EXPECT_THROW((handler.*method)("non-existing_parent", "entry"), std::runtime_error);
}

}

namespace testing {


TEST(SysfsNvmeTargetHandlerTests, GetHosts) {
    test_lvl1_get(DIR_HOSTS, &SysfsNvmeTargetHandler::get_hosts);
}

TEST(SysfsNvmeTargetHandlerTests, GetPorts) {
    test_lvl1_get(DIR_SUBSYSTEMS, &SysfsNvmeTargetHandler::get_subsystems);
}

TEST(SysfsNvmeTargetHandlerTests, GetSubsystems) {
    test_lvl1_get(DIR_PORTS, &SysfsNvmeTargetHandler::get_ports);
}

TEST(SysfsNvmeTargetHandlerTests, GetSubsystemHosts) {
    test_lvl2_get(DIR_SUBSYSTEMS, DIR_ALLOWED_HOSTS, &SysfsNvmeTargetHandler::get_subsystem_hosts, false, DIR_HOSTS);
}

TEST(SysfsNvmeTargetHandlerTests, GetSubsystemNamespaces) {
    test_lvl2_get(DIR_SUBSYSTEMS, DIR_NAMESPACES, &SysfsNvmeTargetHandler::get_subsystem_namespaces, true);
}

TEST(SysfsNvmeTargetHandlerTests, GetPortSubsystems) {
    test_lvl2_get(DIR_PORTS, DIR_SUBSYSTEMS, &SysfsNvmeTargetHandler::get_port_subsystems, false, DIR_SUBSYSTEMS);
}

TEST(SysfsNvmeTargetHandlerTests, GetPortReferrals) {
    test_lvl2_get(DIR_PORTS, DIR_REFERRALS, &SysfsNvmeTargetHandler::get_port_referrals, true);
}

TEST(SysfsNvmeTargetHandlerTests, AddHost) {
    test_lvl1_add(DIR_HOSTS, &SysfsNvmeTargetHandler::add_host);
}

TEST(SysfsNvmeTargetHandlerTests, AddSubsystem) {
    test_lvl1_add(DIR_SUBSYSTEMS, &SysfsNvmeTargetHandler::add_subsystem);
}

TEST(SysfsNvmeTargetHandlerTests, AddPort) {
    test_lvl1_add(DIR_PORTS, &SysfsNvmeTargetHandler::add_port);
}

TEST(SysfsNvmeTargetHandlerTests, RemoveHost) {
    test_lvl1_remove(DIR_HOSTS, &SysfsNvmeTargetHandler::remove_host);
}

TEST(SysfsNvmeTargetHandlerTests, RemoveSubsystem) {
    test_lvl1_remove(DIR_SUBSYSTEMS, &SysfsNvmeTargetHandler::remove_subsystem);
}

TEST(SysfsNvmeTargetHandlerTests, RemovePort) {
    test_lvl1_remove(DIR_PORTS, &SysfsNvmeTargetHandler::remove_port);
}

TEST(SysfsNvmeTargetHandlerTests, AddPortReferral) {
    test_lvl2_add(DIR_PORTS, DIR_REFERRALS, &SysfsNvmeTargetHandler::add_port_referral, true);
}

TEST(SysfsNvmeTargetHandlerTests, AddPortSubsystem) {
    test_lvl2_add(DIR_PORTS, DIR_SUBSYSTEMS, &SysfsNvmeTargetHandler::add_port_subsystem, false, DIR_SUBSYSTEMS);
}

TEST(SysfsNvmeTargetHandlerTests, AddSubsystemNamespace) {
    test_lvl2_add(DIR_SUBSYSTEMS, DIR_NAMESPACES, &SysfsNvmeTargetHandler::add_subsystem_namespace, true);
}

TEST(SysfsNvmeTargetHandlerTests, AddSubsystemHost) {
    test_lvl2_add(DIR_SUBSYSTEMS, DIR_ALLOWED_HOSTS, &SysfsNvmeTargetHandler::add_subsystem_host, false, DIR_HOSTS);
}

TEST(SysfsNvmeTargetHandlerTests, RemovePortReferral) {
    test_lvl2_remove(DIR_PORTS, DIR_REFERRALS, &SysfsNvmeTargetHandler::remove_port_referral, true);
}

TEST(SysfsNvmeTargetHandlerTests, RemoveSubsystemNamespace) {
    test_lvl2_remove(DIR_SUBSYSTEMS, DIR_NAMESPACES, &SysfsNvmeTargetHandler::remove_subsystem_namespace, true);
}

TEST(SysfsNvmeTargetHandlerTests, RemoveSubsystemHost) {
    test_lvl2_remove(DIR_SUBSYSTEMS, DIR_ALLOWED_HOSTS, &SysfsNvmeTargetHandler::remove_subsystem_host, false);
}

TEST(SysfsNvmeTargetHandlerTests, RemovePortSubsystems) {
    test_lvl2_remove(DIR_PORTS, DIR_SUBSYSTEMS, &SysfsNvmeTargetHandler::remove_port_subsystem, false);
}

TEST(SysfsNvmeTargetHandlerTests, SetSubsystemAllowAllHosts) {
    auto sysfs = std::make_shared<MockSysfsInterface>();
    SysfsNvmeTargetHandler handler(sysfs);

    EXPECT_CALL(*sysfs, set_file(BASE_PATH / DIR_SUBSYSTEMS / "ss-1" / FILE_ALLOW_ALL, "0")).Times(1);
    handler.set_subsystem_allow_all_hosts("ss-1", false);

    EXPECT_CALL(*sysfs, set_file(BASE_PATH / DIR_SUBSYSTEMS / "ss-2" / FILE_ALLOW_ALL, "1")).Times(1);
    handler.set_subsystem_allow_all_hosts("ss-2", true);

    EXPECT_CALL(*sysfs, set_file(BASE_PATH / DIR_SUBSYSTEMS / "invalid" / FILE_ALLOW_ALL, "1")).Times(1).WillOnce(Throw(std::runtime_error("")));
    EXPECT_THROW(handler.set_subsystem_allow_all_hosts("invalid", true), std::runtime_error);
}

TEST(SysfsNvmeTargetHandlerTests, SetSubsystemNamespaceDevice) {
    auto sysfs = std::make_shared<MockSysfsInterface>();
    SysfsNvmeTargetHandler handler(sysfs);

    EXPECT_CALL(*sysfs, set_file(BASE_PATH / DIR_SUBSYSTEMS / "ss-1" / DIR_NAMESPACES / "ns-1" / FILE_DEVICE, "/dev/test")).Times(1);
    handler.set_subsystem_namespace_device("ss-1", "ns-1", "/dev/test");

    EXPECT_CALL(*sysfs, set_file(BASE_PATH / DIR_SUBSYSTEMS / "invalid" / DIR_NAMESPACES / "ns-1" / FILE_DEVICE, "/dev/test"))
        .Times(1).WillOnce(Throw(std::runtime_error("")));
    EXPECT_THROW(handler.set_subsystem_namespace_device("invalid", "ns-1", "/dev/test"), std::runtime_error);
}

TEST(SysfsNvmeTargetHandlerTests, GetSubsystemNamespaceDevice) {
    auto sysfs = std::make_shared<MockSysfsInterface>();
    SysfsNvmeTargetHandler handler(sysfs);

    SysfsFile ret{};
    ret.name = FILE_DEVICE;
    ret.path = BASE_PATH / DIR_SUBSYSTEMS / "ss-1" / DIR_NAMESPACES / "ns-1" / FILE_DEVICE;

    ret.value = "/dev/test";
    EXPECT_CALL(*sysfs, get_file(BASE_PATH / DIR_SUBSYSTEMS / "ss-1" / DIR_NAMESPACES / "ns-1" / FILE_DEVICE)).Times(1).WillOnce(Return(ret));
    EXPECT_EQ(std::string{"/dev/test"}, handler.get_subsystem_namespace_device("ss-1", "ns-1"));

    // real data from sysfs contain additional characters that have to be ignored
    ret.value = "/dev/test \n";
    EXPECT_CALL(*sysfs, get_file(BASE_PATH / DIR_SUBSYSTEMS / "ss-1" / DIR_NAMESPACES / "ns-1" / FILE_DEVICE)).Times(1).WillOnce(Return(ret));
    EXPECT_EQ(std::string{"/dev/test"}, handler.get_subsystem_namespace_device("ss-1", "ns-1"));

    EXPECT_CALL(*sysfs, get_file(BASE_PATH / DIR_SUBSYSTEMS / "invalid" / DIR_NAMESPACES / "ns-1" / FILE_DEVICE)).Times(1).WillOnce(Throw(std::runtime_error("")));
    EXPECT_THROW(handler.get_subsystem_namespace_device("invalid", "ns-1"), std::runtime_error);
}

TEST(SysfsNvmeTargetHandlerTests, SetSubsystemNamespaceEnable) {
    auto sysfs = std::make_shared<MockSysfsInterface>();
    SysfsNvmeTargetHandler handler(sysfs);

    EXPECT_CALL(*sysfs, set_file(BASE_PATH / DIR_SUBSYSTEMS / "ss-1" / DIR_NAMESPACES / "ns-1" / FILE_ENABLE, "0")).Times(1);
    handler.set_subsystem_namespace_enable("ss-1", "ns-1", false);

    EXPECT_CALL(*sysfs, set_file(BASE_PATH / DIR_SUBSYSTEMS / "ss-2" / DIR_NAMESPACES / "ns-2" / FILE_ENABLE, "1")).Times(1);
    handler.set_subsystem_namespace_enable("ss-2", "ns-2", true);

    EXPECT_CALL(*sysfs, set_file(BASE_PATH / DIR_SUBSYSTEMS / "invalid" / DIR_NAMESPACES / "ns-1" / FILE_ENABLE, "1"))
        .Times(1).WillOnce(Throw(std::runtime_error("")));
    EXPECT_THROW(handler.set_subsystem_namespace_enable("invalid", "ns-1", true), std::runtime_error);
}

TEST(SysfsNvmeTargetHandlerTests, SetPortReferralEnable) {
    auto sysfs = std::make_shared<MockSysfsInterface>();
    SysfsNvmeTargetHandler handler(sysfs);

    EXPECT_CALL(*sysfs, set_file(BASE_PATH / DIR_PORTS / "p" / DIR_REFERRALS / "r" / FILE_ENABLE, "0")).Times(1);
    handler.set_port_referral_enable("p", "r", false);

    EXPECT_CALL(*sysfs, set_file(BASE_PATH / DIR_PORTS / "p" / DIR_REFERRALS / "r" / FILE_ENABLE, "1")).Times(1);
    handler.set_port_referral_enable("p", "r", true);

    EXPECT_CALL(*sysfs, set_file(BASE_PATH / DIR_PORTS / "invalid" / DIR_REFERRALS / "r" / FILE_ENABLE, "1"))
        .Times(1).WillOnce(Throw(std::runtime_error("")));
    EXPECT_THROW(handler.set_port_referral_enable("invalid", "r", true), std::runtime_error);
}

TEST(SysfsNvmeTargetHandlerTests, SetPortParams) {
    auto sysfs = std::make_shared<MockSysfsInterface>();
    SysfsNvmeTargetHandler handler(sysfs);

    EXPECT_CALL(*sysfs, set_file(BASE_PATH / DIR_PORTS / "p" / FILE_IP, "ip")).Times(1);
    EXPECT_CALL(*sysfs, set_file(BASE_PATH / DIR_PORTS / "p" / FILE_PORT, "123")).Times(1);
    EXPECT_CALL(*sysfs, set_file(BASE_PATH / DIR_PORTS / "p" / FILE_IP_TYPE, "v4")).Times(1);
    EXPECT_CALL(*sysfs, set_file(BASE_PATH / DIR_PORTS / "p" / FILE_TRANSPORT, "rdma")).Times(1);
    handler.set_port_params("p", std::make_tuple("ip", 123, "v4", "rdma"));

    EXPECT_CALL(*sysfs, set_file(BASE_PATH / DIR_PORTS / "invalid" / FILE_IP, "ip"))
        .Times(AtMost(1)).WillOnce(Throw(std::runtime_error("")));
    EXPECT_CALL(*sysfs, set_file(BASE_PATH / DIR_PORTS / "invalid" / FILE_PORT, "123"))
        .Times(AtMost(1)).WillOnce(Throw(std::runtime_error("")));
    EXPECT_CALL(*sysfs, set_file(BASE_PATH / DIR_PORTS / "invalid" / FILE_IP_TYPE, "v4"))
        .Times(AtMost(1)).WillOnce(Throw(std::runtime_error("")));
    EXPECT_CALL(*sysfs, set_file(BASE_PATH / DIR_PORTS / "invalid" / FILE_TRANSPORT, "rdma"))
        .Times(AtMost(1)).WillOnce(Throw(std::runtime_error("")));
    EXPECT_THROW(handler.set_port_params("invalid", std::make_tuple("ip", 123, "v4", "rdma")), std::runtime_error);
}

TEST(SysfsNvmeTargetHandlerTests, SetPortReferralParams) {
    auto sysfs = std::make_shared<MockSysfsInterface>();
    SysfsNvmeTargetHandler handler(sysfs);

    EXPECT_CALL(*sysfs, set_file(BASE_PATH / DIR_PORTS / "p" / DIR_REFERRALS / "r" / FILE_IP, "ip")).Times(1);
    EXPECT_CALL(*sysfs, set_file(BASE_PATH / DIR_PORTS / "p" / DIR_REFERRALS / "r" / FILE_PORT, "123")).Times(1);
    EXPECT_CALL(*sysfs, set_file(BASE_PATH / DIR_PORTS / "p" / DIR_REFERRALS / "r" / FILE_IP_TYPE, "v4")).Times(1);
    EXPECT_CALL(*sysfs, set_file(BASE_PATH / DIR_PORTS / "p" / DIR_REFERRALS / "r" / FILE_TRANSPORT, "rdma")).Times(1);
    handler.set_port_referral_params("p", "r", std::make_tuple("ip", 123, "v4", "rdma"));

    EXPECT_CALL(*sysfs, set_file(BASE_PATH / DIR_PORTS / "invalid" / DIR_REFERRALS / "r" / FILE_IP, "ip"))
        .Times(AtMost(1)).WillOnce(Throw(std::runtime_error("")));
    EXPECT_CALL(*sysfs, set_file(BASE_PATH / DIR_PORTS / "invalid" / DIR_REFERRALS / "r" / FILE_PORT, "123"))
        .Times(AtMost(1)).WillOnce(Throw(std::runtime_error("")));
    EXPECT_CALL(*sysfs, set_file(BASE_PATH / DIR_PORTS / "invalid" / DIR_REFERRALS / "r" / FILE_IP_TYPE, "v4"))
        .Times(AtMost(1)).WillOnce(Throw(std::runtime_error("")));
    EXPECT_CALL(*sysfs, set_file(BASE_PATH / DIR_PORTS / "invalid" / DIR_REFERRALS / "r" / FILE_TRANSPORT, "rdma"))
        .Times(AtMost(1)).WillOnce(Throw(std::runtime_error("")));
    EXPECT_THROW(handler.set_port_referral_params("invalid", "r", std::make_tuple("ip", 123, "v4", "rdma")), std::runtime_error);
}

TEST(SysfsNvmeTargetHandlerTests, GetPortParams) {
    auto sysfs = std::make_shared<MockSysfsInterface>();
    SysfsNvmeTargetHandler handler(sysfs);

    SysfsFile file_ip{};
    file_ip.name = FILE_IP;
    file_ip.path = BASE_PATH / DIR_PORTS / "p" / FILE_IP;
    // value with additional characters that may be read from the file
    file_ip.value = "ip \n";

    SysfsFile file_port{};
    file_port.name = FILE_PORT;
    file_port.path = BASE_PATH / DIR_PORTS / "p" / FILE_PORT;
    file_port.value = "123 \n";

    SysfsFile file_ip_type{};
    file_ip_type.name = FILE_IP_TYPE;
    file_ip_type.path = BASE_PATH / DIR_PORTS / "p" / FILE_IP_TYPE;
    file_ip_type.value = "v3 \n";

    SysfsFile file_transport{};
    file_transport.name = FILE_TRANSPORT;
    file_transport.path = BASE_PATH / DIR_PORTS / "p" / FILE_TRANSPORT;
    file_transport.value = "rdma \n";

    EXPECT_CALL(*sysfs, get_file(BASE_PATH / DIR_PORTS / "p" / FILE_IP)).Times(1).WillOnce(Return(file_ip));
    EXPECT_CALL(*sysfs, get_file(BASE_PATH / DIR_PORTS / "p" / FILE_PORT)).Times(1).WillOnce(Return(file_port));
    EXPECT_CALL(*sysfs, get_file(BASE_PATH / DIR_PORTS / "p" / FILE_IP_TYPE)).Times(1).WillOnce(Return(file_ip_type));
    EXPECT_CALL(*sysfs, get_file(BASE_PATH / DIR_PORTS / "p" / FILE_TRANSPORT)).Times(1).WillOnce(Return(file_transport));
    auto ret = handler.get_port_params("p");
    EXPECT_EQ(std::get<0>(ret), "ip");
    EXPECT_EQ(std::get<1>(ret), 123);
    EXPECT_EQ(std::get<2>(ret), "v3");
    EXPECT_EQ(std::get<3>(ret), "rdma");

    EXPECT_CALL(*sysfs, set_file(BASE_PATH / DIR_PORTS / "invalid" / FILE_IP, "ip"))
        .Times(AtMost(1)).WillOnce(Throw(std::runtime_error("")));
    EXPECT_CALL(*sysfs, set_file(BASE_PATH / DIR_PORTS / "invalid" / FILE_PORT, "123"))
        .Times(AtMost(1)).WillOnce(Throw(std::runtime_error("")));
    EXPECT_CALL(*sysfs, set_file(BASE_PATH / DIR_PORTS / "invalid" / FILE_IP_TYPE, "v4"))
        .Times(AtMost(1)).WillOnce(Throw(std::runtime_error("")));
    EXPECT_CALL(*sysfs, set_file(BASE_PATH / DIR_PORTS / "invalid" / FILE_TRANSPORT, "rdma"))
        .Times(AtMost(1)).WillOnce(Throw(std::runtime_error("")));
    EXPECT_THROW(handler.get_port_params("invalid"), std::runtime_error);
}

TEST(SysfsNvmeTargetHandlerTests, GetPortReferralParams) {
    auto sysfs = std::make_shared<MockSysfsInterface>();
    SysfsNvmeTargetHandler handler(sysfs);

    SysfsFile file_ip{};
    file_ip.name = FILE_IP;
    file_ip.path = BASE_PATH / DIR_PORTS / "p" / DIR_REFERRALS / "r" / FILE_IP;
    // value with additional characters that may be read from the file
    file_ip.value = "ip \n";

    SysfsFile file_port{};
    file_port.name = FILE_PORT;
    file_port.path = BASE_PATH / DIR_PORTS / "p" / DIR_REFERRALS / "r" / FILE_PORT;
    file_port.value = "123 \n";

    SysfsFile file_ip_type{};
    file_ip_type.name = FILE_IP_TYPE;
    file_ip_type.path = BASE_PATH / DIR_PORTS / "p" / DIR_REFERRALS / "r" / FILE_IP_TYPE;
    file_ip_type.value = "v3 \n";

    SysfsFile file_transport{};
    file_transport.name = FILE_TRANSPORT;
    file_transport.path = BASE_PATH / DIR_PORTS / "p" / DIR_REFERRALS / "r" / FILE_TRANSPORT;
    file_transport.value = "rdma \n";

    EXPECT_CALL(*sysfs, get_file(BASE_PATH / DIR_PORTS / "p" / DIR_REFERRALS / "r" / FILE_IP)).Times(1).WillOnce(Return(file_ip));
    EXPECT_CALL(*sysfs, get_file(BASE_PATH / DIR_PORTS / "p" / DIR_REFERRALS / "r" / FILE_PORT)).Times(1).WillOnce(Return(file_port));
    EXPECT_CALL(*sysfs, get_file(BASE_PATH / DIR_PORTS / "p" / DIR_REFERRALS / "r" / FILE_IP_TYPE)).Times(1).WillOnce(Return(file_ip_type));
    EXPECT_CALL(*sysfs, get_file(BASE_PATH / DIR_PORTS / "p" / DIR_REFERRALS / "r" / FILE_TRANSPORT)).Times(1).WillOnce(Return(file_transport));
    auto ret = handler.get_port_referral_params("p", "r");
    EXPECT_EQ(std::get<0>(ret), "ip");
    EXPECT_EQ(std::get<1>(ret), 123);
    EXPECT_EQ(std::get<2>(ret), "v3");
    EXPECT_EQ(std::get<3>(ret), "rdma");

    // exception on not numeric port number
    file_port.value = "bad number";
    EXPECT_CALL(*sysfs, get_file(BASE_PATH / DIR_PORTS / "p" / DIR_REFERRALS / "r" / FILE_IP)).Times(AtMost(1)).WillOnce(Return(file_ip));
    EXPECT_CALL(*sysfs, get_file(BASE_PATH / DIR_PORTS / "p" / DIR_REFERRALS / "r" / FILE_PORT)).Times(AtMost(1)).WillOnce(Return(file_port));
    EXPECT_CALL(*sysfs, get_file(BASE_PATH / DIR_PORTS / "p" / DIR_REFERRALS / "r" / FILE_IP_TYPE)).Times(AtMost(1)).WillOnce(Return(file_ip_type));
    EXPECT_CALL(*sysfs, get_file(BASE_PATH / DIR_PORTS / "p" / DIR_REFERRALS / "r" / FILE_TRANSPORT)).Times(AtMost(1)).WillOnce(Return(file_transport));
    EXPECT_THROW(handler.get_port_referral_params("p", "r"), std::invalid_argument);

    // check other exceptions
    EXPECT_CALL(*sysfs, set_file(BASE_PATH / DIR_PORTS / "invalid" / DIR_REFERRALS / "r" / FILE_IP, "ip"))
        .Times(AtMost(1)).WillOnce(Throw(std::runtime_error("")));
    EXPECT_CALL(*sysfs, set_file(BASE_PATH / DIR_PORTS / "invalid" / DIR_REFERRALS / "r" / FILE_PORT, "123"))
        .Times(AtMost(1)).WillOnce(Throw(std::runtime_error("")));
    EXPECT_CALL(*sysfs, set_file(BASE_PATH / DIR_PORTS / "invalid" / DIR_REFERRALS / "r" / FILE_IP_TYPE, "v4"))
        .Times(AtMost(1)).WillOnce(Throw(std::runtime_error("")));
    EXPECT_CALL(*sysfs, set_file(BASE_PATH / DIR_PORTS / "invalid" / DIR_REFERRALS / "r" / FILE_TRANSPORT, "rdma"))
        .Times(AtMost(1)).WillOnce(Throw(std::runtime_error("")));
    EXPECT_THROW(handler.get_port_referral_params("invalid", "r"), std::runtime_error);
}

}
