
/*!
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
 * */

#include "hotswap/hotswap_watcher.hpp"
#include "logger/logger_factory.hpp"

extern "C" {
#include "sys/inotify.h"
#include "sys/types.h"
#include "sys/poll.h"
}

using namespace agent::storage;

void HotswapWatcher::start() {
    if (!m_running) {
        m_running = true;
        log_debug(GET_LOGGER("storage-agent"), "Hotswap disk watcher job started.");
        m_thread = std::thread(&HotswapWatcher::watch, this);
    }
}

void HotswapWatcher::stop() {
    if (m_running) {
        m_running = false;
        if (m_thread.joinable()) {
            m_thread.join();
            log_debug(GET_LOGGER("storage-agent"), "Hotswap disk watcher job done.");
        }
    }
}

void HotswapWatcher::handle_hotswap() {
    try {
        agent::storage::hotswap_discovery::HotswapManager hotswap_manager;
        hotswap_manager.hotswap_discover_hard_drives();
    } catch (const std::runtime_error& error) {
        log_debug(GET_LOGGER("storage-agent"), "Hotswap exception occured "
                << error.what());
    }
}
namespace {
    constexpr int TIMEOUT_MS = 10000;
    //inotify watcher standard event buffer length
    constexpr int BUFFER_LEN = (1024 * ((sizeof (struct inotify_event)) + 16));
    constexpr char DEVICE_MAPPER_PREFIX[] = "dm-";
    constexpr int CHARS_TO_COMPARE = 3;

    union inotify_event_map {
        char buffer[BUFFER_LEN];
        inotify_event event;
    };

    bool is_ret_ok(int ret) {
        return ret > 0;
    }
}

bool HotswapWatcher::detect_hotswap(int fd, int wd) {
    inotify_event_map i_map;
    struct pollfd pfd = {fd, POLLIN | POLLPRI, 0};
    nfds_t nfds = 1;

    if (is_ret_ok(poll(&pfd, nfds, TIMEOUT_MS))) {
        if (0 > read(fd, i_map.buffer, BUFFER_LEN)) {
            log_warning(GET_LOGGER("storage-agent"), "Could not read disks notification.");
            return false;
        }
        if (i_map.event.len && wd == i_map.event.wd &&
                0 != strncmp(DEVICE_MAPPER_PREFIX, i_map.event.name, CHARS_TO_COMPARE)) {
            return true;
        }
    }
    return false;
}

void HotswapWatcher::watch() {
    auto fd = inotify_init();
    if (fd < 0) {
        log_warning(GET_LOGGER("storage-agent"), "Could not watch disks directory.");
        return;
    }
    auto wd = inotify_add_watch(fd, m_watch_disk_dir, IN_CREATE | IN_DELETE);
    while (m_running) {
        if (detect_hotswap(fd, wd)) {
            handle_hotswap();
        }
    }
    inotify_rm_watch(fd, wd);
    close(fd);

}


