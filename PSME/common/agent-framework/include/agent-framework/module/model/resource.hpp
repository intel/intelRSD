/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 *
 * @file resources.hpp
 * @brief Resource interface
 * */
#pragma once



#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/constants/common.hpp"
#include "agent-framework/module/enum/common.hpp"
#include "agent-framework/module/utils/utils.hpp"
#include "agent-framework/module/utils/compute_hash.hpp"
#include "agent-framework/generic/obj_reference.hpp"
#include <chrono>
#include <mutex>



namespace agent_framework {
namespace model {

/*! Resource */
class Resource {
public:
    using Reference = agent_framework::generic::ObjReference<Resource, std::recursive_mutex>;


    explicit Resource(const std::string& parent_uuid, enums::Component
    parent_type = enums::Component::None);


    virtual ~Resource();


    /*! Enable copy */
    Resource(const Resource&) = default;


    Resource& operator=(const Resource&) = default;


    Resource(Resource&&) = default;


    Resource& operator=(Resource&&) = default;


    using Collections = attribute::Array<attribute::Collection>;


    /*!
     * @brief Get uuid
     *
     * @return Component uuid
     * */
    const std::string& get_uuid() const {
        return ( m_is_uuid_persistent ? m_persistent_uuid : m_temporary_uuid);
    }


    void set_uuid(const std::string& uuid) {
        m_is_uuid_persistent = false;
        m_temporary_uuid = uuid;
    }

    const std::string& get_temporary_uuid() const {
        return m_temporary_uuid;
    }

    const std::string& get_persistent_uuid() const {
        return m_persistent_uuid;
    }




    /*!
     * @brief Create persistent resource UUID
     *
     * Attempts to create persistent UUIDv5 for the resource based on the unique
     * key set by the function set_unique_key(). If the key was not set prior to
     * calling make_persistent_uuid(), UUIDv1 is generated and no there is no
     * guarantee that upon service restart the resource will get the same UUID.
     * */
    const std::string make_persistent_uuid();


    /*!
     * @brief Create random resource UUID
     * */
    const std::string make_random_uuid();


    /*!
     * @brief Get unique key
     *
     * @return OptionalField wrapper containing the resource unique key
     * */
    const OptionalField<std::string>& get_unique_key() const {
        return m_unique_key;
    }


    /*!
     * @brief Set unique key
     *
     * @param[in] in_key Value of the unique key
     * */
    void set_unique_key(const OptionalField<std::string>& in_key) {
        m_unique_key = in_key;
    }


    /*!
     * @brief Get parent's uuid
     *
     * @return parent's uuid
     * */
    const std::string& get_parent_uuid() const {
        return m_parent_uuid;
    }


    /*!
     * @brief Set parent UUID
     *
     * @param[in] parent Parent UUID
     * */
    void set_parent_uuid(const std::string& parent) {
        m_parent_uuid = parent;
    }


    /*!
     * @brief Gets parent component type
     *
     * @return parent component type
     * */
    enums::Component get_parent_type() const {
        return m_parent_type;
    }


    /*!
     * @brief Sets parent component type
     *
     * @param[in] type parent component type
     * */
    void set_parent_type(enums::Component type) {
        m_parent_type = type;
    }


    /*!
     * Gets status
     *
     * @return Status reference
     * */
    const attribute::Status& get_status() const {
        return m_status;
    }


    /*!
     * @brief Set status
     *
     * @param[in] status Resource's status
     * */
    void set_status(const attribute::Status& status) {
        m_status = status;
    }


    /*!
     * @brief Get subcomponents collection
     *
     * @return reference to collections of subcomponents
     */
    const Collections& get_collections() const {
        return m_collections;
    }


    /*!
     * @brief Set subcomponents collection
     *
     * @param[in] collections subcomponents collection
     */
    void set_collections(const Collections& collections) {
        m_collections = collections;
    }


    /*!
     * @brief Adds subcomponent collection
     *
     * @param[in] collection subcomponent collection
     */
    void add_collection(const attribute::Collection& collection) {
        m_collections.add_entry(collection);
    }


    /*!
     * @brief Set oem data
     *
     * @param oem Oem data
     * */
    void set_oem(const attribute::Oem& oem) {
        m_oem = oem;
    }


    /*!
     * @brief Get oem data
     *
     * @return Oem data
     * */
    const attribute::Oem& get_oem() const {
        return m_oem;
    }


    /*!
     * @brief Tells the caller if the Model represents Chassis object
     * @return Information if Model represents chassis
     * */
    static bool is_chassis() {
        return false;
    }


    /*!
     * @brief Get id
     *
     * @return resource's REST id
     */
    std::uint64_t get_id() const {
        return m_id;
    }


    /*!
     * @brief Set id
     *
     * @param id the REST id to set for resource
     */
    void set_id(const std::uint64_t id) {
        m_id = id;
    }


    /*!
     * @brief get agent id
     *
     * @return resource's agent id, as set by REST application
     */
    const std::string& get_agent_id() const {
        return m_agent_id;
    }


    /*!
     * @brief Set agent_id
     *
     * @param agent_id the resource agent_id as assigned by REST application
     */
    void set_agent_id(const std::string& agent_id) {
        m_agent_id = agent_id;
    }


    struct Hash {
        std::string status{};
        std::string resource_without_status{};
    };


    /*!
     * @brief Get resource hash value
     *
     * @return Resource hash
     * */
    const Hash& get_resource_hash() const {
        return m_hash;
    }


    /*!
     * @brief Update resource touched epoch
     *
     * param epoch Touch epoch
     * */
    void touch(const std::uint64_t epoch) {
        m_touched_at = epoch;
    }


    /*!
     * @brief Verify if resource was updated after a given epoch
     *
     * @param epoch Touch epoch
     *
     * @return True if modified after epoch, else otherwise
     * */
    bool was_touched_after(const std::uint64_t epoch) const {
        return (m_touched_at > epoch);
    }


    /*!
     * @brief Check if the resource UUID is persistent or not
     *
     * @return True if UUID is persistent, false otherwise
     * */
    bool has_persistent_uuid() const {
        return m_is_uuid_persistent;
    }

protected:
    static const char STATUS_KEY[];


    /*!
     * @brief Set resource hash
     * @param[in] json to be used to compute hash
     * */
    void set_resource_hash(const Json::Value& json) {
        if (json.isMember(STATUS_KEY)) {
            Json::Value status = json[STATUS_KEY];
            Json::Value all_but_status = json;
            all_but_status.removeMember(STATUS_KEY);

            m_hash.status = utils::compute_hash(status);
            m_hash.resource_without_status = utils::compute_hash(all_but_status);
        }
        else {
            m_hash.status = "";
            m_hash.resource_without_status = utils::compute_hash(json);
        }
    }


private:
    attribute::Status m_status{};
    std::string m_temporary_uuid{};
    std::string m_persistent_uuid{};
    std::string m_parent_uuid{};
    OptionalField<std::string> m_unique_key{};
    bool m_is_uuid_persistent{false};

    /*
     * According to the GAMI specification not all model objects should provide
     * 'collections' property. However - in the code it is much simpler to create
     * unified handler for listing objects' collections rather than creating
     * specific cases for each model object. Therefore this field is added
     * to every model object, however only in a few (according to the GAMI
     * specification) this field is actually visible in JSON function calls.
     * */
    Collections m_collections{};
    enums::Component m_parent_type{enums::Component::None};

    attribute::Oem m_oem{};

    // These members are being used by REST application
    std::uint64_t m_id{0};
    std::string m_agent_id{};
    std::uint64_t m_touched_at{0};
    Hash m_hash{};
};

}
}
