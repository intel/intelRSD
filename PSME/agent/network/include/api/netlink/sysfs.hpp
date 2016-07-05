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
 *
 *
 * @file sysfs.hpp
 *
 * @brief Get switch port attribute via sysfs.
 * */

#ifndef AGENT_NETWORK_NETLINK_SYSFS_HPP
#define AGENT_NETWORK_NETLINK_SYSFS_HPP

#include <memory>
#include <map>
#include <vector>
#include <libudev.h>

namespace agent {
namespace network {
namespace api {
namespace netlink {

/*! SysFs class to get switch port attributes */
class SysFs {
public:

    /*!
     * @brief Default constructor.
     */
    SysFs() = default;

    /*!
     * @brief Copy constructor.
     * @param[in] orig object to be copied.
     */
    SysFs(const SysFs& orig) = delete;

    /*!
     * @brief Copy operator.
     * @param[in] s object to be copied
     */
    SysFs& operator=(const SysFs& s) = delete;

    /*!
     * @brief Default constructor.
     * @param[in] name interface name.
     */
    SysFs(const std::string& name);

    /*!
     * @brief Default destructor.
     */
    ~SysFs();

    /*!
     * @brief Get port count.
     * @return Port count.
     */
    uint32_t get_port_count() const;

    /*!
     * @brief Get max frame size.
     * @return Max frame size.
     */
    uint32_t get_max_frame_size() const;

    /*!
     * @brief Get autoneg.
     * @return Autoneg.
     */
    uint32_t get_autoneg() const;

    /*!
     * @brief Get port list.
     * @return Port list.
     */
    const std::vector<std::string> get_port_list() const;

    /*!
     * @brief Get team list.
     * @return Port list.
     */
    const std::vector<std::string> get_team_list() const;

private:

    /*!
     * @enum SwPortAttribute
     * @brief Switch port attributes
     *
     * @var SwPortAttribute SysFs::SW_PORT_ATTR_MAX_FRAME_SIZE
     * Max frame size switch port attribute.
     *
     * @var SwPortAttribute SysFs::SW_PORT_ATTR_AUTONEG
     * Autoneg switch port attribute.
     *
     * */
    enum class SwPortAttribute {
        SW_PORT_ATTR_MAX_FRAME_SIZE,
        SW_PORT_ATTR_AUTONEG
    };

    /*!
     * @brief Get switch port attribute.
     * @param[in] attr Attribute to get.
     * @return Attribute value.
     */
    std::string get_switch_port_attribute(SwPortAttribute attr) const;

    /*!
     * @brief Get switch port attribute.
     * @param[in] attr Attribute to get.
     * @return Integer attribute value.
     */
    int get_switch_port_attribute_int(SwPortAttribute attr) const;

    /*!
     * @brief Get port list.
     * @param[in] driver Driver name.
     * @return driver entry list.
     */
    const std::vector<std::string> get_driver_entries(const std::string& driver) const;

    /* Udev deleter */
    struct UdevDeleter {
        void operator()(struct udev* ctx) const {
            udev_unref(ctx);
        }
    };

    /* Udev device deleter */
    struct UdevDeviceDeleter {
        void operator()(struct udev_device* dev) const {
            udev_device_unref(dev);
        }
    };

    /* Udev enumerate deleter */
    struct UdevEnumDeleter {
        void operator()(struct udev_enumerate* udev_enum) const {
            udev_enumerate_unref(udev_enum);
        }
    };
    using Udev = std::unique_ptr<struct udev, UdevDeleter>;
    using UdevDevice = std::unique_ptr<struct udev_device, UdevDeviceDeleter>;
    using UdevEnum = std::unique_ptr<struct udev_enumerate, UdevEnumDeleter>;

    std::string ifname{};
    std::map<SwPortAttribute, const char*> attrs{};

    static const constexpr char* SUBSYSTEM = "net";
    static const constexpr char* DRIVER = "fm10ks";
    static const constexpr char* TEAM = "team";
    static const constexpr char* CPU_PORT = "sw0p0";
};

}
}
}
}

#endif /* AGENT_NETWORK_NETLINK_SYSFS_HPP */
