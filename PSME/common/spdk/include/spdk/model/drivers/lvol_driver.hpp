/*!
 * @copyright Copyright (c) 2018-2019 Intel Corporation
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
 * @file lvol_driver.hpp
 */

#pragma once



#include "spdk/json_optional.hpp"



namespace spdk {
namespace model {
namespace drivers {
/*!
 * @brief Implementation of Lvol Driver for Bdev model
 */
class LvolDriver {
public:

    /*!
     * @brief Name for LVOL Driver
     */
    static const char DRIVER_NAME[];


    /*!
     * @brief Lists current info about Lvol Driver
     * @return info about Lvol Driver as a string
     */
    std::string to_string() const;


    /*!
     * @brief Get base Bdev name
     * @return base Bdev name
     */
    const std::string& get_base_bdev() const {
        return m_base_bdev;
    }


    /*!
     * @brief Sets base Bdev name
     * @param base_bdev new base_bdev name
     */
    void set_base_bdev(const std::string& base_bdev) {
        m_base_bdev = base_bdev;
    }


    /*!
     * @brief Check whether the driver is clone
     * @return true if driver is clone
     */
    bool is_clone() const {
        return m_clone;
    }


    /*!
     * @brief Sets driver clone
     * @param clone true if driver is clone
     */
    void set_clone(bool clone) {
        m_clone = clone;
    }


    /*!
     * @brief Check whether the driver is snapshot
     * @return true if driver is snapshot
     */
    bool is_snapshot() const {
        return m_snapshot;
    }


    /*!
     * @brief Sets driver snapshot
     * @param snapshot true if driver is snapshot
     */
    void set_snapshot(bool snapshot) {
        m_snapshot = snapshot;
    }


    /*!
     * @brief Get Lvol Store UUID
     * @return Lvol Store UUID
     */
    const std::string& get_lvol_store_uuid() const {
        return m_lvol_store_uuid;
    }


    /*!
     * @brief Sets Lvol Store UUID
     * @param lvol_store_uuid new base_bdev name
     */
    void set_lvol_store_uuid(const std::string& lvol_store_uuid) {
        m_lvol_store_uuid = lvol_store_uuid;
    }


    /*!
     * @brief Sets driver thin provision
     * @param thin_provision true if driver is thin_provision
     */
    void set_thin_provision(bool thin_provision) {
        m_thin_provision = thin_provision;
    }


    /*!
     * @brief Check whether the driver is thin provision
     * @return true if driver is thin provision
     */
    bool is_thin_provision() const {
        return m_thin_provision;
    }

    /*!
     * @brief Get optional array of BDEVs names which are clones of this volume.
     * @return Optional vector of string.
     */
    const JsonOptional<std::vector<std::string>>& get_clones() const {
        return m_clones;
    }

    /*!
     * @brief Set BDEVs names which are clones of this volume.
     * @param clones Vector of string.
     */
    void set_clones(const JsonOptional<std::vector<std::string>>& clones) {
        m_clones = clones;
    }

    /*!
     * @brief Get optional base snapshot name.
     * @return Optional string.
     */
    const JsonOptional<std::string>& get_base_snapshot() const {
        return m_base_snapshot;
    }

    /*!
     * @brief Set base snapshot name.
     * @param base_snapshot Snapshot name.
     */
    void set_base_snapshot(const JsonOptional<std::string>& base_snapshot) {
        m_base_snapshot = base_snapshot;
    }

private:
    std::string m_base_bdev{};
    bool m_clone{};
    bool m_snapshot{};
    std::string m_lvol_store_uuid{};
    bool m_thin_provision{};
    JsonOptional<std::string> m_base_snapshot{};
    JsonOptional<std::vector<std::string>> m_clones{};

};


/*!
 * @brief converts from Json to LvolDriver
 * @param json Json object
 * @param lvol_drive LvolDriver object
 */
void from_json(const json::Json& json, LvolDriver& lvol_driver);


/*!
 * @brief converts from LvolDriver to Json
 * @param json Json object
 * @param lvol_drive LvolDriver object
 */
void to_json(json::Json& json, const LvolDriver& lvol_driver);

}
}
}

