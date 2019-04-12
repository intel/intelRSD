/*!
 * @brief ManagementController configuration interface.
 *
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
 * @file ipmi/connection_data.hpp
 */

#pragma once

#include <memory>

namespace ipmi {

class ConnectionData {
public:
    using Ptr = std::shared_ptr<ConnectionData>;
    using ConstPtr = std::shared_ptr<const ConnectionData>;

    ConnectionData(const ConnectionData&) = default;
    ConnectionData(ConnectionData&&) = default;
    ConnectionData() = delete;

    /*!
     * @brief Default constructor with interface type given
     * @param interface_type name of the interface
     */
    ConnectionData(const std::string& interface_type);

    /*!
     * @brief Abstract class, appropriate class must be created.
     */
    virtual ~ConnectionData() = 0;

    /*!
     * @brief Get type of the (handling) interface
     *
     * Interface name is used by interface factory to find appropriate builder.
     *
     * @return name of the interface
     */
    virtual const std::string& get_interface_type() const;


    /*!
     * @brief Get info about the connection
     * @return string allowing connection identification (IP, device, etc)
     */
    virtual std::string get_info() const = 0;

private:
    const std::string interface_type;
};

}

