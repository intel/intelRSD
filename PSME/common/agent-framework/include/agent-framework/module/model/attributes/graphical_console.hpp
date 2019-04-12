/*!
* @copyright
* Copyright (c) 2015-2019 Intel Corporation
*
* @copyright
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* @copyright
*    http://www.apache.org/licenses/LICENSE-2.0
*
* @copyright
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* @file graphical_console.hpp
*
* @brief Graphical console attribute object
*/

#pragma once
#include "agent-framework/module/utils/utils.hpp"
#include "agent-framework/module/model/attributes/array.hpp"
#include "agent-framework/module/enum/common.hpp"
#include "json-wrapper/json-wrapper.hpp"

namespace agent_framework {
namespace model {
namespace attribute {

/*! Class representing graphical console attribute */
class GraphicalConsole {
public:
   using SupportedConnections = Array<enums::GraphicalConsoleSupportedType>;

   explicit GraphicalConsole()
   {}

   GraphicalConsole(
           const OptionalField<bool>& enabled,
           const OptionalField<uint32_t>& max_sessions,
           const SupportedConnections& types_supported):
       m_enabled(enabled),
       m_max_sessions(max_sessions),
       m_types_supported(types_supported)
   {}

   ~GraphicalConsole()
   {}

   GraphicalConsole(const GraphicalConsole&) = default;
   GraphicalConsole& operator=(const GraphicalConsole&) = default;
   GraphicalConsole(GraphicalConsole&&) = default;
   GraphicalConsole& operator=(GraphicalConsole&&) = default;


   /*!
    * @brief construct an object of class GraphicalConsole from JSON
    *
    * @param json the json::Json deserialized to object
    *
    * @return the newly constructed GraphicalConsole object
    */
   static GraphicalConsole from_json(const json::Json& json);

   /*!
    * @brief transform the object to JSon
    *
    * @return the object serialized to json::Json
    */
   json::Json to_json() const;


   /*!
    * @brief Get graphical console availability
    * @return if enabled
    */
   const OptionalField<bool>& get_enabled() const {
       return m_enabled;
   }
   /*!
    * @brief Set graphical console availability
    * @param[in] enabled if is available
    */
   void set_enabled(const OptionalField<bool>& enabled) {
       m_enabled = enabled;
   }


   /*!
    * @brief Get number of session that can be established at the same time (optional)
    * @return max allowed sessions
    */
   const OptionalField<uint32_t>& get_max_sessions() const {
       return m_max_sessions;
   }

   /*!
    * @brief Set number of session that can be established at the same time (optional)
    * @param[in] max_sessions max allowed sessions or NIL
    */
   void set_max_sessions(const OptionalField<uint32_t>& max_sessions) {
       m_max_sessions = max_sessions;
   }


   /*!
    * @brief Get supported types of connection (should be optional)
    * @return array with supported types of connections
    */
   const SupportedConnections& get_types_supported() const {
       return m_types_supported;
   }

   /*!
    * @brief Set supported types of connection (should be optional)
    * @param[in] types_supported array with supported types of connections
    */
   void set_types_supported(const SupportedConnections& types_supported) {
       m_types_supported = types_supported;
   }

private:
   OptionalField<bool> m_enabled{};
   OptionalField<uint32_t> m_max_sessions{};
   SupportedConnections m_types_supported{};
};

}
}
}
