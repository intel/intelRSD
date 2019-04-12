/*!
* @brief Aardvark out of band tool implementation
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
* @file aardvark_oob_tool.cpp
*/

#include "aardvark/aardvark_oob_tool.hpp"
#include "gas/utils.hpp"
#include "protocol/pldm/utils/pldm_utils.hpp"



using namespace agent::pnc::aardvark;
using namespace agent::pnc::gas;

namespace {
/*! Default AardvarkOobTool object */
static AardvarkOobToolPtr default_aardvark_mf3_oob_tool_ptr = std::make_shared<AardvarkOobTool>();
}


AardvarkOobTool::~AardvarkOobTool() {}


AardvarkOobTool::AardvarkOobTool() {
    m_slave_address = I2C_MF3_BMC_SLAVE_ADDRESS;
}


AardvarkOobToolPtr AardvarkOobTool::get_default_instance() {
    return default_aardvark_mf3_oob_tool_ptr;
}


void AardvarkOobTool::init_single_device() {
    std::unique_lock<std::mutex> lock{m_mutex};
    m_device_handle = m_aardvark_iface.init_device();
    m_aardvark_iface.enable_slave_mode(m_device_handle, m_slave_address);
}


std::vector<std::uint8_t> AardvarkOobTool::slave_receive() {
    std::unique_lock<std::mutex> lock{m_mutex};
    auto resp_bytes = m_aardvark_iface.slave_receive(m_device_handle, m_slave_receive_timeout_ms);

    return resp_bytes;
}
