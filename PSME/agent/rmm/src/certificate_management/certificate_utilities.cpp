/*!
 * @brief Rmm certificate manager helper functions implementation.
 *
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
 * @file certificate_utilities.cpp
 */

#include "certificate_management/certificate_utilities.hpp"
#include <stdexcept>

extern "C" {
#include <sys/stat.h>
}

namespace agent {
namespace rmm {
namespace certificate_utilities {

bool is_path_absolute(const std::string& path) {
    // A very simple condition, but sufficient for this purpose
    return !path.empty() && path[0] == '/';
}


bool is_regular_file(int fd) {
    struct stat stat_buf;

    if (fstat(fd, &stat_buf) < 0) {
        throw std::logic_error("Cannot stat file to get file mode");
    }

    return S_ISREG(stat_buf.st_mode);
}


timespec get_file_last_modification_time(int fd) {
    struct stat stat_buf;

    if (fstat(fd, &stat_buf) < 0) {
        throw std::logic_error("Cannot stat file to get modification time");
    }

    return stat_buf.st_mtim;
}


bool file_timestamp_changed(const timespec& file_time, const timespec& last_time) {
    return (file_time.tv_sec != last_time.tv_sec) || (file_time.tv_nsec != last_time.tv_nsec);
}

}
}
}
