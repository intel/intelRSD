/*!
 * @brief General database interface
 *
 * @header{License}
 * @copyright Copyright (c) 2016-2017 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @header{Filesystem}
 * @file database.hpp
 */

#include "psme/rest/model/handlers/file_database.hpp"

#include <logger/logger.hpp>
#include <logger/logger_factory.hpp>

#include <safe-string/safe_lib.hpp>

#include <cassert>
#include <cstring>
#include <mutex>
#include <memory>

extern "C" {
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
}



namespace psme {
namespace rest {
namespace model {
namespace handler {

const std::string FileDatabase::EXT{".db"};
const std::string FileDatabase::EMPTY{""};

const unsigned FileDatabase::VALUE_LEN = 512;

/*! FAIL condition for assert. Raise assertion when given condition is met. */
constexpr bool FAIL(const char*, bool when = true) { return !when; }

std::recursive_mutex FileDatabase::mutex{};

std::string FileDatabase::PATH{""};

FileDatabase::FileDatabase(const std::string& _name) :
    Database(_name),
    iterating_state(IteratingState::NOT_STARTED) {
    if (PATH.empty()) {
        PATH = get_directory();
    }
    assert (!PATH.empty());
}

FileDatabase::~FileDatabase() {
    if (iterating_state != IteratingState::NOT_STARTED) {
        FileDatabase::end();
    }
}

bool FileDatabase::start() {
    /*! @todo Double locking on iterating state */
    std::lock_guard<std::recursive_mutex> lock(mutex);

    if (iterating_state != IteratingState::NOT_STARTED) {
        log_error(GET_LOGGER("db"), "Iterating in progress");
        assert (FAIL("In progress"));
        return false;
    }
    iterating_state = IteratingState::STARTED;
    return true;
}

bool FileDatabase::next(Serializable& key, Serializable& value) {
    std::lock_guard<std::recursive_mutex> lock(mutex);

    switch (iterating_state) {
        case IteratingState::NOT_STARTED:
            log_error(GET_LOGGER("db"), "Iterating not started");
            assert (FAIL("Not iterating"));
            return false;
        case IteratingState::STARTED:
            iterated_files.clear();
            foreach(key, [this](const std::string& stripped_name) -> bool {
                iterated_files.push_back(stripped_name);
                return true;
            });
            current_file = iterated_files.begin();
            iterating_state = IteratingState::ITERATE;
            break;
        case IteratingState::ITERATE:
            break;
        case IteratingState::NO_MORE_DATA:
            return false;
        default:
            assert (FAIL("Unreachable code"));
            return false;
    }

    while (iterated_files.end() != current_file) {
        const std::string& stripped_name = *current_file;
        current_file++;

        /* parse key, it was checked just before, so it is proper */
        key.unserialize(stripped_name);

        /* parse value, if not valid, then log error and check next one */
        std::string data{};
        if (!read_file(full_name(stripped_name), data)) {
            log_error(GET_LOGGER("db"), "Cannot read data for " << stripped_name);
            continue;
        } else if (!value.unserialize(data)) {
            log_error(GET_LOGGER("db"), "Incorrect data for " << stripped_name << ":: " << data);
            continue;
        }

        /* key/value are filled without any issue */
        return true;
    }
    /* no more data */
    return false;
}

void FileDatabase::end() {
    std::lock_guard<std::recursive_mutex> lock(mutex);

    switch (iterating_state) {
        case IteratingState::NOT_STARTED:
            log_error(GET_LOGGER("db"), "Not iterating");
            assert (FAIL("Not iterating"));
            return;
        case IteratingState::STARTED:
            log_error(GET_LOGGER("db"), "Iterating started but not proceeded");
            assert (FAIL("Not iterating"));
            break;
        case IteratingState::ITERATE:
            log_warning(GET_LOGGER("db"), "Not all files were checked while iterating");
            break;
        case IteratingState::NO_MORE_DATA:
            break;

        default:
            assert (FAIL("Unreachable code"));
            return;
    }
    iterating_state = IteratingState::NOT_STARTED;
}

bool FileDatabase::get(const Serializable& key, Serializable& value) {
    std::lock_guard<std::recursive_mutex> lock(mutex);
    std::string data{};
    if (!read_file(full_name(key.serialize()), data)) {
        return false;
    }
    return value.unserialize(data);
}

bool FileDatabase::put(const Serializable& key, const Serializable& value) {
    std::lock_guard<std::recursive_mutex> lock(mutex);
    return save_file(full_name(key.serialize()), value.serialize());
}

bool FileDatabase::remove(const Serializable& key) {
    std::lock_guard<std::recursive_mutex> lock(mutex);
    return remove_file(full_name(key.serialize()));
}

bool FileDatabase::invalidate(const Serializable& key) {
    std::lock_guard<std::recursive_mutex> lock(mutex);
    return invalidate_file(full_name(key.serialize()));
}

Database::EntityValidity FileDatabase::get_validity(const Serializable& key, std::chrono::seconds interval) {
    std::lock_guard<std::recursive_mutex> lock(mutex);
    return file_validity(full_name(key.serialize()), interval);
}

unsigned FileDatabase::cleanup(Serializable& key, std::chrono::seconds interval) {
    return foreach(key, [this, interval](const std::string& stripped_name) -> bool {
        std::string file_name = full_name(stripped_name);
        switch (file_validity(file_name, interval)) {
            case EntityValidity::ERROR:
                return false;
            case EntityValidity::VALID:
                return invalidate_file(file_name);
            case EntityValidity::INVALID:
                return false;
            case EntityValidity::OUTDATED:
                return remove_file(file_name);
            default:
                assert (FAIL("Unreachable code"));
                return false;
        }
    });
}

unsigned FileDatabase::wipe_outdated(Serializable& key, std::chrono::seconds interval) {
    return foreach(key, [this, interval](const std::string& stripped_name) -> bool {
        std::string file_name = full_name(stripped_name);
        switch (file_validity(file_name, interval)) {
            case EntityValidity::ERROR:
            case EntityValidity::VALID:
            case EntityValidity::INVALID:
                return false;
            case EntityValidity::OUTDATED:
                return remove_file(file_name);
            default:
                assert (FAIL("Unreachable code"));
                return false;
        }
    });
}

unsigned FileDatabase::drop(Serializable& key) {
    return foreach(key, [this](const std::string& stripped_name) -> bool {
        std::string file_name = full_name(stripped_name);
        return remove_file(file_name);
    });
}

unsigned FileDatabase::foreach(Serializable& key, ForeachFunction function) {
    std::lock_guard<std::recursive_mutex> lock(mutex);

    DIR* directory = opendir(PATH.c_str());
    if (nullptr == directory) {
        log_error(GET_LOGGER("db"), "Cannot open directory " << PATH
                                    << " for reading:: " << strerror(errno));
        return 0;
    }

    /* appoint all names to be processed */
    IteratedFiles names;
    while (nullptr != directory) {
        errno = 0;
        struct dirent* dir_entry = readdir(directory);

        /* no more entries in the directory */
        if (nullptr == dir_entry) {
            if (errno) {
                log_warning(GET_LOGGER("db"), "Reading directory " << PATH
                                              << ":: " << strerror(errno));
            }
            closedir(directory);
            directory = nullptr;
            break;
        }

        /* only plain files contains DB entries */
        if (DT_REG != dir_entry->d_type) {
            continue;
        }

        std::string stripped_name = strip_name(dir_entry->d_name);
        /* file from other db.. or even non-db file */
        if (stripped_name.empty()) {
            continue;
        }

        /* parse key */
        if (!key.unserialize(stripped_name)) {
            continue;
        }
        names.push_back(stripped_name);
    }

    /* process all names */
    unsigned num = 0;
    for (IteratedFiles::const_iterator it = names.begin(); it != names.end(); it++) {
        if (function(*it)) {
            num++;
        }
    }
    return num;
}

std::string FileDatabase::full_name(const std::string& stripped_name) const {
    if (stripped_name.empty()) {
        return EMPTY;
    }
    std::string ret = PATH + "/";
    if (!get_name().empty()) {
        ret.append(get_name() + ".");
    }
    ret.append(stripped_name);
    ret.append(EXT);
    return ret;
}

std::string FileDatabase::strip_name(const std::string& d_name) const {
    /* there's no PATH in the name to strip */
    std::string stripped = d_name;

    if (!get_name().empty()) {
        /* strip database name */
        if (stripped.substr(0, get_name().length()) != get_name()) {
            return EMPTY;
        }
        stripped.erase(0, get_name().length());

        /* strip '.' */
        if (stripped.substr(0, 1) != ".") {
            return EMPTY;
        }
        stripped.erase(0, 1);
    }

    /* strip extension */
    if ((stripped.length() < EXT.length()) || (stripped.substr(stripped.length() - EXT.length()) != EXT)) {
        return EMPTY;
    }
    stripped.erase(stripped.length() - EXT.length());

    return stripped;
}

bool FileDatabase::read_file(const std::string& file_name, std::string& data) {
    if (file_name.empty()) {
        log_error(GET_LOGGER("db"), "No file name given");
        return false;
    }

    char buf[VALUE_LEN];
    FILE* file = fopen(file_name.c_str(), "rb");
    if (!file) {
        log_debug(GET_LOGGER("db"), "Cannot read file " << file_name << ":: " << strerror(errno));
        return false;
    }

    size_t bytes = fread(&buf, sizeof(char), sizeof(buf), file);
    bool ok;
    if (ferror(file)) {
        log_error(GET_LOGGER("db"), "Cannot read file " << file_name << ":: " << strerror(errno));
        ok = false;
    }
    else if (!feof(file)) {
        log_error(GET_LOGGER("db"), "File " << file_name << " is longer than allowed");
        ok = false;
    } else {
        data = std::string(buf, bytes);
        ok = true;
    }
    fclose(file);

    return ok;
}

bool FileDatabase::save_file(const std::string& file_name, const std::string& data) {
    if (file_name.empty()) {
        log_error(GET_LOGGER("db"), "No file name given");
        return false;
    }

    if (data.empty()) {
        log_error(GET_LOGGER("db"), "Writing empty data forbidden, file " << file_name);
        return false;
    }

    std::shared_ptr<FILE> file{fopen(file_name.c_str(), "wb"), fclose};
    if (!file) {
        log_error(GET_LOGGER("db"), "Cannot write file " << file_name);
        return false;
    }
    int file_descriptor = fileno(file.get());

    size_t bytes = fwrite(data.data(), sizeof(char), data.length(), file.get());
    if (data.length() != bytes) {
        log_error(GET_LOGGER("db"), "Cannot write whole file " << file_name << ", only " << bytes << " written");
        return false;
    }

    /* check current mode of the file */
    struct stat stats;
    if (0 != fstat(file_descriptor, &stats)) {
        log_error(GET_LOGGER("db"), "Cannot set mode for " << file_name << ":: " << strerror(errno));
        return false;
    }
    /* already marked as valid */
    if (S_ISVTX == (stats.st_mode & S_ISVTX)) {
        return true;
    }

    /* remove sticky bit from the file, this alter stats.st_ctim as well */
    return 0 == fchmod(file_descriptor, stats.st_mode | S_ISVTX);
}

bool FileDatabase::remove_file(const std::string& file_name) {
    if (file_name.empty()) {
        log_error(GET_LOGGER("db"), "No file name given");
        return false;
    }

    /* POSIX compatible.. */
    return (::remove(file_name.c_str()) == 0);
}

Database::EntityValidity FileDatabase::file_validity(const std::string& file_name, std::chrono::seconds interval) {
    struct stat stats;
    errno = 0;
    if (0 != stat(file_name.c_str(), &stats)) {
        log_error(GET_LOGGER("db"), "Cannot check mode for " << file_name << ":: " << strerror(errno));
        return EntityValidity::ERROR;
    }
    /* still marked as valid */
    if (S_ISVTX == (stats.st_mode & S_ISVTX)) {
        return EntityValidity::VALID;
    }

    /* If times match.. then file is outdated! */
    if (stats.st_ctim.tv_sec + interval.count() <= time(nullptr)) {
        return EntityValidity::OUTDATED;
    }
    return EntityValidity::INVALID;
}

bool FileDatabase::invalidate_file(const std::string& file_name) {
    if (file_name.empty()) {
        return false;
    }

    std::shared_ptr<FILE> file{fopen(file_name.c_str(), "r+b"), fclose};
    if (!file) {
        log_error(GET_LOGGER("db"), "Cannot read file" << file_name);
        return false;
    }
    int file_descriptor = fileno(file.get());

    /* check current mode of the file */
    struct stat stats;
    errno = 0;
    if (0 != fstat(file_descriptor, &stats)) {
        log_error(GET_LOGGER("db"), "Cannot check mode for " << file_name << ":: " << strerror(errno));
        return false;
    }
    /* already marked as invalid */
    if (S_ISVTX != (stats.st_mode & S_ISVTX)) {
        return false;
    }
    /* remove sticky bit from the file, this alter stats.st_ctim as well */
    return 0 == fchmod(file_descriptor, stats.st_mode & (~S_ISVTX));
}

std::string FileDatabase::get_directory() {
    char file_name[128] = "/var/opt/psme/check_dir_XXXXXX";
    int fd;

    errno = 0;
    fd = mkstemp(file_name);
    if (fd < 0) {
        log_warning(GET_LOGGER("db"), "Cannot create tmp file " << file_name << ":: " << strerror(errno));
        ::strcpy_s(file_name, sizeof(file_name), "/tmp/database_XXXXXX");
        fd = mkstemp(file_name);
        if (fd < 0) {
            log_error(GET_LOGGER("db"), "Cannot get temporary directory name:: " << strerror(errno));
            assert (FAIL("Cannot get temporary directory name"));
            return "/tmp";
        }
        /* replace temp file with directory writable for all */
        close(fd);
        ::remove(file_name);

        log_info(GET_LOGGER("db"), "Database created in temporary " << file_name);

        mkdir(file_name, 01777);
        ::strcat_s(file_name, sizeof(file_name), "/check_dir_XXXXXX");
        fd = mkstemp(file_name);
    }

    if (fd < 0) {
        log_error(GET_LOGGER("db"), "Cannot create database checker " << file_name << ":: " << strerror(errno));
        assert (FAIL("Cannot create database file"));
        return "/tmp";
    }

    close(fd);
    ::remove(file_name);

    std::string path = file_name;
    path.erase(path.rfind('/'));
    return path;
}


} // @i{handler}
} // @i{model}
} // @i{rest}
} // @i{psme}
