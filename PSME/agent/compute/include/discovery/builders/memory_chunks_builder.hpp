/*!
 * @brief MemoryChunks builder class interface.
 *
 * @copyright Copyright (c) 2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file memory_chunks_builder.hpp
 */

#pragma once

#include "agent-framework/module/model/memory_chunks.hpp"


namespace agent {
namespace compute {
namespace discovery {

class MemoryChunksBuilder {
public:
    /*!
     * @brief Build default memory chunks object.
     * @param parent_uuid Parent UUID.
     * @return Default memory chunks object.
     */
    static agent_framework::model::MemoryChunks build_default(const Uuid& parent_uuid);

};

}
}
}