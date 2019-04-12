/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
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
 * @file tests/mock_drive_interface.hpp
 */

#include "partition/abstract_drive_interface.hpp"
#include "utils/byte_buffer.hpp"

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wweak-vtables"
#endif

/*
 * Mock implementation using a vector buffer.
 * It allows reading any number from bytes (if outside the buffer, will fill data with zeroes)
 * Writing is possible only inside the stored buffer.
 */
class MockDriveInterface final : public partition::AbstractDriveInterface {
public:

    virtual ~MockDriveInterface() {}

    virtual utils::ByteBuffer read(const std::string&, uint64_t location, std::size_t num_of_bytes) const override {
        std::size_t max_index = std::min(location + num_of_bytes, mocked_data.size());
        long bytes_to_read = max_index - location;
        utils::ByteBuffer data(num_of_bytes);
        if (bytes_to_read > 0) {
            for (long i = 0; i < bytes_to_read; ++i) {
                data[i] = mocked_data[std::size_t(location) + i];
            }
        }
        return data;
    }

    virtual void write(const std::string&, uint64_t location, const utils::ByteBuffer& data) const override {
        std::size_t max_index = std::min(location + data.size(), mocked_data.size());
        long bytes_to_write = max_index - location;
        if (bytes_to_write >= 0) {
            for (long i = 0; i < bytes_to_write; ++i) {
                mocked_data[std::size_t(location) + i] = data[i];
            }
        }
    }

    virtual void write(const std::string&, uint64_t, uint64_t, partition::DataGenerator) const override {
        throw std::runtime_error("Not implemented in mocks");
    }

    mutable utils::ByteBuffer mocked_data{};
};

#ifdef __clang__
#pragma clang diagnostic pop
#endif
