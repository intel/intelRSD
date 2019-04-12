/*!
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
 * @file udev_interface_reader.hpp
 */

#pragma once

#include "interface-reader/base_interface_reader.hpp"

namespace interface_reader {

/*!
 * UDev interface reader.
 */
class UdevInterfaceReader : public BaseInterfaceReader {
public:

    explicit UdevInterfaceReader(const std::vector<std::string>& drivers) :
        m_drivers(drivers) { }

    /*!
     * @brief Virtual destructor
     */
    virtual ~UdevInterfaceReader();

    /*!
     * @brief Returns list of interfaces found in the system
     * @return List of names of the found interfaces
     */
    virtual std::vector<std::string> get_interfaces() const;


private:
    std::vector<std::string> m_drivers{};

};

}
