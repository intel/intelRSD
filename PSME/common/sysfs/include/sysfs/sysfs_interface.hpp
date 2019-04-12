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
 * @file sysfs/sysfs_interface.hpp
 */

#pragma once

#include "sysfs/abstract_sysfs_interface.hpp"

namespace sysfs {

/*! Implementation of the AbstractSysfsInterface */
class SysfsInterface : public AbstractSysfsInterface {
public:

    SysfsInterface();
    virtual ~SysfsInterface();

    SysfsInterface(const SysfsInterface&) = default;
    SysfsInterface& operator=(const SysfsInterface&) = default;

    SysfsInterface(SysfsInterface&&) = default;
    SysfsInterface& operator=(SysfsInterface&&) = default;

    /*! Implementations of the pure virtual parent class methods */
    virtual SysfsDir get_dir(const Path& path) const override;
    virtual void add_dir(const Path& path, const std::string& name) const override;
    virtual void del_dir(const Path& path) const override;
    virtual SysfsLink get_link(const Path& path) const override;
    virtual void add_link(const Path& path, const std::string& name, const Path& value) const override;
    virtual void del_link(const Path& path) const override;
    virtual SysfsFile get_file(const Path& path) const override;
    virtual void add_file(const Path& path, const std::string& name, const std::string& value) const override;
    virtual void set_file(const Path& path, const std::string& value) const override;
    virtual void del_file(const Path& path) const override;
    virtual bool dir_exists(const Path& path) const override;
    virtual bool link_exists(const Path& path) const override;
    virtual bool file_exists(const Path& path) const override;
    virtual std::vector<SysfsMount> get_mounts(const Path& path) const override;
    virtual Path get_absolute_path(const Path& input) const override;

};

}
