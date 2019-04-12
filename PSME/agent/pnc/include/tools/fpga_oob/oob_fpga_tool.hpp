/*!
* @brief OobFpgaTool class used for out of band data extracting using PLDM commmands.
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
* @file oob_fpga_tool.hpp
*/

#pragma once



#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/pnc_components.hpp"
#include "tools/fpga_oob/oob_fpga_toolset.hpp"
#include "protocol/pldm/command/get_pldm_version.hpp"
#include "protocol/pldm/command/get_pdr_repository_info.hpp"
#include "protocol/pldm/command/get_pdr.hpp"
#include "protocol/pldm/command/get_fru_record_set_pdr.hpp"
#include "protocol/pldm/command/get_pldm_commands.hpp"
#include "protocol/pldm/command/get_terminus_uid.hpp"



namespace agent {
namespace pnc {
namespace tools {

/*! OobFpgaTool class declaration */
class OobFpgaTool {
public:
    /*! Default constructor */
    OobFpgaTool() {}


    /*! Enable copy */
    OobFpgaTool(const OobFpgaTool&) {}


    OobFpgaTool& operator=(const OobFpgaTool&) { return *this; }


    /*! Default destructor */
    virtual ~OobFpgaTool();


    /*!
     * @brief Reads PLDM version from PLDM-capable device on the provided port
     * @param[out] pldm_version Reference to the GetPldmVersion object to be filled with data
     * @param[in] port Downstream port used to read PLDM version
     * @return True if successful
     * */
    bool get_pldm_version(pldm::command::GetPldmVersion& pldm_version, const agent_framework::model::Port& port) const;


    /*!
     * @brief Reads PDR Repository Info structure from PLDM-capable device on the provided port
     * @param[out] pdr_info Reference to the GetPdrRepositoryInfo object to be filled with data
     * @param[in] port Downstream port used to read PDR Repository Info
     * @return True if successful
     * */
    bool get_pdr_repo_info(pldm::command::GetPdrRepositoryInfo& pdr_info, const agent_framework::model::Port& port) const;


    /*!
     * @brief Reads PDR from PLDM-capable device on the provided port
     * @param[out] pdr Reference to the GetPdr object to be filled with data
     * @param[in] port Downstream port used to read PDR
     * @return True if successful
     * */
    bool get_pdr(pldm::command::GetPdr& pdr, const agent_framework::model::Port& port) const;


    /*!
     * @brief Reads Terminus Uid from PLDM-capable device on the provided port
     * @param[out] terminus_uid Reference to the GetTerminusUid object to be filled with data
     * @param[in] port Downstream port used to read Terminus Uid
     * @return True if successful
     * */
    bool get_terminus_uid(pldm::command::GetTerminusUid& terminus_uid, const agent_framework::model::Port& port) const;


    /*!
     * @brief Reads PLDM commands from PLDM-capable device on the provided port
     * @param[out] pldm_commands Reference to the GetPldmCommands object to be filled with data
     * @param[in] port Downstream port used to read PLDM commands
     * @return True if successful
     * */
    bool get_pldm_commands(pldm::command::GetPldmCommands& pldm_commands, const agent_framework::model::Port& port) const;
};

using OobFpgaToolPtr = std::shared_ptr<OobFpgaTool>;

}
}
}
