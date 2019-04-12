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

#include <stdlib.h>
#include <stdio.h>

#define TEST_COUNT      1000
#define LOGUSR          logusr
#define LOGDEV          logdev

#define LOGFILE         "example.log"

#define UDP_ADDRESS     "127.0.0.1"
#define UDP_PORT        8888

#define TCP_ADDRESS     "127.0.0.1"
#define TCP_PORT        8889

static void logger_init(void);
static void logger_cleanup(void);

static struct logger *logusr = NULL;
static struct logger *logdev = NULL;

static struct logger_stream *file_stream = NULL;
static struct logger_stream *stdout_stream = NULL;
static struct logger_stream *udp_stream = NULL;
static struct logger_stream *tcp_stream = NULL;

int main(void) {
    logger_init();

    for (unsigned int i = 0; i < TEST_COUNT; i++) {
        log_emergency(LOGUSR,   "%s %d", "log test", i);
        log_emergency(LOGDEV,   "%s %d", "log test", i);
        log_alert(LOGUSR,       "%s %d", "log test", i);
        log_alert(LOGDEV,       "%s %d", "log test", i);
        log_critical(LOGUSR,    "%s %d", "log test", i);
        log_critical(LOGDEV,    "%s %d", "log test", i);
        log_error(LOGUSR,       "%s %d", "log test", i);
        log_error(LOGDEV,       "%s %d", "log test", i);
        log_warning(LOGUSR,     "%s %d", "log test", i);
        log_warning(LOGDEV,     "%s %d", "log test", i);
        log_notice(LOGUSR,      "%s %d", "log test", i);
        log_notice(LOGDEV,      "%s %d", "log test", i);
        log_info(LOGUSR,        "%s %d", "log test", i);
        log_info(LOGDEV,        "%s %d", "log test", i);
        log_debug(LOGUSR,       "%s %d", "log test", i);
        log_debug(LOGDEV,       "%s %d", "log test", i);
    }

    return 0;
}

static void logger_init(void) {
    union logger_options options = {.raw = 0};

    options.option.level = LOG_ERROR;
    options.option.time_format = LOG_TIME_DATE_SEC;
    options.option.color = false;
    options.option.tagging = true;
    options.option.more_debug = false;
    options.option.output_enable = true;

    file_stream = logger_stream_create(LOGGER_STREAM_FILE, "file_log",
            &options);
    logger_stream_open_file(file_stream, LOGFILE);

    options.option.level = LOG_DEBUG;
    options.option.time_format = LOG_TIME_DATE_MS;
    options.option.color = true;
    options.option.tagging = true;
    options.option.more_debug = true;
    options.option.output_enable = false;

    stdout_stream = logger_stream_create(LOGGER_STREAM_STDOUT, "stdout",
            &options);

    options.option.level = LOG_DEBUG;
    options.option.time_format = LOG_TIME_DATE_NS;
    options.option.color = true;
    options.option.tagging = true;
    options.option.more_debug = true;
    options.option.output_enable = true;

    udp_stream = logger_stream_create(LOGGER_STREAM_UDP, "udp", &options);
    logger_stream_open_udp(udp_stream, UDP_ADDRESS, UDP_PORT);

    tcp_stream = logger_stream_create(LOGGER_STREAM_TCP, "tcp", &options);
    logger_stream_open_tcp(tcp_stream, TCP_ADDRESS, TCP_PORT);

    options.option.level = LOG_DEBUG;
    options.option.color = true;
    options.option.tagging = true;
    options.option.more_debug = true;
    options.option.output_enable = true;

    LOGUSR = logger_create("USR", &options);
    logger_add_stream(LOGUSR, file_stream);
    logger_add_stream(LOGUSR, stdout_stream);
    logger_add_stream(LOGUSR, udp_stream);
    logger_add_stream(LOGUSR, tcp_stream);

    options.option.level = LOG_DEBUG;
    options.option.color = true;
    options.option.tagging = true;
    options.option.more_debug = true;
    options.option.output_enable = true;

    LOGDEV = logger_create("DEV", &options);
    logger_add_stream(LOGDEV, file_stream);
    logger_add_stream(LOGDEV, stdout_stream);
    logger_add_stream(LOGDEV, udp_stream);
    logger_add_stream(LOGDEV, tcp_stream);

    atexit(logger_cleanup);
}

static void logger_cleanup(void) {
    logger_destroy(LOGUSR);
    logger_destroy(LOGDEV);

    logger_stream_destroy(stdout_stream);
    logger_stream_destroy(udp_stream);
    logger_stream_destroy(tcp_stream);
    logger_stream_destroy(file_stream);

    printf("Logger cleaned up\n");
    printf("Finished\n");
}
