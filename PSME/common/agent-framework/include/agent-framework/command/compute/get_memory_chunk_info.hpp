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
 * @file command/compute/get_memory_chunk_info.hpp
 * @brief Generic compute GetMemoryChunkInfo command
 * */

#ifndef AGENT_FRAMEWORK_COMMAND_COMPUTE_GET_MEMORY_CHUNK_INFO_HPP
#define AGENT_FRAMEWORK_COMMAND_COMPUTE_GET_MEMORY_CHUNK_INFO_HPP

#include "agent-framework/command/command.hpp"
#include "agent-framework/module-ref/model/memory_chunk.hpp"
#include "agent-framework/module-ref/constants/common.hpp"
#include "agent-framework/module-ref/constants/compute.hpp"

namespace agent_framework {
namespace command {
namespace compute {

/* Forward declaration */
namespace json { class GetMemoryChunkInfo; }

/*! Generic compute command GetMemoryChunkInfo */
class GetMemoryChunkInfo : public Command {
public:
    class Request;
    class Response;

    /*! Tag string for identify agent */
    static constexpr const char AGENT[] = "Compute";

    /*! Tag string for identify command */
    static constexpr const char TAG[] = "getMemoryChunkInfo";

    /*!
     * @brief Execute command with given request and response argument
     *
     * @param[in]   request     Input request argument
     * @param[out]  response    Output response argument
     * */
    virtual void execute(const Request& request, Response& response) = 0;

    /*! Command destructor */
    virtual ~GetMemoryChunkInfo();
protected:
    /*!
     * @brief Execute command with given command arguments
     *
     * @param[in]   in      Input command argument
     * @param[out]  out     Output command argument
     * */
    void execute(const Argument& in, Argument& out) override final {
        execute(static_cast<const Request&>(in), static_cast<Response&>(out));
    }
public:
    /*! Argument request to execute */
    class Request : public Argument {
    private:
        friend class json::GetMemoryChunkInfo;
        std::string m_chunk{};
    public:
        /*!
         * @brief Get memory chunk from request
         * @return uuid string
         * */
        const std::string& get_chunk() const {
            return m_chunk;
        }

        ~Request();
    };

    /*! Argument response from execute */
    class Response : public Argument {
    public:
        /*!
         * Sets memory chunk
         * @param[in] chunk Memory chunk
         * */
        void set_chunk(const model::MemoryChunk& chunk) {
            m_chunk = chunk;
        }
        ~Response();
    private:
        friend class json::GetMemoryChunkInfo;
        model::MemoryChunk m_chunk{};
    };
};

}
}
}

#endif /* AGENT_FRAMEWORK_COMMAND_COMPUTE_GET_MEMORY_CHUNK_INFO_HPP */
