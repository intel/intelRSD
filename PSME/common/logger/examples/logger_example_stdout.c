/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
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

#include "logger/logger.h"
#include "logger/stream.h"

#include <stdlib.h>     // atexit
#include <stdio.h>      // printf

#define TEST_COUNT      10

#define LOGUSR          g_logusr
#define LOGOUT          g_logout

/* Extern these */
static struct logger *LOGUSR = NULL;
static struct logger_stream *LOGOUT = NULL;

void logger_init(void);

static void logger_cleanup(void);

int main(void) {
    printf("Logger basic test\n");

    logger_init();

    for (unsigned int i = 0; i < TEST_COUNT; i++) {
        log_emergency(LOGUSR,   "%s %d", "log test", i);
        log_alert(LOGUSR,       "%s %d", "log test", i);
        log_critical(LOGUSR,    "%s %d", "log test", i);
        log_error(LOGUSR,       "%s %d", "log test", i);
        log_warning(LOGUSR,     "%s %d", "log test", i);
        log_notice(LOGUSR,      "%s %d", "log test", i);
        log_info(LOGUSR,        "%s %d", "log test", i);
        log_debug(LOGUSR,       "%s %d", "log test", i);
    }

    return 0;
}

void logger_init(void) {
    LOGOUT = logger_stream_create(LOGGER_STREAM_STDOUT, "stdout", NULL);

    LOGUSR = logger_create("USR", NULL);
    logger_add_stream(LOGUSR, LOGOUT);

    atexit(logger_cleanup);
}

static void logger_cleanup(void) {
    logger_destroy(LOGUSR);
    logger_stream_destroy(LOGOUT);

    printf("Logger cleaned up\n");
}
