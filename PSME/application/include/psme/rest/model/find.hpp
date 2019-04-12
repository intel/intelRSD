/*!
 * @copyright
 * Copyright (c) 2018-2019 Intel Corporation
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
 * @file find.hpp
 *
 * @brief Definition of the find API. Can throw exceptions.
 * */


#pragma once



#include "psme/rest/endpoints/utils.hpp"
#include "psme/rest/constants/constants_templates.hpp"
#include "generic/last_template_type.hpp"

#include <string>
#include <sstream>



namespace psme {
namespace rest {
namespace model {

/*!
 * @brief Helper struct for find function that stores state of the find function.
 *
 * FindStateObject is designed to be the result of find function. Its purpose is to store data between each find steps
 * and to retrieve find results.
 *
 * For example:
 *     FindStateObject f = find<Manager, System, Processor>(params);
 *     Uuid u = f.get_uuid();
 *
 * @tparam M type of the object which has to be found.
 */
template<typename M>
struct FindStateObject {
public:

    template<typename T, typename ... Tn>
    friend
    struct FindImpl;


    /*!
     * @brief Transforms FindStateObject object into single resource.
     *
     * @return ObjReference object for found resource type M.
     */
    agent_framework::generic::ObjReference<M, std::recursive_mutex> get_one() const {
        auto& manager = agent_framework::module::get_manager<M>();
        auto uuid = manager.rest_id_to_uuid(m_id, m_parent_uuid);

        return manager.get_entry_reference(uuid);
    }


    /*!
     * @brief returns the found model object.
     *
     * @return a copy of the found M type object.
     * */
    M get() const {
        const auto& manager = agent_framework::module::get_manager<M>();
        auto uuid = manager.rest_id_to_uuid(m_id, m_parent_uuid);

        return manager.get_entry(uuid);
    }


    /*!
     * @brief returns the found object's uuid.
     *
     * @return a the found M type object's uuid.
     * */
    Uuid get_uuid() const {
        const auto& manager = agent_framework::module::get_manager<M>();
        return manager.rest_id_to_uuid(m_id, m_parent_uuid);
    }


private:
    std::uint64_t m_id{};
    std::string m_parent_uuid{};
};

/*!
 * @brief Wrapper structure for find_impl_method recursive template method.
 *
 * It is used as generic template for further specialization.
 *
 * @tparam T FindStateObject type - result of the find.
 * @tparam Tn endpoint types of the find path.
 */
template<typename T, typename ... Tn>
struct FindImpl {
};

/*!
 * @brief Wrapper structure specialization for find_impl_method recursive template method.
 *
 * It is used to resolve template parameter pack and reduce it by 1, performing one find step.
 *
 * @tparam T first type of the endpoint types from parameter pack.
 * @tparam Tn remaining types of endpoints.
 */
template<typename T, typename ... Tn>
struct FindImpl<FindStateObject<typename LastOf<Tn ...>::type>, T, Tn ...> {
    /*!
     * @brief Internal recursive method which performs one step of the find process.
     *
     * Resolves template parameter pack reducing it by 1, and performs one find step.
     *
     * @param m FindStateObject object exchanged between recursive calls.
     * @param params Parameters object container with endpoints to find within.
     * @return Object of FindStateObject type containing results.
     */
    static FindStateObject<typename LastOf<Tn ...>::type>
    find_impl_method(FindStateObject<typename LastOf<Tn ...>::type>& find_state, const server::Parameters& params) {

        std::uint64_t id = psme::rest::endpoint::utils::id_to_uint64(params[constants::get_resource_id<T>()]);
        find_state.m_parent_uuid =
            agent_framework::module::get_manager<T>().rest_id_to_uuid(id, find_state.m_parent_uuid);

        return FindImpl<FindStateObject<typename LastOf<Tn ...>::type>, Tn ...>::find_impl_method(find_state,
                                                                                                  params);
    }
};

/*!
 * @brief Wrapper structure specialization for find_impl_method recursive template method.
 *
 * It is used for last recursive call of the find_impl_method.
 *
 * @tparam T last type.
 */
template<typename T>
struct FindImpl<FindStateObject<typename LastOf<T>::type>, T> {
    /*!
     * @brief Internal method which performs last step of the find process.
     *
     * Performs final find step and returns FindStateObject with results.
     *
     * @param m FindStateObject object exchanged between recursive calls.
     * @param params Parameters object container with endpoints to find within.
     * @return Object of FindStateObject type containing results.
     */
    static FindStateObject<typename LastOf<T>::type>
    find_impl_method(FindStateObject<typename LastOf<T>::type>& find_state, const server::Parameters& params) {

        find_state.m_id = psme::rest::endpoint::utils::id_to_uint64(params[constants::get_resource_id<T>()]);

        return find_state;
    }
};


/*!
 * @brief Finds given resource based on the resources types in the template parameter list. Entry point to find API.
 *
 * @tparam args Subsequent types of resources being on the search path. Last type is target to find.
 * @param params Const reference to Parameters object container with endpoints to find within.
 * @return Object of FindStateObject type containing results.
 */
template<typename ... Args>
FindStateObject<typename LastOf<Args...>::type> find(const server::Parameters& params) {
    FindStateObject<typename LastOf<Args...>::type> find_state;
    return FindImpl<FindStateObject<typename LastOf<Args...>::type>, Args ...>
    ::find_impl_method(find_state, params);
}

}
}
}

