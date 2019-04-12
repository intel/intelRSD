/*!
 * @brief Testing ManagementController configuration data.
 *
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
 * @file test_conn_data.hpp
 */

#pragma once

#include <ipmi/connection_data.hpp>

namespace testing {

class TestConnectionData : public ipmi::ConnectionData {
public:
    constexpr static const char* INTF_TYPE = "test";

    TestConnectionData() = delete;

    TestConnectionData(const std::string& _descr):
        ipmi::ConnectionData(INTF_TYPE),
        descr(_descr) { }

    virtual ~TestConnectionData() = default;


    /*!
     * Sets password to connect to management controller.
     * @param password string with password.
     */
    virtual void set_descr(const std::string& _descr) {
        descr = _descr;
    }


    /*!
     * Gets password used to connect to controller.
     * @return string with password.
     */
    virtual const std::string& get_descr() const {
        return descr;
    }

    std::string get_info() const override {
        return INTF_TYPE;
    }

private:
    std::string descr{};
};

}
