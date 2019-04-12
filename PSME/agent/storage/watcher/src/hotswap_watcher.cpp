
/*!
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
 * @file hotswap_watcher.cpp
 * @brief Implementation of Hotswap watcher thread
 * */

#include "agent/watcher/hotswap_watcher.hpp"


extern "C" {
#include <sys/types.h>
#include <sys/inotify.h>
#include <sys/poll.h>
#include <unistd.h>
#include <linux/limits.h>
}

using namespace agent::storage::watcher;

namespace {

constexpr const int TIMEOUT_MS = 10000;
constexpr const char WATCH_DISK_PATH[] = "/dev/disk/by-id/";
constexpr const char ATA_DEVICE_PREFIX[] = "ata-";
constexpr const std::size_t BUFFER_LENGTH = sizeof(struct inotify_event) + NAME_MAX + 1;

/* INotify event message deleter */
struct INotifyDeleter {
    void operator()(struct inotify_event* event) const {
        delete event;
    }
};

using INotify = std::unique_ptr<struct inotify_event, INotifyDeleter>;
}


void HotswapWatcher::handle_hotswap() {
    try {
        HotswapManager hotswap_manager{m_context};
        hotswap_manager.hotswap_discover_hard_drives();
    }
    catch (const std::exception& error) {
        log_error("hot-swap", "Hotswap exception occurred: " << error.what());
    }
}


bool HotswapWatcher::detect_hotswap(int fd, int wd) {
    INotify ievent_buffer(INotify::pointer(::operator new(::BUFFER_LENGTH)));
    auto ievent = ievent_buffer.get();
    struct pollfd pfds[] = {{fd, POLLIN | POLLPRI, 0}};

    /* poll inotify events */
    if (0 < poll(pfds, sizeof(pfds) / sizeof(pfds[0]), ::TIMEOUT_MS)) {
        if (0 < read(fd, ievent, ::BUFFER_LENGTH)) {
            return (0 < ievent->len) &&
                (wd == ievent->wd) &&
                (0 == strncmp(::ATA_DEVICE_PREFIX, ievent->name, strlen(::ATA_DEVICE_PREFIX)));
        }

        log_warning("hot-swap", "Could not read disks notification.");
    }
    return false;
}


void HotswapWatcher::execute() {
    /* initialize an inotify instance */
    auto fd = inotify_init();
    if (0 > fd) {
        log_warning("hot-swap", "Could not initialize an iNotify instance!");
        return;
    }

    /* add watch directory */
    auto wd = inotify_add_watch(fd, ::WATCH_DISK_PATH, IN_CREATE | IN_DELETE);
    if (0 > wd) {
        log_warning("hot-swap", "Could not watch disks directory.");
        close(fd);
        return;
    }

    /* listen for inotify events and handle them */
    while (is_running()) {
        if (detect_hotswap(fd, wd)) {
            log_info("hot-swap", "Drive's watcher detected!");
            handle_hotswap();
        }
    }

    /* close inotify instance */
    inotify_rm_watch(fd, wd);
    close(fd);
}


HotswapWatcher::~HotswapWatcher() {}
