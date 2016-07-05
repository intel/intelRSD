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
 * @file hard_drive_partition.hpp
 * @brief Hard Drive Partition
 * */

#ifndef AGENT_FRAMEWORK_MODULE_HARD_DRIVE_PARTITION_HPP
#define AGENT_FRAMEWORK_MODULE_HARD_DRIVE_PARTITION_HPP

#include "agent-framework/module/fru_info.hpp"
#include "agent-framework/module/status.hpp"
#include "agent-framework/logger_ext.hpp"
#include "json/json.hpp"

#include "uuid++.hh"

#include <string>
#include <vector>

namespace agent_framework {
namespace generic {

/*! Hard Drive Partition */
class HardDrivePartition {
    const std::string BLOCK_DEVICE_TYPE = "BlockDevice";

    uuid m_name{};
    std::string m_label{};
    std::string m_type{ BLOCK_DEVICE_TYPE };
    double m_capacity_gb{};
    FruInfo m_fru_info{};
    Status m_status{};
public:
    /*! HardDrivePartition unique pointer */
    using HardDrivePartitionUniquePtr = std::unique_ptr<HardDrivePartition>;

    /*! Default constructor */
    HardDrivePartition(){
        m_name.make(UUID_MAKE_V1);
    }

    /*! Creates Hard Drive Partition unique object */
    static HardDrivePartitionUniquePtr make_partition() {
        return HardDrivePartitionUniquePtr{ new HardDrivePartition() };
    }

    /*!
     * @brief Gets unique name of Partition.
     * @return Name of Partition represented by UUID.
     * */
    const std::string get_name() { return std::string(m_name.string()); }

    /*!
     * Gets Hard Drive Partition label
     *
     * @return Hard Drive Partition label
     * */
    const std::string& get_label() const { return m_label; }

    /*!
     * Sets Hard Drive Partition label
     *
     * @param[in] label Hard Drive Partition label
     * */
    void set_label(const std::string& label) {
        m_label = label;
    }

    /*!
     * Gets Hard Drive Partition resource type
     *
     * @return Hard Drive Partition resource type
     * */
    const std::string& get_type() const { return m_type; }

    /*!
     * Sets Hard Drive Partition resource type
     *
     * @param[in] type Hard Drive Partition resource type
     * */
    void set_type(const std::string& type) {
        m_type = type;
    }

    /*!
     * Gets partition's capacity in GB
     *
     * @return partition's capacity in GB
     * */
    double get_capacity_gb() const { return m_capacity_gb; }

    /*!
     * Sets partition capacity in GB
     *
     * @param[in] capacity_gb Partition capacity in GB
     * */
    void set_capacity_gb(const double capacity_gb) {
        m_capacity_gb = capacity_gb;
    }

    ~HardDrivePartition();
};

}
}
#endif /* AGENT_FRAMEWORK_MODULE_HARD_DRIVE_PARTITION_HPP */
