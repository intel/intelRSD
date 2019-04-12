/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @brief Metric model interface
 * @file metric.hpp
 * */

#pragma once



#include "agent-framework/module/model/resource.hpp"
#include "agent-framework/module/enum/common.hpp"

namespace agent_framework {
namespace model {

class Metric : public Resource {
public:

    explicit Metric(const std::string& parent_uuid = {}, enums::Component parent_type = enums::Component::None);


    ~Metric();


    /* Enable copy and move semantics */
    Metric(const Metric&) = default;


    Metric(Metric&&) = default;


    Metric& operator=(const Metric&) = default;


    Metric& operator=(Metric&&) = default;


    /*!
     * Construct a Metric object from json::Json
     *
     * @param[in] json Json object representing a metric
     *
     * @return Metric object
     * */
    static Metric from_json(const json::Json& json);


    /*!
     * Convert a Metric object to json::Json
     *
     * @return json::Json object representing a metric
     * */
    json::Json to_json() const;


    /*!
     * @brief Get collection name
     * @return collection name
     */
    static enums::CollectionName get_collection_name() {
        return Metric::collection_name;
    }


    /*!
     * Get metric component name enum
     *
     * @return Metric component name enum
     * */
    static enums::Component get_component() {
        return Metric::component;
    }

    /*!
     * Get metric value.
     *
     * @return Metric value.
     * */
    const json::Json& get_value() const {
        return m_value;
    }

    /*!
     * Set metric value.
     *
     * @param[in] metric_value metric value.
     * */
    void set_value(const json::Json& metric_value) {
        m_value = metric_value;
    }

    /*!
     * Get measure timestamp
     *
     * @return Measure timestamp.
     * */
    std::uint64_t get_timestamp() const {
        return m_timestamp;
    }

    /*!
     * Set measure timestamp
     *
     * @param[in] timestamp Measure timestamp.
     * */
    void set_timestamp(const std::uint64_t timestamp) {
        m_timestamp = timestamp;
    }

    /*!
     * Get metric definition uuid.
     *
     * @return Metric definition uuid.
     * */
    const std::string& get_metric_definition_uuid() const {
        return m_metric_definition_uuid;
    }

    /*!
     * Set metric definition uuid.
     *
     * @param[in] metric_definition_uuid Metric definition uuid.
     * */
    void set_metric_definition_uuid(const std::string& metric_definition_uuid) {
        m_metric_definition_uuid = metric_definition_uuid;
    }

    /*!
     * Get measured component uuid.
     *
     * @return Measured component uuid.
     * */
    const std::string& get_component_uuid() const {
        return m_component_uuid;
    }

    /*!
     * Set measured component uuid.
     *
     * @param[in] component_uuid Measured component uuid.
     * */
    void set_component_uuid(const std::string& component_uuid) {
        m_component_uuid = component_uuid;
    }

    /*!
     * Get measured component type.
     *
     * @return Measured component type.
     * */
    enums::Component get_component_type() const {
        return m_component_type;
    }

    /*!
     * Set measured component type.
     *
     * @param[in] component_type Measured component type.
     * */
    void set_component_type(const enums::Component component_type) {
        m_component_type = component_type;
    }

private:
    std::string m_component_uuid{};
    std::string m_metric_definition_uuid{};
    enums::Component m_component_type{enums::Component::None};
    std::uint64_t m_timestamp{};
    json::Json m_value{};

    static const enums::Component component;
    static const enums::CollectionName collection_name;
};

}
}
