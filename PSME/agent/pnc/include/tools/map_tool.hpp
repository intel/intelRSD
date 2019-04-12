/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 * @file map_tool.hpp
 *
 * @brief MapTool used for convertions between gas/i2c fields and their model
 * representations.
 * */

#pragma once

#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/pnc_components.hpp"
#include "gas/mrpc/link_status_retrieve.hpp"
#include "nvme/smart.hpp"

namespace agent {
namespace pnc {
namespace tools {

/*! MapTool class */
class MapTool {
public:

    /*! Default constructor */
    MapTool() {}

    /*! Enable copy */
    MapTool(const MapTool&){}
    MapTool& operator=(const MapTool&) {return *this;}

    /*! Default destructor */
    virtual ~MapTool();

    /*!
     * @brief Extracts model status from the LinkStatusRetrieve command output
     * @param[in] lsr LinkStatusRetrieve command result
     * @return Status of the port
     * */
    virtual agent_framework::model::attribute::Status get_port_status_from_link_status(
        const gas::mrpc::LinkStatusRetrieve& lsr) const;

    /*!
     * @brief Extracts port width and speed from the LinkStatusRetrieve command output
     * @param[in] lsr LinkStatusRetrieve command result
     * @return Tuple consisting of four fields: link width, max link width, link speed
     * */
    virtual std::tuple<uint32_t, uint32_t, double> get_port_width_and_speed_from_link_status(
        const gas::mrpc::LinkStatusRetrieve lsr) const;

    /*!
     * @brief Extracts status from read smart data
     * @param[in] smart Read smart data
     * @return Drive Status
     * */
    virtual agent_framework::model::attribute::Status get_status_from_smart(const nvme::Smart& smart) const;

    /*!
     * @brief Converts GT/s to gbps
     * @param[in] speed Speed in GT/s
     * @param[in] width Current width of the line
     * @return Speed converted to gpbs
     * */
    virtual double gtps_to_gbps(double speed, uint32_t width) const;

};

using MapToolPtr = std::shared_ptr<MapTool>;

}
}
}
