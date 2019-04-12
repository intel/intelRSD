/*!
 * @brief MonitorContentBuilder class definition
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
 * @file monitor_content_builder.hpp
 * */


#pragma once



#include "psme/rest/server/response.hpp"
#include "psme/rest/server/http_headers.hpp"

#include "agent-framework/generic/singleton.hpp"
#include "agent-framework/exceptions/not_found.hpp"

#include "json-wrapper/json-wrapper.hpp"

#include <functional>
#include <map>
#include <mutex>
#include <string>



namespace psme {
namespace rest {
namespace endpoint {

/*!
 * A class acting as a content provider for task monitor endpoint.
 * Its purpose is to store stateless functional objects that render to Response
 * objects when they are provided with a context in form of a json::Json object.
 * Statelessness of stored functionals is crucial as they can be called much after
 * state variables have been destroyed (i.e. in a different thread of execution).
 * */
class MonitorContentBuilder : public agent_framework::generic::Singleton<MonitorContentBuilder> {
public:
    using ContentBuilderFunctionType = std::function<psme::rest::server::Response(json::Json)>;
    using MonitorContentBuildersMap = std::map<std::string, ContentBuilderFunctionType>;


    ~MonitorContentBuilder();


    /*!
     * Add content builder.
     *
     * @param[in] task_uuid Task UUID to associate a builder with
     * @param[in] builder Response builder object
     * */
    void add_builder(const std::string& task_uuid, const ContentBuilderFunctionType& builder) {
        std::lock_guard<std::recursive_mutex> lock_guard{m_resource_mutex};
        m_content_builders.insert({task_uuid, builder});
    }


    /*!
     * Get response from a builder.
     *
     * @param[in] task_uuid UUID of task associated with a builder
     * @param[in] json json::Json object representing context to be provided to the builder
     *
     * @return Response object rendered by the builder
     * */
    server::Response get_response(const std::string& task_uuid, json::Json json) {
        std::lock_guard<std::recursive_mutex> lock_guard{m_resource_mutex};

        ContentBuilderFunctionType content_builder = m_content_builders.at(task_uuid);
        auto response = content_builder(json);
        response.set_header(psme::rest::server::http_headers::ContentType::CONTENT_TYPE,
            psme::rest::server::http_headers::ContentType::JSON);
        return response;
    }


private:
    MonitorContentBuildersMap m_content_builders{};
    std::recursive_mutex m_resource_mutex{};
};

}
}
}
