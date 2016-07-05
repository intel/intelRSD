/*!
 * @section LICENSE
 *
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
 * @section DESCRIPTION
 * */

#include "logger/logger.hpp"
#include "logger/stream.hpp"

#include <cstdlib>      // atexit
#include <iostream>     // cout
#include <array>        // array
#include <memory>

using namespace logger_cpp;

static const size_t TEST_COUNT = 10;

/* Extern these */
static Logger* LOGUSR = nullptr;

void logger_init(void);

static void logger_cleanup(void);

int main(void) {
    std::cout << "Logger basic test" << std::endl;

    const std::array<unsigned char, 4> data = {{0x00, 0xAA, 0x55, 0xFF}};
    const std::vector<unsigned char> vec_data = {0xFF, 0x55, 0xAA, 0x0F};

    logger_init();

    for (size_t i = 0; i < TEST_COUNT; i++) {
        log_emergency(LOGUSR,  "log test" << " " << i);
        log_alert(LOGUSR,      "log test" << " " << i);
        log_critical(LOGUSR,   "log test" << " " << i);
        log_error(LOGUSR,      "log test" << " " << i);
        log_warning(LOGUSR,    "log test" << " " << i);
        log_notice(LOGUSR,     "log test" << " " << i);
        log_info(LOGUSR,       "log test" << " " << i);
        log_debug(LOGUSR,      "log test" << " " << i);
    }

    /* C++ way of logger buffer */
    for (size_t i = 0; i < TEST_COUNT; i++) {
        std::stringstream log;

        log << "log test " << i << " ";
        log << "data: ";
        log << data;
        log_warning(LOGUSR, log.str());
    }

    for (size_t i = 0; i < TEST_COUNT; i++) {
        std::stringstream log;

        log << "log test " << i << " ";
        log << "data: ";
        log << vec_data;
        log_warning(LOGUSR, log.str());
    }

    return 0;
}

void logger_init(void) {
    LOGUSR = new Logger("USR");
    LOGUSR->add_stream(new Stream(Stream::Type::STDOUT, "stdout")); // Logger release Stream object

    atexit(logger_cleanup);
}

static void logger_cleanup(void) {
	delete LOGUSR;

    std::cout << "Logger cleaned up" << std::endl;
}
