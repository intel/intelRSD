/*!
 * @copyright Copyright (c) 2018-2019 Intel Corporation.
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
 * @file nvmf-discovery/memory_buffer.hpp
 */

#pragma once

#include "logger/logger_factory.hpp"
#include "fabric.hpp"

namespace nvmf_discovery {

/*! forward declaration */
class FabricEndpoint;

/*!
 * Aligned memory buffer
 */
struct MemoryBuffer final {
    static constexpr size_t DEFAULT_SIZE = 4096;
    static constexpr size_t DEFAULT_ALIGNMENT = 4096;
    MemoryBuffer() = default;
    MemoryBuffer(MemoryBuffer&&) noexcept;
    MemoryBuffer& operator=(MemoryBuffer&&) noexcept;
    MemoryBuffer(const MemoryBuffer& other) = delete;
    MemoryBuffer& operator=(const MemoryBuffer& right) = delete;

    void allocate_and_register(fid_domain& domain, size_t size, size_t alignment = DEFAULT_ALIGNMENT);

    void unregister_memory();

    FidMr mr{};
    using pointer = std::unique_ptr<char, void(*)(char*)>;
    pointer buffer{nullptr, [](char* ptr) { free(ptr); }};
    size_t length{};
};

}
