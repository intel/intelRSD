/*!
 * @brief Mock sysfs interface
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation
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
 * @file mock_sysfs_interface.hpp
 */
#pragma once



#include "sysfs/abstract_sysfs_interface.hpp"



namespace agent {
namespace spdk {
namespace testing {

namespace {
static constexpr const char SYSTEM_FILE_TEST_VALUE[] = "system file test value";
}

class MockSysfsInterface : public ::sysfs::AbstractSysfsInterface {
public:
    MockSysfsInterface() {}


    ~MockSysfsInterface() {}


    sysfs::SysfsFile get_file(const sysfs::Path&) const override {
        sysfs::SysfsFile sf{};
        sf.value = SYSTEM_FILE_TEST_VALUE;
        return sf;
    }


    sysfs::SysfsDir get_dir(const sysfs::Path&) const override { return sysfs::SysfsDir{}; };


    void add_dir(const sysfs::Path&, const std::string&) const override {};


    void del_dir(const sysfs::Path&) const override {};


    sysfs::SysfsLink get_link(const sysfs::Path&) const override { return sysfs::SysfsLink{}; };


    void add_link(const sysfs::Path&, const std::string&, const sysfs::Path&) const override {};


    void del_link(const sysfs::Path&) const override {};


    void add_file(const sysfs::Path&, const std::string&, const std::string&) const override {};


    void set_file(const sysfs::Path&, const std::string&) const override {};


    void del_file(const sysfs::Path&) const override {};


    bool dir_exists(const sysfs::Path&) const override { return false; };


    bool link_exists(const sysfs::Path&) const override { return false; };


    bool file_exists(const sysfs::Path&) const override { return false; };


    std::vector<sysfs::SysfsMount>
    get_mounts(const sysfs::Path&) const override { return std::vector<sysfs::SysfsMount>{}; };


    sysfs::Path get_absolute_path(const sysfs::Path&) const override { return sysfs::Path{}; };
};

}
}
}
