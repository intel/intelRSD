/*!
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file sysfs/sysfs_interface.cpp
 */

#include "sysfs/sysfs_interface.hpp"

#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <mntent.h>
#include <fcntl.h>
#include <stdexcept>

using namespace sysfs;

namespace {

static unsigned int MASK = 0755;

bool is_of_type(const Path& path, mode_t type) {
    struct stat st;
    if (lstat(path.to_string().c_str(), &st) != 0) {
        return false;
    }
    return (st.st_mode & S_IFMT) == type;
}

}

SysfsInterface::SysfsInterface() {}

SysfsInterface::~SysfsInterface() {}

SysfsDir SysfsInterface::get_dir(const Path& path) const {
    SysfsDir sd{};
    sd.path = path;
    sd.name = path.basename();

    DIR* dir = opendir(path.to_string().c_str());
    if (!dir) {
        throw std::runtime_error(std::string{"Unable to open directory "} + path.to_string() + ", error: " + strerror(errno));
    }
    dirent* entry{nullptr};
    while ((entry = readdir(dir))) {
        std::string name = entry->d_name;
        if (DT_DIR == entry->d_type && name != "." && name != "..") {
            sd.dirs.push_back(path / name);
        }
        else if (DT_LNK == entry->d_type) {
            sd.links.push_back(path / name);
        }
        else if (DT_REG == entry->d_type) {
            sd.files.push_back(path / name);
        }
    }
    if (closedir(dir)) {
        throw std::runtime_error(std::string{"Unable to close directory "} + path.to_string()
            + ", error: " + strerror(errno));
    }
    return sd;
}

void SysfsInterface::add_dir(const Path& path, const std::string& name) const {
    auto full_path = path / name;
    if (mkdir(full_path.to_string().c_str(), MASK) != 0) {
        throw std::runtime_error(std::string{"Unable to create directory "} + full_path.to_string()
            + ", error: " + strerror(errno));
    }
}

void SysfsInterface::del_dir(const Path& path) const {
    if (rmdir(path.to_string().c_str()) != 0) {
        throw std::runtime_error(std::string{"Unable to remove directory "} + path.to_string()
            + ", error: " + strerror(errno));
    }
}

SysfsLink SysfsInterface::get_link(const Path& path) const {

    // many symlinks in sysfs have size equal to 0, so it is not practical to use lstat
    std::vector<char> buffer(PATH_MAX + 1, '\0');
    auto read_size = readlink(path.to_string().c_str(), buffer.data(), PATH_MAX);
    if (read_size < 0) {
        throw std::runtime_error(std::string{"Unable to read link "} + path.to_string()
            + ", error: " + strerror(errno));
    }

    SysfsLink sl{};
    sl.path = path;
    sl.name = path.basename();
    sl.value = {std::string(buffer.begin(), buffer.begin() + read_size)};
    return sl;
}

void SysfsInterface::add_link(const Path& path, const std::string& name, const Path& value) const {
    auto link_path = path / name;
    if (symlink(value.to_string().c_str(), link_path.to_string().c_str()) != 0) {
        throw std::runtime_error(std::string{"Unable to create link "} + link_path.to_string()
            + " (linked to " + value.to_string() + "), error: " + strerror(errno));
    }
}

void SysfsInterface::del_link(const Path& path) const {

    if (!link_exists(path)) {
        throw std::runtime_error(std::string{"Provided path does not point to a link "} + path.to_string());
    }
    if (unlink(path.to_string().c_str()) != 0) {
        throw std::runtime_error(std::string{"Unable to remove link "} + path.to_string()
            + ", error: " + strerror(errno));
    }
}

SysfsFile SysfsInterface::get_file(const Path& path) const {

    // this is required as open will not fail on directories
    if (!file_exists(path)) {
        throw std::runtime_error(std::string{"Provided path does not point to a file "} + path.to_string());
    }
    int fd = open(path.to_string().c_str(), O_RDONLY);
    if (fd < 0) {
        throw std::runtime_error(std::string{"Unable to open file "} + path.to_string()
            + ", error: " + strerror(errno));
    }
    auto size = lseek(fd, 0, SEEK_END);
    if (size == -1 || lseek(fd, 0, SEEK_SET) == -1) {
        close(fd);
        throw std::runtime_error(std::string{"Unable to lseek file "} + path.to_string()
            + ", error: " + strerror(errno));
    }
    std::vector<char> buffer(size, '\0');
    auto read_size = read(fd, buffer.data(), size);
    close(fd);
    // for many files in sysfs, lseek returns a false size of 4096, so we cannot check `if (read_size == size)`
    if (read_size > size) {
        throw std::runtime_error(std::string{"Too many bytes read from file "} + path.to_string());
    }

    SysfsFile sf{};
    sf.path = path;
    sf.name = path.basename();
    sf.value = std::string(buffer.begin(), buffer.begin() + read_size);

    return sf;
}

void SysfsInterface::add_file(const Path& path, const std::string& name, const std::string& value) const {
    auto full_path = path / name;
    int fd = open(full_path.to_string().c_str(), O_CREAT | O_EXCL | O_WRONLY, MASK);
    if (fd < 0) {
        throw std::runtime_error(std::string{"Unable to create file "} + full_path.to_string()
            + ", error: " + strerror(errno));
    }
    auto written_bytes = write(fd, value.c_str(), value.size());
    close(fd);
    if (written_bytes == -1) {
        throw std::runtime_error(std::string{"Unable to write file "} + full_path.to_string()
            + ", error: " + strerror(errno));
    }
    if (std::size_t(written_bytes) != value.size()) {
        throw std::runtime_error(std::string{"Invalid number of bytes written to the file "} + full_path.to_string()
            + ", error: " + strerror(errno));
    }
}

void SysfsInterface::set_file(const Path& path, const std::string& value) const {
    int fd = open(path.to_string().c_str(), O_TRUNC | O_WRONLY);
    if (fd < 0) {
        throw std::runtime_error(std::string{"Unable to set file "} + path.to_string()
            + " value, error: " + strerror(errno));
    }
    auto written_bytes = write(fd, value.c_str(), value.size());
    close(fd);
    if (written_bytes == -1) {
        throw std::runtime_error(std::string{"Unable to write file "} + path.to_string()
            + ", error: " + strerror(errno));
    }
    if (std::size_t(written_bytes) != value.size()) {
        throw std::runtime_error(std::string{"Invalid number of bytes written to the file "} + path.to_string()
            + ", error: " + strerror(errno));
    }
}

void SysfsInterface::del_file(const Path& path) const {
    if (!file_exists(path)) {
        throw std::runtime_error(std::string{"Provided path does not point to a file "} + path.to_string());
    }
    if (unlink(path.to_string().c_str()) != 0) {
        throw std::runtime_error(std::string{"Unable to remove file "} + path.to_string()
            + ", error: " + strerror(errno));
    }
}

bool SysfsInterface::dir_exists(const Path& path) const {
    return is_of_type(path, S_IFDIR);
}

bool SysfsInterface::link_exists(const Path& path) const {
    return is_of_type(path, S_IFLNK);
}

bool SysfsInterface::file_exists(const Path& path) const {
    return is_of_type(path, S_IFREG);
}


std::vector<SysfsMount> SysfsInterface::get_mounts(const Path& path) const {
    FILE* fp;
    fp = setmntent(path.to_string().c_str(), "r");
    if (fp == nullptr) {
        throw std::runtime_error(std::string{"Unable to open mounts file: "} + path.to_string());
    }

    struct mntent* ent;
    std::vector<SysfsMount> ret{};
    while ((ent = getmntent(fp)) != nullptr) {
        SysfsMount entry{};
        entry.name = ent->mnt_fsname;
        entry.type = ent->mnt_type;
        entry.path = Path(ent->mnt_dir);
        ret.push_back(std::move(entry));
    }

    endmntent(fp);
    return ret;
}


Path SysfsInterface::get_absolute_path(const Path& input) const {
    std::string ret{};
    char* real_path = realpath(input.to_string().c_str(), nullptr);
    if (real_path != nullptr) {
        ret = real_path;
        free(real_path);
        real_path = nullptr;
    }
    else {
        throw std::runtime_error(std::string{"Unable get absolute path for "} + input.to_string()
            + "), error: " + strerror(errno));
    }
    return {ret};
}
