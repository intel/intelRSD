/*!
 * @brief Processor builder class
 *
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
 * @file processor_builder.hpp
 */

#pragma once



#include "abstract_builder.hpp"
#include "agent-framework/module/pnc_components.hpp"



namespace agent {
namespace pnc {
namespace discovery {
namespace builders {

/*!
 * @brief Builder used to create processor objects
 */
class ProcessorBuilder : public AbstractBuilder<agent_framework::model::Processor> {
public:
    using ReturnType = ProcessorBuilder&;


    ProcessorBuilder() = default;


    virtual ~ProcessorBuilder();


    /*!
    * @brief Adds link to a dsp port
    * @return dsp_port_uuid Dsp port uuid
    * */
    virtual ReturnType add_dsp_port_uuid(const Uuid& dsp_port_uuid);


    /*!
    * @brief Updates processor to be in critical discovery state
    * @return Pointer to the builder
    * */
    virtual ReturnType update_critical_state();


    /*!
     * @brief Updates processor FRU info
     * @param fru_info FRU info
     * @return Pointer to the builder
     */
    ReturnType update_fru_info(const agent_framework::model::attribute::FruInfo& fru_info);


private:

    virtual void build_default() override;
};

}
}
}
}