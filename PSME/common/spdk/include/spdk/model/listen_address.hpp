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
 * @file listen_address.hpp
 */

#pragma once



#include <string>
#include "json-wrapper/json-wrapper.hpp"
#include "spdk/json_optional.hpp"



namespace spdk {
namespace model {

class AddressFamily {
public:
    AddressFamily() = delete;


    static constexpr const char IPv4[] = "IPv4";
    static constexpr const char IPv6[] = "IPv6";
    static constexpr const char IB[] = "IB";
    static constexpr const char FC[] = "FC";
};

class TransportType {
public:
    TransportType() = delete;


    static constexpr const char RDMA[] = "RDMA";
};

class ListenAddress {

public:

    static constexpr const char TRTYPE[] = "trtype";
    static constexpr const char ADRFAM[] = "adrfam";
    static constexpr const char TRADDR[] = "traddr";
    static constexpr const char TRSVCID[] = "trsvcid";


    /*!
     * @brief Gets transport type ("RDMA")
     * @return transport type
     */
    const std::string& get_trtype() const {
        return m_trtype;
    }


    /*!
     * @brief Sets transport type ("RDMA")
     * @param trtype transport type
     */
    void set_trtype(const std::string& trtype) {
        m_trtype = trtype;
    }


    /*!
     * @brief Gets address family ("IPv4", "IPv6", "IB", or "FC")
     * @return address family
     */
    const JsonOptional<std::string>& get_adrfam() const {
        return m_adrfam;
    }


    /*!
     * @brief Sets address family ("IPv4", "IPv6", "IB", or "FC")
     * @param adrfam address family
     */
    void set_adrfam(const JsonOptional<std::string>& adrfam) {
        m_adrfam = adrfam;
    }


    /*!
     * @brief Gets transport address
     * @return transport address
     */
    const std::string& get_traddr() const {
        return m_traddr;
    }


    /*!
     * @brief Sets transport address
     * @param traddr transport address
     */
    void set_traddr(const std::string& traddr) {
        m_traddr = traddr;
    }


    /*!
     * @brief Gets Transport service ID
     * @return Transport service ID
     */
    const JsonOptional<std::string>& get_trsvcid() const {
        return m_trsvcid;
    }


    /*!
     * @brief Sets Transport service ID
     * @param trsvcid Transport service ID
     */
    void set_trsvcid(const JsonOptional<std::string>& trsvcid) {
        m_trsvcid = trsvcid;
    }


    /*!
     * @brief Lists current info about ListenAddress
     * @return info about ListenAddress as a string
     */
    std::string to_string() const;


private:

    std::string m_trtype{};
    JsonOptional<std::string> m_adrfam{};
    std::string m_traddr{};
    JsonOptional<std::string> m_trsvcid{};

};


/*!
 * @brief converts from JSON to ListenAddress
 * @param json json object
 * @param listen_address ListenAddress object
 */
void from_json(const json::Json& json, ListenAddress& listen_address);


/*!
 * @brief converts from ListenAddress to Json
 * @param json Json object
 * @param listen_address ListenAddress object
 */
void to_json(json::Json& json, const ListenAddress& listen_address);

}
}
