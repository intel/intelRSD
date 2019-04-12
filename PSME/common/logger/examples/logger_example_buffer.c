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
#include "logger/buffer.h"

#include <stdlib.h>     // atexit
#include <stdio.h>      // printf

#define TEST_COUNT      100000
#define LOGUSR          g_logusr

static struct logger *LOGUSR = NULL;
static struct logger_stream *stdout_stream = NULL;

void logger_init(void);

static void logger_cleanup(void);

int main(void) {
    logger_init();

    const unsigned char data[] = {0x00, 0xAA, 0x55, 0xFF};

    for (size_t i = 0; i < TEST_COUNT; i++) {
        struct logger_buffer *logbuf = logbuf_create();

        logbuf_write(logbuf, "%s ", "log test");
        logbuf_write(logbuf, "%d ", i);
        logbuf_write(logbuf, "data: ");
        logbuf_array(logbuf, data, sizeof(data));
        log_warning(LOGUSR, "%s", logbuf_string(logbuf));

        logbuf_destroy(logbuf);
    }

    return 0;
}

void logger_init(void) {
    stdout_stream = logger_stream_create(LOGGER_STREAM_STDOUT, "stdout", NULL);

    LOGUSR = logger_create("USR", NULL);
    logger_add_stream(LOGUSR, stdout_stream);

    atexit(logger_cleanup);
}

static void logger_cleanup(void) {
    logger_destroy(LOGUSR);

    logger_stream_destroy(stdout_stream);

    printf("Logger cleaned up\n");
    printf("Finished\n");
}
