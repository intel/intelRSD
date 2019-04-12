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
 * @file controller_data.hpp
 */

#pragma once



#include <string>
#include "json-wrapper/json-wrapper.hpp"
#include "spdk/model/optional_admin_command_support.hpp"



namespace spdk {
namespace model {

class ControllerData {
public:

    static constexpr const char FIRMWARE_REVISION[] = "firmware_revision";
    static constexpr const char SERIAL_NUMBER[] = "serial_number";
    static constexpr const char OACS[] = "oacs";
    static constexpr const char VENDOR_ID[] = "vendor_id";
    static constexpr const char MODEL_NUMBER[] = "model_number";


    /*!
     * @brief Gets the firmware revision
     * @return firmware revision
     */
    const std::string& get_firmware_revision() const {
        return m_firmware_revision;
    }


    /*!
     * @brief Sets the firmware revision
     * @param firmware_revision new firmware revision
     */
    void set_firmware_revision(const std::string& firmware_revision) {
        m_firmware_revision = firmware_revision;
    }


    /*!
     * @brief Gets the serial number
     * @return serial number
     */
    const std::string& get_serial_number() const {
        return m_serial_number;
    }


    /*!
     * @brief Sets the serial number
     * @param serial_number new serial number
     */
    void set_serial_number(const std::string& serial_number) {
        m_serial_number = serial_number;
    }


    /*!
     * @brief Gets the Optional Admin Command Support
     * @return OACS
     */
    const OptionalAdminCommandSupport& get_oacs() const {
        return m_oacs;
    }


    /*!
     * @brief Sets the Optional Admin Command Support
     * @param oacs new Optional Admin Command Support
     */
    void set_oacs(const OptionalAdminCommandSupport& oacs) {
        m_oacs = oacs;
    }


    /*!
     * @brief Gets the vendor id
     * @return vendor id
     */
    const std::string& get_vendor_id() const {
        return m_vendor_id;
    }


    /*!
     * @brief Sets the vendor id
     * @param vendor_id new vendor id
     */
    void set_vendor_id(const std::string& vendor_id) {
        m_vendor_id = vendor_id;
    }


    /*!
     * @brief Gets the model number
     * @return model number
     */
    const std::string& get_model_number() const {
        return m_model_number;
    }


    /*!
     * @brief Sets the model number
     * @param model_number new model number
     */
    void set_model_number(const std::string& model_number) {
        m_model_number = model_number;
    }


    /*!
     * @brief Lists current info about Ctrlr Data
     * @return info about Ctrlr Data as a string
     */
    std::string to_string() const;


private:
    std::string m_firmware_revision{};
    std::string m_serial_number{};
    OptionalAdminCommandSupport m_oacs{};
    std::string m_vendor_id{};
    std::string m_model_number{};

};


/*!
 * @brief converts from JSON to ControllerData
 * @param json json object
 * @param ctrlr ControllerData object
 */
void from_json(const json::Json& json, ControllerData& ctrlr);


/*!
 * @brief converts from ControllerData to Json
 * @param json Json object
 * @param ctrlr ControllerData object
 */
void to_json(json::Json& json, const ControllerData& ctrlr);

}
}



