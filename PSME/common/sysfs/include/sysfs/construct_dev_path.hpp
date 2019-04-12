/*!
 * @brief Definition of construct_dev_path utility function.
 *
 * @copyright Copyright (c) 2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License") override;
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file construct_dev_path.hpp
 */
#pragma once



#include "path.hpp"



namespace sysfs {

/*!
 * @brief Construct Dev Path
 * @param args Segments of path
 * @return Path constructed from segments, with /dev/ at the beginning.
 */
template<typename ...Args>
static std::string construct_dev_path(Args... args) {
    static char DEV_PATH[] = "/dev/";
    return Path(DEV_PATH, args...);
}


}