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
 * @file namespaces.hpp
 */

#pragma once



#include <string>
#include "spdk/json_optional.hpp"



namespace spdk {
namespace model {

class Namespace {
public:

    static constexpr const char NSID[] = "nsid";
    static constexpr const char BDEV_NAME[] = "bdev_name";
    static constexpr const char NGUID[] = "nguid";
    static constexpr const char EUI64[] = "eui64";
    static constexpr const char UUID[] = "uuid";


    /*!
     * @brief Gets namespace ID between 1 and 4294967294
     * @return namespace ID
     */
    const JsonOptional<std::uint32_t>& get_namespace_id() const {
        return m_nsid;
    }


    /*!
     * @brief Sets namespace ID between 1 and 4294967294
     * @param nsid namespace ID
     */
    void set_namespace_id(const JsonOptional<std::uint32_t>& nsid) {
        m_nsid = nsid;
    }


    /*!
     * @brief Gets name of bdev
     * @return Name of bdev
     */
    const std::string& get_bdev_name() const {
        return m_bdev_name;
    }


    /*!
     * @brief Sets name of bdev to expose as a namespace.
     * @param bdev_name dev Name of bdev
     */
    void set_bdev_name(const std::string& bdev_name) {
        m_bdev_name = bdev_name;
    }


    /*!
     * @brief Gets 16-byte namespace globally unique identifier in hexadecimal
     * @return nguid
     */
    const JsonOptional<std::string>& get_nguid() const {
        return m_nguid;
    }


    /*!
     * @brief Sets 16-byte namespace globally unique identifier in hexadecimal
     * @param nguid namespace globally unique id
     */
    void set_nguid(const JsonOptional<std::string>& nguid) {
        m_nguid = nguid;
    }


    /*!
     * @brief Gets 8-byte namespace EUI-64 in hexadecimal
     * @param 8-byte namespace EUI-64 in hexadecimal as string
     */
    const JsonOptional<std::string>& get_eui_64() const {
        return m_eui64;
    }


    /*!
     * @brief Sets 8-byte namespace EUI-64 in hexadecimal
     * @param eui64 8-byte namespace EUI-64 in hexadecimal as string
     */
    void set_eui_64(const JsonOptional<std::string>& eui64) {
        m_eui64 = eui64;
    }


    /*!
     * @brief Gets RFC 4122 UUID
     * @return RFC 4122 UUID
     */
    const JsonOptional<std::string>& get_uuid() const {
        return m_uuid;
    }


    /*!
     * @brief Sets RFC 4122 UUID
     * @param uuid  RFC 4122 UUID
     */
    void set_uuid(const JsonOptional<std::string>& uuid) {
        m_uuid = uuid;
    }


    /*!
    * @brief Lists current info about Namespace
    * @return info about Namespace as a string
    */
    std::string to_string() const;


private:

    std::string m_bdev_name{};
    JsonOptional<std::uint32_t> m_nsid{};
    JsonOptional<std::string> m_nguid{};
    JsonOptional<std::string> m_eui64{};
    JsonOptional<std::string> m_uuid{};

};


/*!
 * @brief converts from JSON to Namespace
 * @param json json object
 * @param namespace Namespace object
 */
void from_json(const json::Json& json, Namespace& nvmf_namespace);


/*!
 * @brief converts from Namespace to Json
 * @param json Json object
 * @param nvmf_namespace Namespace object
 */
void to_json(json::Json& json, const Namespace& nvmf_namespace);

}
}



