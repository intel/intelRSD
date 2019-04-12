/*!
 * @brief Load FPGA-oF agent configuration.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License") override;
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License
 *
 * @file fpgaof_loader.hpp
 */

#pragma once



#include "agent-framework/module/loader/loader.hpp"
#include "fpgaof_configuration.hpp"

#include <memory>
#include <string>



namespace agent {
namespace fpgaof {
namespace loader {

/*!
 * @brief Fpga-of Loader.
 */
class FpgaofLoader : public agent_framework::module::loader::Loader {
public:

    /*!
     * @brief Default constructor.
     */
    FpgaofLoader();


    /*!
     * @brief Default destructor.
     */
    ~FpgaofLoader() override = default;


    /*!
     * @brief Load configuration
     * @param[in] json JSON configuration file
     * @return true if success otherwise false
     */
    bool load(const json::Json& json) override;


    /*!
     * @brief Get loaded configuration object.
     * @return Shared pointer to the loaded configuration.
     */
    std::shared_ptr<FpgaofConfiguration> get() const;


private:

    /*! @brief Loaded configuration object. */
    std::shared_ptr<FpgaofConfiguration> m_configuration{};
};

}
}
}

