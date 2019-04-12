/*!
 * @brief Logging test application
 *
 * @copyright Copyright (c) 2016-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file logger_example_cpp.cpp
 */

#include "logger/logger.hpp"
#include "logger/stream.hpp"
#include "logger/logger_factory.hpp"


#include <iostream>     // cout
#include <array>        // array
#include <memory>

using namespace logger_cpp;

static const size_t TEST_COUNT = 10;

int main(void) {
    std::cout << "Logger basic test" << std::endl;

    const std::array<unsigned char, 4> data = {{0x00, 0xAA, 0x55, 0xFF}};
    const std::vector<unsigned char> vec_data = {0xFF, 0x55, 0xAA, 0x0F};

    for (size_t i = 0; i < TEST_COUNT; i++) {
        log_emergency(LOGUSR, "log test" << " " << i);
        log_alert    (LOGUSR, "log test" << " " << i);
        log_critical (LOGUSR, "log test" << " " << i);
        log_error    (LOGUSR, "log test" << " " << i);
        log_warning  (LOGUSR, "log test" << " " << i);
        log_notice   (LOGUSR, "log test" << " " << i);
        log_info     (LOGUSR, "log test" << " " << i);
        log_debug    (LOGUSR, "log test" << " " << i);
    }

    /* C++ way of logger buffer */
    for (size_t i = 0; i < TEST_COUNT; i++) {
        log_warning(LOGUSR, "log test " << i << " " << "data: " << data);
    }

    for (size_t i = 0; i < TEST_COUNT; i++) {
        log_warning(LOGUSR, "log test " << i << " " << "data: " << vec_data);
    }

    return 0;
}
