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
 * @file nvmf_subsystem.hpp
 */

#pragma once



#include <string>
#include "listen_address.hpp"
#include "host.hpp"
#include "namespace.hpp"
#include "json-wrapper/json-wrapper.hpp"



namespace spdk {
namespace model {

class NvmfSubsystem {
public:

    static constexpr const char NQN[] = "nqn";
    static constexpr const char SUBTYPE[] = "subtype";
    static constexpr const char LISTEN_ADDRESSES[] = "listen_addresses";
    static constexpr const char LISTEN_ADDRESS[] = "listen_address";
    static constexpr const char HOST[] = "host";
    static constexpr const char HOSTS[] = "hosts";
    static constexpr const char ALLOW_ANY_HOST[] = "allow_any_host";
    static constexpr const char SERIAL_NUMBER[] = "serial_number";
    static constexpr const char NAMESPACE[] = "namespace";
    static constexpr const char NAMESPACES[] = "namespaces";
    static constexpr const char MAX_NAMESPACES[] = "max_namespaces";
    static constexpr const char DISCOVERY[] = "Discovery";


    /*!
    * @brief Gets Subsystem NQN
    * @return Subsystem NQN
    */
    const std::string& get_nqn() const {
        return m_nqn;
    }


    /*!
     * @brief Sets Subsystem NQN
     * @param nqn Subsystem NQN
     */
    void set_nqn(const std::string& nqn) {
        m_nqn = nqn;
    }


    /*!
    * @brief Gets NVMe over Fabrics subtype
    * @return subtype
    */
    const std::string& get_subtype() const {
        return m_subtype;
    }


    /*!
    * @brief Sets NVMe over Fabrics subtype
    * @param subtype NVMe over Fabrics subtype
    */
    void set_subtype(const std::string& subtype) {
        m_subtype = subtype;
    }


    /*!
     * @brief Gets vector of listen_addresses
     * @return vector of listen_addresses
     */
    const std::vector<spdk::model::ListenAddress>& get_listen_addresses() const {
        return m_listen_addresses;
    }


    /*!
     * @brief Sets vector of listen_addresses
     * @param listen_addresses vector of listen_addresses
     */
    void set_listen_addresses(const std::vector<spdk::model::ListenAddress>& listen_addresses) {
        m_listen_addresses = listen_addresses;
    }


    /*!
     * @brief Gets vector of strings containing allowed host NQNs
     * @return vector of strings containing allowed host NQNs
     */
    const std::vector<spdk::model::Host>& get_hosts() const {
        return m_hosts;
    }


    /*!
     * @brief Sets vector of strings containing allowed host NQNs
     * @param hosts vector of strings containing allowed host NQNs
     */
    void set_hosts(const std::vector<spdk::model::Host>& hosts) {
        m_hosts = hosts;
    }


    /*!
     * @brief Gets information if any host is allowed
     * @return true if any host is allowed, false otherwise
     */
    bool is_allow_any_host() const {
        return m_allow_any_host;
    }


    /*!
     * @brief Sets information if any host is allowed
     * @rparam allow_any_host true if any host is allowed, false otherwise
     */
    void set_allow_any_host(bool allow_any_host) {
        m_allow_any_host = allow_any_host;
    }


    /*!
     * @brief Gets serial number of virtual controller
     * @return serial number of virtual controller
     */
    const  JsonOptional<std::string>& get_serial_number() const {
        return m_serial_number;
    }


    /*!
     * @brief Sets serial number of virtual controller
     * @param serial_number serial number of virtual controller
     */
    void set_serial_number(const  JsonOptional<std::string>& serial_number) {
        m_serial_number = serial_number;
    }


    /*!
     * @brief Lists current info about Nvmf Subsystems
     * @return info about Nvmf Subsystems as a string
     */
    std::string to_string() const;


    /*!
     * @brief Gets vector of Namespaces
     * @return vector of namespaces
     */
    const std::vector<spdk::model::Namespace>& get_namespaces() const {
        return m_namespaces;
    }


    /*!
     * @brief Sets vector of namespaces con
     * @param Vector of namespaces
     */
    void set_namespaces(const std::vector<spdk::model::Namespace>& namespaces) {
        m_namespaces = namespaces;
    }


private:
    std::string m_nqn{};
    std::string m_subtype{};
    std::vector<spdk::model::ListenAddress> m_listen_addresses{};
    std::vector<spdk::model::Host> m_hosts{};
    bool m_allow_any_host{};
    JsonOptional<std::string> m_serial_number{};
    std::vector<spdk::model::Namespace> m_namespaces{};

};


/*!
 * @brief converts from JSON to NvmfSubsystem
 * @param json json object
 * @param nvmf_subsystem NvmfSubsystem object
 */
void from_json(const json::Json& json, NvmfSubsystem& nvmf_subsystem);


/*!
 * @brief converts from NvmfSubsystem to Json
 * @param json Json object
 * @param nvmf_subsystem NvmfSubsystem object
 */
void to_json(json::Json& json, const NvmfSubsystem& nvmf_subsystem);

}
}


