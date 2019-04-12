/*!
 * @brief MessageRegistryFileManager class definition
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
 * @file message_registry_file_manager.hpp
 * */

#pragma once



#include "psme/rest/registries/model/message_registry_file.hpp"

#include "agent-framework/generic/singleton.hpp"

#include <cstdint>
#include <mutex>
#include <string>
#include <vector>
#include <map>



namespace psme {
namespace rest {
namespace registries {

/*!
 * Class responsible for managing all message registry files within the redfish server.
 * */
class MessageRegistryFileManager : public agent_framework::generic::Singleton<MessageRegistryFileManager> {
public:
    using MessageRegistryFiles = std::vector<MessageRegistryFile>;
    using UuidToRegistryFileMap = std::map<std::string, MessageRegistryFile>;
    using IdToUuidMap = std::map<std::uint64_t, std::string>;


    /*!
     * Destructor.
     * */
    virtual ~MessageRegistryFileManager();


    /*!
     * Add a registry file to the manager.
     *
     * @param[in] message_registry_file Message registry file object to be added.
     * */
    void add_registry_file(MessageRegistryFile message_registry_file);


    /*!
     * Get a message registry file with a given UUID.
     *
     * @param[in] file_uuid UUID of a file to look for.
     *
     * @return Message registry file object.
     * */
    const MessageRegistryFile& get_file_by_uuid(const std::string& file_uuid);


    /*!
     * Get a message registry file with a given id.
     *
     * @param[in] file_id Id of a file to look for.
     *
     * @return Message registry file object.
     * */
    const MessageRegistryFile& get_file_by_id(uint64_t file_id);


    /*!
     * Get all message registry file objects.
     *
     * @return Vector containing all message registry file objects.
     * */
    MessageRegistryFiles get_files() const;


    /*!
     * Get the number of stored message registry file objects.
     *
     * @return Number of stored message registry file objects.
     * */
    std::size_t get_count() const;


protected:
    /*!
     * Get the maximal assigned id.
     *
     * @return Maximal assigned id.
     * */
    uint64_t get_max_id() const;


    /*!
     * Increment maximal id by 1.
     * */
    void increment_max_id();


private:
    std::recursive_mutex m_mutex{};
    UuidToRegistryFileMap m_uuid_to_registry_file_map{};
    IdToUuidMap m_id_to_uuid_map{};
    std::uint64_t m_max_id{0u};
};

}
}
}
