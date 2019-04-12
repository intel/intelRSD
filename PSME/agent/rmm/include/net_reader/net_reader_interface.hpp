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
 * @file rmm/net_reader/net_reader_interface.hpp
 */

#pragma once

#include "net_reader/raw_network_interface.hpp"

namespace agent {
namespace rmm {
namespace net_reader {

/*!
 * NetReaderInterface describing interface of a generic network interface reader
 */
class NetReaderInterface {
public:

    NetReaderInterface() = default;
    NetReaderInterface(const NetReaderInterface&) = default;
    NetReaderInterface& operator=(const NetReaderInterface&) = default;

    virtual ~NetReaderInterface();

    /*!
     * @brief Reads list of all network interfaces
     * @return List of all network interfaces found in the system
     */
    virtual std::vector<RawNetworkInterface> get_interfaces() const = 0;

};

}
}
}
