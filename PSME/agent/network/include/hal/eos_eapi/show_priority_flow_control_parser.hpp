/*!
 * @brief Eapi show priority flow control command parser class declaration.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file show_priority_flow_control_parser.hpp
 */

#pragma once

#include <string>

namespace agent {
namespace network {
namespace hal {
namespace eapi {

/*!
 * Class for accessing information obtained from "show priority flow control" command
 */
class ShowPriorityFlowControlParser {
public:
    /*!
     * @brief Constructor
     * @param output output from the switch
     */
    ShowPriorityFlowControlParser(const std::string& output) {
        parse(output);
    }
    /*!
    * @brief Default constructor
    */
    ShowPriorityFlowControlParser() = delete;

    /*!
     * @brief Destructor
     */
    virtual ~ShowPriorityFlowControlParser() = default;

    /*!
     * @brief Is global PFC enabled on the switch
     * @return true if PFC is enabled, false otherwise
     */
    bool is_pfc_enabled() const {
        return m_pfc_enabled;
    }

private:
    void parse(const std::string& output);

    bool m_pfc_enabled{};
};
}
}
}
}
