/*!
 * @copyright
 * Copyright (c) 2015-2018 Intel Corporation
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
 * */


#pragma once



#include "agent-framework/module/compute_components.hpp"
#include "agent-framework/module/network_components.hpp"
#include "psme/rest/endpoints/utils.hpp"

#include <string>
#include <sstream>



namespace psme {
namespace rest {
namespace model {

/*!
 * @brief Find helper class that simplifies accessing Resource specified by rest http request.
 *
 * It purpose is to be used in http endpoint code.
 * For example:
 *     Find<Processor>(req.params["processorID"]).via<Manager>(req.params["managerID"]).
 *         via<System>(req.params["systemID"]).get_one();
 * */
template<typename M>
class Find {
public:
    /*!
     * @brief Constructs Find object that can be used to find resource by ID
     *
     * @param id Rest Id of resource to find (this is not uuid)
     */
    Find(const std::string& id) : m_id(psme::rest::endpoint::utils::id_to_uint64(id)) {}


    /*!
     * @brief Using this method one can specify path to resource.
     *
     * For example:
     *     Find<Processor>(req.params["processorID"]).via<Manager>(manager_uuid).get_one();
     *
     * @param parent_uuid Rest ID of indirect resource being on the path of requested resource
     * */
    Find<M>& via(const Uuid& parent_uuid) {
        m_parent_uuid = parent_uuid;
        return *this;
    }


    /*!
     * @brief Using this method one can specify path to resource.
     *
     * For example:
     *     Find<Processor>(req.params["processorID"]).via<Manager>(req.params["managerID"]).
     *         via<System>(req.params["systemID"]).get_one();
     *
     * @param rest_id Rest ID of indirect resource being on the path of requested resource
     * */
    template<typename T>
    Find<M>& via(const std::string& rest_id) {
        std::uint64_t id = psme::rest::endpoint::utils::id_to_uint64(rest_id);
        m_parent_uuid = agent_framework::module::get_manager<T>().rest_id_to_uuid(id, m_parent_uuid);
        return *this;
    }


    /*!
     * @brief Transforms Find object into single resource
     * */
    agent_framework::generic::ObjReference<M, std::recursive_mutex> get_one() const {
        auto& manager = agent_framework::module::get_manager<M>();
        auto uuid = manager.rest_id_to_uuid(m_id, m_parent_uuid);

        return manager.get_entry_reference(uuid);
    }


    /*!
     * @brief returns the found model object
     *
     * @return a copy of the found M type object
     * */
    M get() const {
        const auto& manager = agent_framework::module::get_manager<M>();
        auto uuid = manager.rest_id_to_uuid(m_id, m_parent_uuid);

        return manager.get_entry(uuid);
    }


    /*!
     * @brief returns the found object's uuid
     *
     * @return a the found M type object's uuid
     * */
    Uuid get_uuid() const {
        const auto& manager = agent_framework::module::get_manager<M>();
        return manager.rest_id_to_uuid(m_id, m_parent_uuid);
    }


protected:
    std::uint64_t m_id{};
    Uuid m_parent_uuid{};
};

}
}
}

