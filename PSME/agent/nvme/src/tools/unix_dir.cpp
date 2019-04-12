/*!
 * @brief Implementation of UnixDir class.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
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
 * @file unix_dir.cpp
 */

#include "tools/unix_dir.hpp"

#include "logger/logger_factory.hpp"

#include <stdexcept>
#include <fstream>

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <linux/limits.h>

using namespace agent::nvme::tools;
using namespace std;

namespace {
DIR* open_dir(const string& path) {
    DIR* dir = opendir(path.c_str());
    if (!dir) {
        throw runtime_error("opendir call failed on " + path + " with error:"
                            + strerror(errno));
    }
    return dir;
}

void close_dir(DIR* dir) {
    if (closedir(dir)) {
        // closedir failed, it is not a disaster, but log an error
        log_error("nvme-agent", string("closedir call failed with error:")
                  + strerror(errno));
    }
}

vector<string> read_dir_entries(const string& path, unsigned char entry_type) {
    vector<string> ret{};
    DIR* dir = open_dir(path);
    dirent* entry{nullptr};
    while ((entry = readdir(dir))) {
        if (entry_type == entry->d_type) {
            string name = entry->d_name;
            if (("." == name) || (".." == name)) {
                continue;
            }
            ret.push_back(name);
        }
    }
    close_dir(dir);
    return ret;
}

}

vector<string> UnixDir::read_directories(const string& path) const {
    return read_dir_entries(path, DT_DIR);
}

vector<string> UnixDir::read_links(const string& path) const {
    return read_dir_entries(path, DT_LNK);
}

string UnixDir::read_first_line(const string& path) const {
    ifstream file(path);
    if (!file.is_open()) {
        throw runtime_error("Unable to open file " + path);
    }
    char buf[PATH_MAX + 1];
    file.getline(buf, PATH_MAX);
    buf[PATH_MAX] = '\0';
    string line{buf};
    if (line.empty()) {
        throw runtime_error("File " + path + " was not configured");
    }
    return line;
}

