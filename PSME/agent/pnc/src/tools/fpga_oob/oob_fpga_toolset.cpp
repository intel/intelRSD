/*!
* @brief OobFpgaToolset class implementation.
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
* @file oob_fpga_toolset.cpp
*/

#include "tools/fpga_oob/oob_fpga_toolset.hpp"



using namespace agent::pnc::tools;
using namespace agent::pnc::aardvark;


OobFpgaToolset OobFpgaToolset::get() {
    OobFpgaToolset t{};
    t.m_pldm_tool = std::make_shared<pldm::PldmTool>();
    t.m_aardvark_oob_tool = AardvarkOobTool::get_default_instance();
    t.m_i2c_interface = i2c::I2cAccessInterfaceFactory::get_instance().get_interface();
    return t;
}
