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
 * @file interleave_set.hpp
 * @brief Interleave set
 * */
#ifndef AGENT_FRAMEWORK_MODULE_MODEL_ATTRIBUTES_INTERLEAVE_SET_HPP
#define AGENT_FRAMEWORK_MODULE_MODEL_ATTRIBUTES_INTERLEAVE_SET_HPP

#include "agent-framework/module-ref/utils/utils.hpp"

#include <string>
#include <json/json.h>

namespace agent_framework {
namespace model {
namespace attribute {

/*! Interleave set */
class InterleaveSet {
public:

    explicit InterleaveSet();

    InterleaveSet(const std::string& dimm,
                  const std::string& region_id):
                  m_dimm(dimm),
                  m_region_id(region_id)
                  {}

    ~InterleaveSet();

    /*! Enable copy */
    InterleaveSet(const InterleaveSet&) = default;
    InterleaveSet& operator=(const InterleaveSet&) = default;
    InterleaveSet(InterleaveSet&&) = default;
    InterleaveSet& operator=(InterleaveSet&&) = default;

    /*!
     * @brief Sets dimm uuids
     * @param[in] dimm Dimm uuid
     * */
    void set_dimm(const OptionalField<std::string>& dimm) {
        m_dimm = dimm;
    }

    /*!
     * @brief Gets dimm uuid
     * @return Dimm uuid
     * */
    const OptionalField<std::string>& get_dimm() const {
        return m_dimm;
    }

    /*!
     * @brief Sets region id
     * @param[in] region_id Region ID
     * */
    void set_region_id(const OptionalField<std::string>& region_id) {
        m_region_id = region_id;
    }

    /*!
     * @brief Gets region id
     * @return region id
     * */
    const OptionalField<std::string>& get_region_id() const {
        return m_region_id;
    }

    /*!
     * @brief Convert InterleaveSet to JSON object
     * @return JSON representation of InterleaveSet object
     * */
    Json::Value to_json() const;

    /*!
     * @brief construct an object of class Ipv from JSON
     *
     * @param json the Json::Value deserialized to object
     *
     * @return the newly constructed Ipv object
     */
    static InterleaveSet from_json(const Json::Value& json);

private:

    OptionalField<std::string> m_dimm{};
    OptionalField<std::string> m_region_id{};

};

}
}
}

#endif /* AGENT_FRAMEWORK_MODULE_MODEL_ATTRIBUTES_INTERLEAVE_SET_HPP */
