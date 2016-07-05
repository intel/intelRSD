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
 * @file agent-framework/module/chassis.hpp
 * @brief Chassis
 * */

#ifndef AGENT_FRAMEWORK_MODULE_CHASSIS_HPP
#define AGENT_FRAMEWORK_MODULE_CHASSIS_HPP

#include "agent-framework/module/resources.hpp"
#include "agent-framework/module/compute_zone.hpp"

#include "agent-framework/logger_ext.hpp"

#include <vector>
#include <string>

using std::string;

/*! Psme namespace */
namespace agent_framework {
/*! Generic namespace */
namespace generic {

/*! Chassis class */
class Chassis : public Resources {
    const std::string CHASSIS_COMPONENT_TYPE = "Chassis";

    std::string m_type{};
    std::uint32_t m_size{};
    std::uint32_t m_location_offset{};

    std::vector<ComputeZone::ComputeZoneSharedPtr> m_compute_zones{};

public:
    /*! Chassis unique pointer */
    using ChassisSharedPtr = std::shared_ptr<Chassis>;

    /*! Copy constructor */
    Chassis(const Chassis&) = default;

    /*! Assignment operator */
    Chassis& operator=(const Chassis&) = default;

    /*!
     * @brief Default constructor
     * */
    Chassis(const std::string& type = "", std::uint32_t size = 0,
            std::uint32_t location_offset = 0)
        : m_type(type), m_size(size), m_location_offset(location_offset) {}

    virtual ~Chassis();

    /*!
     * @brief Gets type of chassis.
     * @return Chassis type.
     * */
    const std::string& get_type() const {
        return m_type;
    }

    /*!
     * @brief Gets size
     * @return Size
     * */
    std::uint32_t get_size() const {
        return m_size;
    }

    /*!
     * @brief Gets location offset
     * @return Location offset
     * */
    std::uint32_t get_location_offset() const {
        return m_location_offset;
    }

    /*!
     * @brief Gets compute zones
     * @return Vector of compute zones
     * */
    const std::vector<ComputeZone::ComputeZoneSharedPtr>& get_compute_zones() const {
        return m_compute_zones;
    }

    /*!
     * @brief Load settings from configuration
     *
     * @param[in] chassis_configuration Chassis configuration section
     * */
    void read_configuration(const json::Value& chassis_configuration);

private:
    void m_read_compute_zones(const json::Value& chassis_configuration);
};

}
}
#endif /* AGENT_FRAMEWORK_MODULE_CHASSIS_HPP */
