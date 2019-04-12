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
 * @file partition/file_drive_interface.cpp
 */

#include "partition/file_drive_interface.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <limits>
#include <cstring>

extern "C" {
#include <fcntl.h>
#include <unistd.h>
}

using namespace partition;
using namespace utils;

namespace {
static constexpr char DEV_PATH[] = "/dev/";

class FileSync {
public:
    explicit FileSync(const std::string& file_path) : m_file_path{file_path},
    m_fd{open(file_path.c_str(), O_WRONLY | O_EXCL)} {
        if (m_fd == -1) {
            throw std::runtime_error("Unable to open file " + file_path + strerror(errno));
        }
    }

    void sync() {
        auto ret = fsync(m_fd);
        if(ret) {
            throw std::runtime_error("Unable to sync file " + m_file_path + strerror(errno));
        }
    }

    ~FileSync() {
        // handle EINTR failure
        TEMP_FAILURE_RETRY(close(m_fd));
    }

private:
    const std::string& m_file_path;
    int m_fd;
};

}

FileDriveInterface::~FileDriveInterface() {
}

ByteBuffer FileDriveInterface::read(const std::string& target, uint64_t location, std::size_t num_of_bytes) const {

    auto target_path = DEV_PATH + target;
    std::ifstream file(DEV_PATH + target, std::ios::in | std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error(std::string{"Unable to open drive file for reading: "} + target_path);
    }
    file.seekg(location, std::ios::beg);

    std::vector<uint8_t> read_data(num_of_bytes);
    file.read(reinterpret_cast<char*>(read_data.data()), num_of_bytes);

    auto read_bytes = file.gcount();
    if (read_bytes < 0 || unsigned(read_bytes) != num_of_bytes) {
        std::stringstream str{};
        str << "Unable to read desired number of bytes: read " << read_bytes << " out of " << num_of_bytes;
        throw std::runtime_error(str.str());
    }

    return read_data;
}

void FileDriveInterface::write(const std::string& target, uint64_t location, const ByteBuffer& data) const {

      auto target_path = DEV_PATH + target;
      {
          std::fstream file(target_path, std::ios::in | std::ios::out | std::ios::binary);
          if (!file.is_open()) {
              throw std::runtime_error(std::string{"Unable to open drive file for writing: "} + DEV_PATH + target_path);
          }
          file.seekg(location, std::ios::beg);

          file.write(reinterpret_cast<const char*>(data.data()), data.size());

          uint64_t written_bytes = uint64_t(file.tellp()) - location; // this has to be positive, we are not going backwards
          if (unsigned(written_bytes) != data.size()) {
              std::stringstream str{};
              str << "Unable to write desired number of bytes: written " << written_bytes << " out of " << data.size();
              throw std::runtime_error(str.str());
          }
      }
      // flush kernel buffers
      FileSync fs(target_path);
      fs.sync();
}

void FileDriveInterface::write(const std::string& target, uint64_t location, uint64_t size, DataGenerator generator) const {

    static constexpr unsigned MAX_UNSIGNED = std::numeric_limits<unsigned>().max();

    auto target_path = DEV_PATH + target;
    {
        std::fstream file(target_path, std::ios::in | std::ios::out | std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error(std::string{"Unable to open drive file for writing: "} + DEV_PATH + target_path);
        }
        file.seekg(location, std::ios::beg);

        uint64_t total_written = 0;
        // repeat until all bytes are written
        while (total_written < size) {
            uint64_t missing_bytes_64 = size - total_written;
            unsigned missing_bytes = (missing_bytes_64 > MAX_UNSIGNED ? MAX_UNSIGNED : unsigned(missing_bytes_64));

            // get another portion of data
            auto& buffer = generator();

            // throw if no data available
            if (buffer.size() == 0) {
                throw std::runtime_error("Unable to write desired number of bytes: data source empty");
            }

            // write bytes
            unsigned bytes_to_write = (missing_bytes > buffer.size() ? unsigned(buffer.size()) : missing_bytes);
            file.write(reinterpret_cast<const char*>(buffer.data()), bytes_to_write);

            // check if successful
            uint64_t written_bytes = uint64_t(file.tellp()) - location - total_written; // this has to be positive, we are not going backwards
            total_written += written_bytes;
            if (unsigned(written_bytes) != bytes_to_write) {
                std::stringstream str{};
                str << "Unable to write desired number of bytes, written: " << written_bytes << "/" << bytes_to_write
                    << " (this iteration), " << total_written << " / " << size << " (total)";
                throw std::runtime_error(str.str());
            }
        }
    }
    // flush kernel buffers
    FileSync fs(target_path);
    fs.sync();
}
