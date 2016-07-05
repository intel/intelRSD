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
 * @file block_device.hpp
 * @brief BlockDevice interface
 * */
#ifndef AGENT_FRAMEWORK_MODULE_BLOCK_DEVICE_HPP
#define AGENT_FRAMEWORK_MODULE_BLOCK_DEVICE_HPP

#include "agent-framework/module/resources.hpp"

#include <string>

namespace agent_framework {
namespace generic {

/*! BlockDevice class */
class BlockDevice : public Resources {
public:
    explicit BlockDevice(const std::string& device_type);
    virtual ~BlockDevice();

    /*! Enable copy */
    BlockDevice(const BlockDevice&) = default;
    BlockDevice& operator=(const BlockDevice&) = default;
    /*! Enable move */
    BlockDevice(BlockDevice&&) = default;
    BlockDevice& operator=(BlockDevice&&) = default;

    /*!
     * @brief Get block device type
     * @return Block device type
     */
    const std::string& get_device_type() const {
        return m_device_type;
    }

    /*!
     * @brief Get device path
     * @return Device path
     */
    const std::string& get_device_path() const {
        return m_device_path;
    }

    /*!
     * @brief Get device path
     * @param device_path Device path
     */
    void set_device_path(const std::string& device_path) {
        m_device_path = device_path;
    }
private:
    std::string m_device_type;
    std::string m_device_path{};
};

}
}

#endif /* AGENT_FRAMEWORK_MODULE_BLOCK_DEVICE_HPP */
