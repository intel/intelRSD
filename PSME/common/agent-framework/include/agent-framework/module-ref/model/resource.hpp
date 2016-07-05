/*!
 * @copyright
 * Copyright (c) 2015-2016 Intel Corporation
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
#ifndef AGENT_FRAMEWORK_MODULE_MODEL_RESOURCES_HPP
#define AGENT_FRAMEWORK_MODULE_MODEL_RESOURCES_HPP

#include "agent-framework/module-ref/model/attributes/model_attributes.hpp"
#include "agent-framework/module-ref/constants/common.hpp"
#include "agent-framework/module-ref/enum/common.hpp"
#include "agent-framework/module-ref/utils/utils.hpp"
#include <chrono>

namespace agent_framework {
namespace model {

/*! Resource */
class Resource {
public:
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
     * @return Component uuid
     * */
    const std::string& get_uuid() const {
        return m_uuid;
    }

    void set_uuid(const std::string& uuid) {
        m_uuid = uuid;
    }

    /*!
     * @brief Get parent's uuid
     *
     * @return parent's uuid
     * */
    const std::string& get_parent_uuid() const {
        return m_parent_uuid;
    }

    void set_parent_uuid(const std::string& parent) {
        m_parent_uuid = parent;
    }

    /*!
     * @brief Gets parent component type
     * @return parent component type
     * */
    enums::Component get_parent_type() const {
        return m_parent_type;
    }

    /*!
     * @brief Sets parent component type
     * @param[in] type parent component type
     * */
    void set_parent_type(enums::Component type) {
        m_parent_type = type;
    }

    /*!
     * Gets status
     * @return Status reference
     * */
    const attribute::Status& get_status() const {
        return m_status;
    }

    /*!
     * @brief Set status
     * @param[in] status Resource's status
     * */
    void set_status(const attribute::Status& status){
        m_status = status;
    }

    /*!
     * @brief Get subcomponents collection
     * @return reference to collections of subcomponents
     */
    const Collections& get_collections() const {
        return m_collections;
    }

    /*!
     * @brief Set subcomponents collection
     * @param[in] collections subcomponents collection
     */
    void set_collections(const Collections& collections) {
        m_collections = collections;
    }

    /*!
     * @brief Adds subcomponent collection
     * @param[in] collection subcomponent collection
     */
    void add_collection(const attribute::Collection& collection) {
        m_collections.add_entry(collection);
    }

    /*!
     * @brief Set oem data
     * @param oem Oem data
     * */
    void set_oem(const attribute::Oem& oem) {
        m_oem = oem;
    }

    /*!
     * @brief Get oem data
     * @return Oem data
     * */
    const attribute::Oem& get_oem() const {
        return m_oem;
    }

    /*! @brief Tells the caller if the Model represents Chassis object
     *  @return information if Model represents chassis
     * */

    static bool is_chassis() { return false; }

    /*!
     * @brief Get id
     *
     * @return resource's REST id
     */
    std::uint64_t get_id() const{
        return m_id;
    }

    /*!
     * @brief Set id
     *
     * @param id the REST id to set for resource
     */
    void set_id(const std::uint64_t id){
        m_id = id;
    }

    /*!
     * @brief get agent id
     *
     * @return resource's agent id, as set by REST application
     */
    const std::string& get_agent_id() const{
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

    /*!
     * @brief Get resource hash value
     * @return Resource hash
     * */
    const std::string& get_resource_hash() const {
        return m_resource_hash;
    }

    /*!
     * @brief Update resource touched epoch
     * param epoch Touch epoch
     * */
    void touch(const std::uint64_t epoch) {
        m_touched_at = epoch;
    }

    /*!
     * @brief Verify if resource was updated after a given epoch
     * @param epoch Touch epoch
     * @return True if modified after epoch, else otherwise
     * */
    bool was_touched_after(const std::uint64_t epoch) const {
        return (m_touched_at > epoch);
    }
protected:
    template<class T>
    using OptionalField = agent_framework::module::utils::OptionalField<T>;

    /*!
     * @brief Set resource hash
     * @param[in] in_hash Hash to be set
     * */
    void set_resource_hash(const std::string& in_hash) {
        m_resource_hash = in_hash;
    }
private:
    attribute::Status m_status{};
    std::string m_uuid{};
    std::string m_parent_uuid{};

    /* According to the GAMI specification not all model objects should provide
     * 'collections' property. However - in the code it is much simpler to create
     * unified handler for listing objects' collections rather than creating
     * specific cases for each model object. Therefore this field is added
     * to every model object, however only in a few (according to the GAMI
     * specification) this field is actually visible in JSON function calls. */
    Collections m_collections{};
    enums::Component m_parent_type{ enums::Component::None };

    attribute::Oem m_oem{};

    // these members are being used by REST application
    std::uint64_t m_id {0};
    std::string m_agent_id {};
    std::uint64_t m_touched_at {0};
    std::string m_resource_hash{};
};



}
}

#endif /* AGENT_FRAMEWORK_MODULE_MODEL_RESOURCES_HPP */
