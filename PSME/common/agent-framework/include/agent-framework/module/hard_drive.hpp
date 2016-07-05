/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
 *
 * @file hard_drive.hpp
 * @brief Hard drive
 * */
#ifndef AGENT_FRAMEWORK_MODULE_HARD_DRIVE_HPP
#define AGENT_FRAMEWORK_MODULE_HARD_DRIVE_HPP

#include "agent-framework/module/block_device.hpp"
#include "agent-framework/module/fru_info.hpp"
#include "agent-framework/module/hard_drive_partition.hpp"
#include "agent-framework/logger_ext.hpp"
#include "json/json.hpp"

#include <string>
#include <vector>

namespace agent_framework {
namespace generic {

using HardDrivePartitionUniquePtr =
    HardDrivePartition::HardDrivePartitionUniquePtr;

/*! Hard Disk Drive */
class HardDrive : public BlockDevice {
    std::string m_name{};
    std::string m_interface{};
    std::string m_type{};
    double m_capacity_gb{};
    uint32_t m_rpm{};
    FruInfo m_fru_info{};

    std::vector<HardDrivePartitionUniquePtr> m_partitions{};
public:

    /*! HardDrive shared pointer */
    using HardDriveSharedPtr = std::shared_ptr<HardDrive>;

    /*! HardDrive weak pointer */
    using HardDriveWeakPtr = std::weak_ptr<HardDrive>;

    /*! Default constructor */
    HardDrive();

    /*!
     * Read Hard Disk Drive configuration
     *
     * param[in] drive_configuration Hard Disk Drive configuration
     * */
    void read_configuration(const json::Value& drive_configuration);

    /*!
     * Gets drive name
     *
     * @return Drive name
     * */
    const std::string& get_name() const { return m_name; }

    /*!
     * Sets drive name
     *
     * @param[in] name Drive name
     * */
    void set_name(const std::string& name) {
        m_name = name;
    }

    /*!
     * Gets FRU info
     *
     * @return FRU info reference
     * */
    const FruInfo& get_fru_info() const { return m_fru_info; }

    /*!
     * Sets FRU info
     *
     * @param[in] fru_info FRU info
     * */
    void set_fru_info(const FruInfo& fru_info) {
        m_fru_info = fru_info;
    }

    /*!
     * Gets the rpm
     *
     * @return rpm
     * */
    uint32_t get_rpm() const { return m_rpm; }

    /*!
     * Sets RPM
     *
     * @param[in] rpm RPM
     * */
    void set_rpm(uint32_t rpm) {
        m_rpm = rpm;
    }

    /*!
     * Gets drive's capacity in GB
     *
     * @return Drive's capacity in GB
     * */
    double get_capacity_gb() const { return m_capacity_gb; }

    /*!
     * Sets drive capacity in GB
     *
     * @param[in] capacity_gb Drive capacity in GB
     * */
    void set_capacity_gb(double capacity_gb) {
        m_capacity_gb = capacity_gb;
    }

    /*!
     * Gets the type
     *
     * @return Drive's type
     * */
    const std::string& get_type() const { return m_type; }

    /*!
     * Sets drive type
     *
     * @param[in] type Drive type
     * */
    void set_type(const std::string& type) {
        m_type = type;
    }

    /*!
     * Gets the interface
     *
     * @return Drive's interface
     * */
    const std::string& get_interface() const { return m_interface; }

    /*!
     * Sets drive interface
     *
     * @param[in] interface Drive interface
     * */
    void set_interface(const std::string& interface) {
        m_interface = interface;
    }

    /*!
     * @brief Adds new partition to hard drive.
     *
     * @param[in] partition  Pointer to new partirion.
     * */
    void add_partition(HardDrivePartitionUniquePtr partition) {
        m_partitions.push_back(std::move(partition));
    }

    /*!
     * @brief Returns vector of partitions.
     *
     * @return Reference to vector of partitions.
     * */
    const std::vector<HardDrivePartitionUniquePtr>& get_partitions() const {
        return m_partitions;
    }

    ~HardDrive();

};

}
}

#endif /* AGENT_FRAMEWORK_MODULE_HARD_DRIVE_HPP */
