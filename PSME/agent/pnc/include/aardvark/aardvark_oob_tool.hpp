/*!
* @brief Tool to use Aardvark adapter in out of band discovery.
*
* @copyright Copyright (c) 2019 Intel Corporation.
*
* Licensed under the Apache License, Version 2.0 (the "License") override;
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License
*
* @file aardvark_oob_tool.hpp
*/


#pragma once



#include "aardvark_interface.hpp"
#include <mutex>

/*! Agent namespace */
namespace agent {
/*! PNC namespace */
namespace pnc {
/*! Aardvark namespace */
namespace aardvark {

/*! Class which allows to use Aardvark adapter in oob discovery */
class AardvarkOobTool {
public:
    /*! Default constructor */
    AardvarkOobTool();


    /*! Default destructor */
    virtual ~AardvarkOobTool();


    /*!
     * @brief Returns instance of default AardvarkOobTool with single Aardvark adapter
     * @return pointer to default AardvarkOobTool object
     */
    static std::shared_ptr<AardvarkOobTool> get_default_instance();


    /*!
     * @brief Initializes single Aardvark device assigned to this tool
     */
    void init_single_device();


    /*!
     * @brief Receives data from master using Aardvark adapter
     * @return vector of bytes received from i2c master
     */
    std::vector<std::uint8_t> slave_receive();


private:
    const int m_slave_receive_timeout_ms = 500;
    std::uint8_t m_slave_address{};

    AardvarkDevice m_device_handle{};
    AardvarkInterface m_aardvark_iface{};

    std::mutex m_mutex{};
};

using AardvarkOobToolPtr = std::shared_ptr<AardvarkOobTool>;

}
}
}
