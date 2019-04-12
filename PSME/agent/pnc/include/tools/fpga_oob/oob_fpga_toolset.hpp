/*!
 * @brief OobFpgaToolset class which aggregates all tools needed for extracting data out of band.
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
 * @file oob_fpga_toolset.hpp
*/

#pragma once



#include "aardvark/aardvark_oob_tool.hpp"
#include "protocol/pldm/pldm_tool.hpp"
#include "i2c/i2c_access_interface_factory.hpp"



namespace agent {
namespace pnc {
namespace tools {

/*! OobFpgaToolset class declaration */
class OobFpgaToolset final {
public:
    /*! Default constructor */
    OobFpgaToolset() = default;


    /*! Default destructor */
    ~OobFpgaToolset() = default;


    /*!
     * @brief Gets a default toolset used in fro OOB
     * @return Default toolset object
     * */
    static OobFpgaToolset get();


    pldm::PldmToolPtr m_pldm_tool{};
    aardvark::AardvarkOobToolPtr m_aardvark_oob_tool{};
    i2c::I2cAccessInterfacePtr m_i2c_interface{};
};

using OobToolsetPtr = std::shared_ptr<OobFpgaToolset>;

}
}
}
