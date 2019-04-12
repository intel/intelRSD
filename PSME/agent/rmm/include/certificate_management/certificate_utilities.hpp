/*!
 * @brief Rmm certificate manager helper functions interface.
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
 * @file certificate_utilities.hpp
 */

#pragma once



#include <string>



extern "C" {
#include <time.h>
}

namespace agent {
namespace rmm {
namespace certificate_utilities {

/*!
 * @brief Check if the provided file path is absolute or not.
 *
 * This functionality is achieved by a very simple condition check: an
 * absolute file path must start with '/' character. Even if further characters refer
 * to '.' or '..', the resulting location is nevertheless always fixed in the filesystem.
 *
 * @param path File path to check.
 * @return True if the path is absolute, false otherwise.
 */
bool is_path_absolute(const std::string& path);


/*!
 * @brief Check if the file referenced by file_path is a regular file.
 * @param fd file descriptor of (opened) file.
 * @return true if regular file.
 */
bool is_regular_file(int fd);


/*!
 * @brief Get file last modification time.
 * @param fd file descriptor of (opened) file.
 * @return timespec structure as documented in lstat system call.
 */
timespec get_file_last_modification_time(int fd);


/*!
 * @brief Check if the file has been modified externally.
 * @param file_time File modification time.
 * @param last_time Time of modification for previously read file.
 * @return True if the file was modified.
 */
bool file_timestamp_changed(const timespec& file_time, const timespec& last_time);

}
}
}
