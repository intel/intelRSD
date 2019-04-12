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
 * @file memory_buffer.cpp
 */

#include "nvmf-discovery/memory_buffer.hpp"
#include "nvmf-discovery/fabric_endpoint.hpp"
#include "logger/logger_factory.hpp"

#include <cstring>

namespace nvmf_discovery {

MemoryBuffer::MemoryBuffer(MemoryBuffer&& other) noexcept
    : mr{std::move(other.mr)},
      buffer(std::move(other.buffer)),
      length(other.length) {
    other.length = 0;
}

MemoryBuffer& MemoryBuffer::operator=(MemoryBuffer&& other) noexcept {
    if (this != &other) {
        mr = std::move(other.mr);
        buffer = std::move(other.buffer);
        other.length = 0;
    }
    return *this;
}

void MemoryBuffer::unregister_memory() {
    mr.reset();
}

void MemoryBuffer::allocate_and_register(fid_domain& domain, size_t size, size_t alignment) {
    void* new_buf;
    if (posix_memalign(&new_buf, alignment, size)) {
	    throw std::bad_alloc();
    }
    pointer buf{reinterpret_cast<char*>(new_buf), [](char* ptr) { free(ptr); }};

    mr = register_memory_buffer(domain, buf.get(), size, FI_RECV | FI_SEND |
			FI_REMOTE_READ | FI_REMOTE_WRITE |
			FI_READ | FI_WRITE);
    buffer.swap(buf);
    memset(buffer.get(), 0, size);
    length = size;
}

}
