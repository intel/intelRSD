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
 * @file request/compute/get_memory_chunk_info.hpp
 * @brief Compute GetMemoryChunkInfo request
 * */

#ifndef AGENT_FRAMEWORK_MODEL_REQUESTS_COMPUTE_GET_MEMORY_CHUNK_INFO_HPP
#define AGENT_FRAMEWORK_MODEL_REQUESTS_COMPUTE_GET_MEMORY_CHUNK_INFO_HPP

#include <string>

namespace Json {
    class Value;
}

namespace agent_framework {
namespace model {
namespace requests {

/*! GetMemoryChunkInfo request */
class GetMemoryChunkInfo {
public:
    explicit GetMemoryChunkInfo(const std::string& memory);

    static std::string get_command() {
        return "getMemoryChunkInfo";
    }

    /*!
     * @brief Get memory from request
     * @return uuid string
     * */
    const std::string& get_uuid() const {
        return m_chunk;
    }

    /*!
     * @brief Transform request to Json
     *
     * @return created Json value
     */
    Json::Value to_json() const;

    /*!
     * @brief create GetMemoryChunkInfo form Json
     *
     * @param[in] json the input argument
     *
     * @return new GetMemoryChunkInfo
     */
    static GetMemoryChunkInfo from_json(const Json::Value& json);

private:
    std::string m_chunk{};
};

}
}
}

#endif /* AGENT_FRAMEWORK_MODEL_REQUESTS_COMPUTE_GET_MEMORY_CHUNK_INFO_HPP */
