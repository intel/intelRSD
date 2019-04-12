/*!
 * @brief MessageRegistryFile class definition
 *
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file message_registry_file.hpp
 * */

#pragma once



#include "attributes/location.hpp"

#include "agent-framework/module/model/attributes/array.hpp"
#include "agent-framework/module/model/attributes/oem.hpp"
#include "agent-framework/module/utils/utils.hpp"

#include <string>



namespace psme {
namespace rest {
namespace registries {

/*!
 * Class representing redfish message registry file in the data model.
 * */
class MessageRegistryFile {
public:
    using Languages = std::vector<std::string>;
    using Locations = std::vector<Location>;


    MessageRegistryFile();
    MessageRegistryFile(const MessageRegistryFile&) = default;
    MessageRegistryFile(MessageRegistryFile&&) = default;


    ~MessageRegistryFile() = default;


    MessageRegistryFile& operator=(const MessageRegistryFile&) = default;
    MessageRegistryFile& operator=(MessageRegistryFile&&) = default;


    /*!
     * Set message registry file id.
     *
     * @param[in] id Message registry file id.
     * */
    void set_id(const uint64_t& id);


    /*!
     * Set message registry file name.
     *
     * @param[in] name Message registry file name.
     * */
    void set_name(const OptionalField<std::string>& name);


    /*!
     * Set message registry file description.
     *
     *
     * @param[in] description Message registry file description.
     * */
    void set_description(const OptionalField<std::string>& description);


    /*!
     * Set message registry files languages.
     *
     * @param[in] languages Message registry file languages.
     * */
    void set_languages(const Languages& languages);


    /*!
     * Set message registry file registry field.
     *
     * @param[in] registry Message registry file registry field.
     * */
    void set_registry(const OptionalField<std::string>& registry);


    /*!
     * Set message registry file locations.
     *
     * @param[in] locations Message registry file locations.
     * */
    void set_locations(const Locations& locations);


    /*!
     * Get message registry file id.
     *
     * @return Message registry file id.
     * */
    const uint64_t& get_id() const;


    /*!
     * Get message registry file name.
     *
     * @return Message registry file name.
     * */
    const OptionalField<std::string>& get_name() const;


    /*!
     * Get message registry file description.
     *
     * @return Messasge registry file description.
     * */
    const OptionalField<std::string>& get_description() const;


    /*!
     * Get message registry file languages.
     *
     * @return Message registry file languages.
     * */
    const Languages& get_languages() const;


    /*!
     * Get message registry file registry field.
     *
     * @return Message registry file registry field.
     * */
    const OptionalField<std::string>& get_registry() const;


    /*!
     * Get message registry file locations.
     *
     * @return Message registry file locations.
     * */
    const Locations& get_locations() const;


    /*!
     * Get message regisatry file UUID.
     *
     * @return Message registry file UUID.
     * */
    const std::string& get_uuid() const;


private:
    std::uint64_t m_id{};
    OptionalField<std::string> m_name{};
    OptionalField<std::string> m_description{};
    Languages m_languages{};
    OptionalField<std::string> m_registry{};
    Locations m_locations{};

    std::string m_uuid{};
};

}
}
}
