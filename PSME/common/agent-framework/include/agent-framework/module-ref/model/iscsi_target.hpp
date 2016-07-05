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
 * @file target.hpp
 * @brief Target model interface
 * */
#ifndef AGENT_FRAMEWORK_MODULE_MODEL_ISCSI_TARGET_HPP
#define AGENT_FRAMEWORK_MODULE_MODEL_ISCSI_TARGET_HPP

#include "agent-framework/module-ref/model/attributes/model_attributes.hpp"
#include "agent-framework/module-ref/model/resource.hpp"
#include "agent-framework/module-ref/enum/storage.hpp"
#include "agent-framework/module-ref/enum/common.hpp"

namespace agent_framework {
namespace model {

/*! Target */
class IscsiTarget : public Resource {
public:
    explicit IscsiTarget(const std::string& parent_uuid = {});
    ~IscsiTarget();

    IscsiTarget(const IscsiTarget&) = default;
    IscsiTarget& operator=(const IscsiTarget&) = default;
    IscsiTarget(IscsiTarget&&) = default;
    IscsiTarget& operator=(IscsiTarget&&) = default;

    using TargetLuns = attribute::Array<attribute::TargetLun>;

    /**
     * @brief Get collection name
     * @return collection name
     */
    static enums::CollectionName get_collection_name() {
        return IscsiTarget::collection_name;
    }

    /**
     * @brief Get component name
     * @return component name
     */
    static enums::Component get_component() {
        return IscsiTarget::component;
    }

    /*!
     * @brief construct an object of class IscsiTarget from JSON
     *
     * @param json the Json::Value deserialized to object
     *
     * @return the newly constructed IscsiTarget object
     */
    static IscsiTarget from_json(const Json::Value& json);

    /*!
     * @brief transform the object to JSon
     *
     * @return the object serialized to Json::Value
     */
    Json::Value to_json() const;

    /*!
     * @brief return an object initiator_iqn of class string
     *
     * @return the initiator_iqn value
     */
    const OptionalField<string>& get_initiator_iqn() const {
        return m_initiator_iqn;
    }

    /*!
     * @brief setter for initiator_iqn attribute
     *
     * @param initiator_iqn of type string
     */
    void set_initiator_iqn(const OptionalField<string>& initiator_iqn) {
        m_initiator_iqn = initiator_iqn;
    }

    /*!
     * @brief return an object target_address of class string
     *
     * @return the target_address value
     */
    const OptionalField<string>& get_target_address() const {
        return m_target_address;
    }

    /*!
     * @brief setter for target_address attribute
     *
     * @param target_address of type string
     */
    void set_target_address(const OptionalField<string>& target_address) {
        m_target_address = target_address;
    }

    /*!
     * @brief return an object target_iqn of class string
     *
     * @return the target_iqn value
     */
    const OptionalField<string>& get_target_iqn() const {
        return m_target_iqn;
    }

    /*!
     * @brief setter for target_port attribute
     *
     * @param target_port of type string
     */
    void set_target_port(const OptionalField<string>& target_port) {
        m_target_port = target_port;
    }

    /*!
     * @brief return an object target_port of class string
     *
     * @return the target_port value
     */
    const OptionalField<string>& get_target_port() const {
        return m_target_port;
    }

    /*!
     * @brief setter for target_iqn attribute
     *
     * @param target_iqn of type string
     */
    void set_target_iqn(const OptionalField<string>& target_iqn) {
        m_target_iqn = target_iqn;
    }

    /*!
     * @brief return an object target_lun of class TargetLuns
     *
     * @return the target_lun value
     */
    const TargetLuns& get_target_lun() const {
        return m_target_lun;
    }

    /*!
     * @brief setter for target_lun attribute
     *
     * @param target_lun of type TargetLuns
     */
    void set_target_lun(const TargetLuns& target_lun) {
        m_target_lun = target_lun;
    }

    /*!
     * @brief adds TargetLUN to the target_luns array
     *
     * @param target_lun of type TargetLun
     */
    void add_target_lun(const attribute::TargetLun& target_lun) {
        m_target_lun.add_entry(target_lun);
    }

private:

    OptionalField<std::string> m_initiator_iqn{};
    OptionalField<std::string> m_target_address{};
    OptionalField<std::string> m_target_port{};
    OptionalField<std::string> m_target_iqn{};
    TargetLuns m_target_lun{};

    static const enums::CollectionName collection_name;
    static const enums::Component component;

};

}
}

#endif /* AGENT_FRAMEWORK_MODULE_MODEL_ISCSI_TARGET_HPP */
