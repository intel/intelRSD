/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file access_interface.hpp
 * @brief GAS access interface
 * */

#pragma once

#include <memory>

/*! Agent namespace */
namespace agent {
/*! PNC namespace */
namespace pnc {
/*! GAS namespace */
namespace gas {

/*! GAS Access Interface class */
class AccessInterface {
public:
    /*! Default constructor */
    AccessInterface();

    /*! Copy constructor */
    AccessInterface(const AccessInterface&) = default;

    /*! Assignment operator */
    AccessInterface& operator=(const AccessInterface&) = default;

    /*!
     * @brief Initializes interface.
     * @param path Path to device.
     */
    virtual void init(const std::string& path) = 0;

    /*!
     * @brief Deinitializes interface.
     */
    virtual void deinit() = 0;

    /*!
     * @brief Method used to write memory region.
     * @param data Data to write.
     * @param size Data size to write.
     * @param offset Region offset to write.
     */
    virtual void write(uint8_t* data, uint32_t size, uint32_t offset) = 0;

    /*!
     * @brief Method used to read memory region.
     * @param data Buffer to store read data.
     * @param size Data size to read.
     * @param offset Region offset to read.
     */
    virtual void read(uint8_t* data, uint32_t size, uint32_t offset) = 0;

    virtual ~AccessInterface();
};

}
}
}
