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
 * @file sysfs/abstract_sysfs_interface.hpp
 */

#pragma once

#include "sysfs/path.hpp"

#include <vector>

namespace sysfs {

/*! Represents a directory in the sysfs */
struct SysfsDir final {
    /*! Path to the directory */
    Path path{};
    /*! Name of the directory */
    std::string name{};
    /*! List of all sub directories */
    std::vector<Path> dirs{};
    /*! List of all links in the directory */
    std::vector<Path> links{};
    /*! List of all files/attributes in the directory */
    std::vector<Path> files{};
};

/*! Represents a sym link in the sysfs */
struct SysfsLink final {
    /*! Path to the link */
    Path path{};
    /*! Name of the link */
    std::string name{};
    /*! Value of the link (linked path) */
    Path value{};
};

/*! Represents a file/attribute in the sysfs */
struct SysfsFile final {
    /*! Path to the file/attribute */
    Path path{};
    /*! Name of the file/attribute */
    std::string name{};
    /*! Value of the attribute (stored in the file) */
    std::string value{};
};

/*! Represents a fileystem description mount */
struct SysfsMount final {
    /*! Mounted filesystem name */
    std::string name{};
    /*! Mounted fileystem type */
    std::string type{};
    /*! Filesystem path */
    Path path{};
};

/*! Abstraction for the sysfs access interface */
class AbstractSysfsInterface {
public:

    AbstractSysfsInterface();
    virtual ~AbstractSysfsInterface();

    AbstractSysfsInterface(const AbstractSysfsInterface&) = default;
    AbstractSysfsInterface& operator=(const AbstractSysfsInterface&) = default;

    AbstractSysfsInterface(AbstractSysfsInterface&&) = default;
    AbstractSysfsInterface& operator=(AbstractSysfsInterface&&) = default;

    /*!
     * @brief Gets the directory data
     * @param path Path to the directory
     * @return Struct representing the directory
     */
    virtual SysfsDir get_dir(const Path& path) const = 0;

    /*!
     * @brief Adds a directory
     * @param path Path to the place where directory is to be created
     * @param name Name of the new directory
     */
    virtual void add_dir(const Path& path, const std::string& name) const = 0;

    /*!
     * @brief Removes a directory
     * @param path Path to the directory to be removed
     */
    virtual void del_dir(const Path& path) const = 0;

    /*!
     * @brief Gets the symlink
     * @param path Path to the symlink
     * @return Struct representing the symlink
     */
    virtual SysfsLink get_link(const Path& path) const = 0;

    /*!
     * @brief Adds a link
     * @param path Path to the place where link is to be created
     * @param name Name of the new link
     * @param value Path pointed to by the link
     */
    virtual void add_link(const Path& path, const std::string& name, const Path& value) const = 0;

    /*!
     * @brief Removes a link
     * @param path Path to the link to be removed
     */
    virtual void del_link(const Path& path) const = 0;

    /*!
     * @brief Gets the file data
     * @param path Path to the file
     * @return Struct representing the file
     */
    virtual SysfsFile get_file(const Path& path) const = 0;

    /*!
     * @brief Adds a file/attribute
     * @param path Path to the place where file is to be created
     * @param name Name of the new file
     * @param value Value to be stored in the file
     */
    virtual void add_file(const Path& path, const std::string& name, const std::string& value) const = 0;

    /*!
     * @brief Sets a new value in an existing file
     * @param path Path to the file to be modified
     * @param value New value to be stored by the file/attribute
     */
    virtual void set_file(const Path& path, const std::string& value) const = 0;

    /*!
     * @brief Removes a file
     * @param path Path to the file to be removed
     */
    virtual void del_file(const Path& path) const = 0;

    /*!
     * @brief Checks if directory exists
     * @param path Path to be checked
     * @return True if directory exists
     */
    virtual bool dir_exists(const Path& path) const = 0;

    /*!
     * @brief Checks if link exists
     * @param path Path to be checked
     * @return True if directory exists
     */
    virtual bool link_exists(const Path& path) const = 0;

    /*!
     * @brief Checks if file exists
     * @param path Path to be checked
     * @return True if file exists
     */
    virtual bool file_exists(const Path& path) const = 0;

    /*!
     * @brief Reads filesystem mounts from the file
     * @param path Path to the filesystem descriptor file to be read
     * @return List of all mounts found in the system
     */
    virtual std::vector<SysfsMount> get_mounts(const Path& path) const = 0;

    /*!
     * @brief Converts a provided path to its canonized form
     * @param input Input path (may be a symlink)
     * @return Canonized input path
     */
    virtual Path get_absolute_path(const Path& input) const = 0;

};

}
