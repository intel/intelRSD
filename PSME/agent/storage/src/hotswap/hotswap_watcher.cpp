
/*!
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
#include <sys/inotify.h>
#include <sys/types.h>
#include <sys/poll.h>
#include <unistd.h>
#include <string.h>
#include <linux/limits.h>
}

using namespace agent::storage;
using namespace agent::storage::hotswap_discovery;

namespace {
    constexpr int TIMEOUT_MS = 10000;
    constexpr int BUFFER_LEN = sizeof(struct inotify_event) + NAME_MAX + 1;
    constexpr char WATCH_DISK_PATH[] = "/dev/disk/by-id/";
    constexpr char DEVICE_MAPPER_PREFIX[] = "dm-";

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
        HotswapManager hotswap_manager{};
        hotswap_manager.hotswap_discover_hard_drives();
    }
    catch (const std::exception& error) {
        log_debug(GET_LOGGER("storage-agent"), "Hotswap exception occured "
                << error.what());
    }
}

bool HotswapWatcher::detect_hotswap(int fd, int wd) {
    INotify ievent_buffer(INotify::pointer((::operator new (BUFFER_LEN))));
    auto ievent = ievent_buffer.get();
    struct pollfd pfds[] = {{fd, POLLIN | POLLPRI, 0}};

    /* poll inotify events */
    if (0 < poll(pfds, sizeof(pfds)/sizeof(pfds[0]), TIMEOUT_MS)) {
        if (0 < read(fd, ievent, BUFFER_LEN)) {
            return (0 < ievent->len) && (wd == ievent->wd) &&
                (0 != strncmp(DEVICE_MAPPER_PREFIX, ievent->name,
                strlen(DEVICE_MAPPER_PREFIX)));
        }
        log_warning(GET_LOGGER("storage-agent"),
            "Could not read disks notification.");
    }
    return false;
}

void HotswapWatcher::execute() {
    /* init an inotify instance */
    auto fd = inotify_init();
    if (0 > fd) {
        log_warning(GET_LOGGER("storage-agent"),
            "Could not initialize an inotify instance");
        return;
    }
    /* add watch directory */
    auto wd = inotify_add_watch(fd, WATCH_DISK_PATH, IN_CREATE | IN_DELETE);
    if (0 > wd) {
        log_warning(GET_LOGGER("storage-agent"),
            "Could not watch disks directory.");
        close(fd);
        return;
    }
    /* listen for inotify events and handle them */
    while (is_running()) {
        if (detect_hotswap(fd, wd)) {
            handle_hotswap();
        }
    }
    /* close inotify instance */
    inotify_rm_watch(fd, wd);
    close(fd);
}

HotswapWatcher::~HotswapWatcher() { }
