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
 * @file rmm/net_reader/net_reader.hpp
 */

#pragma once

#include "net_reader/net_reader_interface.hpp"
#include "sysfs/sysfs_interface.hpp"

#include <memory>

namespace agent {
namespace rmm {
namespace net_reader {

/*!
 * NetReader describing interface of a generic network interface reader
 */
class NetReader : public NetReaderInterface {
public:

    NetReader(): m_sysfs_interface(std::make_shared<::sysfs::SysfsInterface>()) {};
    NetReader(const NetReader&) = default;
    NetReader& operator=(const NetReader&) = default;

    virtual ~NetReader();

    /*!
     * @brief Reads list of all network interfaces
     * @return List of all network interfaces found in the system
     */
    virtual std::vector<RawNetworkInterface> get_interfaces() const override;

private:

    std::shared_ptr<::sysfs::AbstractSysfsInterface> m_sysfs_interface{};

};

}
}
}
