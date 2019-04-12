/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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
 * @file extended_cpu_id.hpp
 * @brief Extended CPI ID attribute class
 * */

#pragma once


#include "agent-framework/module/utils/utils.hpp"
#include "json-wrapper/json-wrapper.hpp"



namespace agent_framework {
namespace model {
namespace attribute {

/*!
 * @brief Class representing Extended CPUID attribute
 * */
class ExtendedCpuId {
public:

    explicit ExtendedCpuId();


    virtual ~ExtendedCpuId();


    ExtendedCpuId(const ExtendedCpuId&) = default;


    ExtendedCpuId& operator=(const ExtendedCpuId&) = default;


    ExtendedCpuId(ExtendedCpuId&&) = default;


    ExtendedCpuId& operator=(ExtendedCpuId&&) = default;


    /*!
     * @brief Get CPU ID data when registers are: EAX 00h
     * @return Processor information when registers are: EAX 00h
     * */
    const OptionalField<std::string>& get_eax_00h() const {
        return m_eax_00h;
    }


    /*!
    * @brief Set CPU ID data for registers: EAX 00h
    * @param[in] eax_00h Binary value of CPU ID for register: EAX 00h
    * */
    void set_eax_00h(const OptionalField<std::string>& eax_00h) {
        m_eax_00h = eax_00h;
    }


    /*!
     * @brief Get CPU ID data when registers are: EAX 01h
     * @return Processor information when registers are: EAX 01h
     * */
    const OptionalField<std::string>& get_eax_01h() const {
        return m_eax_01h;
    }


    /*!
    * @brief Set CPU ID data for registers: EAX 01h
    * @param[in] eax_01h Binary value of CPU ID for register: EAX 01h
    * */
    void set_eax_01h(const OptionalField<std::string>& eax_01h) {
        m_eax_01h = eax_01h;
    }


    /*!
     * @brief Get CPU ID data when registers are: EAX 02h
     * @return Processor information when registers are: EAX 02h
     * */
    const OptionalField<std::string>& get_eax_02h() const {
        return m_eax_02h;
    }


    /*!
     * @brief Set CPU ID data for registers: EAX 02h
     * @param[in] eax_02h Binary value of CPU ID for register: EAX 02h
     * */
    void set_eax_02h(const OptionalField<std::string>& eax_02h) {
        m_eax_02h = eax_02h;
    }

    /*!
     * @brief Get CPU ID data when registers are: EAX 03h
     * @return Processor information when registers are: EAX 03h
     * */
    const OptionalField<std::string>& get_eax_03h() const {
        return m_eax_03h;
    }


    /*!
     * @brief Set CPU ID data for registers: EAX 03h
     * @param[in] eax_03h Binary value of CPU ID for register: EAX 03h
     * */
    void set_eax_03h(const OptionalField<std::string>& eax_03h) {
        m_eax_03h = eax_03h;
    }


    /*!
     * @brief Get CPU ID data when registers are: EAX 04h
     * @return Processor information when registers are: EAX 04h
     * */
    const OptionalField<std::string>& get_eax_04h() const {
        return m_eax_04h;
    }


    /*!
     * @brief Set CPU ID data for registers: EAX 04h
     * @param[in] eax_04h Binary value of CPU ID for register: EAX 04h
     * */
    void set_eax_04h(const OptionalField<std::string>& eax_04h) {
        m_eax_04h = eax_04h;
    }


    /*!
     * @brief Get CPU ID data when registers are: EAX 05h
     * @return Processor information when registers are: EAX 05h
     * */
    const OptionalField<std::string>& get_eax_05h() const {
        return m_eax_05h;
    }


    /*!
     * @brief Set CPU ID data for registers: EAX 05h
     * @param[in] eax_05h Binary value of CPU ID for register: EAX 05h
     * */
    void set_eax_05h(const OptionalField<std::string>& eax_05h) {
        m_eax_05h = eax_05h;
    }

    /*!
     * @brief Get CPU ID data when registers are: EAX 06h
     * @return Processor information when registers are: EAX 06h
     * */
    const OptionalField<std::string>& get_eax_06h() const {
        return m_eax_06h;
    }


    /*!
     * @brief Set CPU ID data for registers: EAX 06h
     * @param[in] eax_06h Binary value of CPU ID for register: EAX 06h
     * */
    void set_eax_06h(const OptionalField<std::string>& eax_06h) {
        m_eax_06h = eax_06h;
    }

    /*!
     * @brief Get CPU ID data when registers are: EAX 07h
     * @return Processor information when registers are: EAX 07h
     * */
    const OptionalField<std::string>& get_eax_07h() const {
        return m_eax_07h;
    }


    /*!
     * @brief Set CPU ID data for registers: EAX 07h
     * @param[in] eax_07h Binary value of CPU ID for register: EAX 07h
     * */
    void set_eax_07h(const OptionalField<std::string>& eax_07h) {
        m_eax_07h = eax_07h;
    }

    /*!
     * @brief Get CPU ID data when registers are: EAX 09h
     * @return Processor information when registers are: EAX 09h
     * */
    const OptionalField<std::string>& get_eax_09h() const {
        return m_eax_09h;
    }


    /*!
     * @brief Set CPU ID data for registers: EAX 09h
     * @param[in] eax_09h Binary value of CPU ID for register: EAX 09h
     * */
    void set_eax_09h(const OptionalField<std::string>& eax_09h) {
        m_eax_09h = eax_09h;
    }


    /*!
     * @brief Get CPU ID data when registers are: EAX 0Ah
     * @return Processor information when registers are: EAX 0Ah
     * */
    const OptionalField<std::string>& get_eax_0ah() const {
        return m_eax_0ah;
    }


    /*!
     * @brief Set CPU ID data for registers: EAX 0Ah
     * @param[in] eax_0ah Binary value of CPU ID for register: EAX 0Ah
     * */
    void set_eax_0ah(const OptionalField<std::string>& eax_0ah) {
        m_eax_0ah = eax_0ah;
    }


    /*!
     * @brief Get CPU ID data when registers are: EAX 0Bh
     * @return Processor information when registers are: EAX 0Bh
     * */
    const OptionalField<std::string>& get_eax_0bh() const {
        return m_eax_0bh;
    }


    /*!
     * @brief Set CPU ID data for registers: EAX 0Bh
     * @param[in] eax_0bh Binary value of CPU ID for register: EAX 0Bh
     * */
    void set_eax_0bh(const OptionalField<std::string>& eax_0bh) {
        m_eax_0bh = eax_0bh;
    }


    /*!
     * @brief Get CPU ID data when registers are: EAX 0Dh
     * @return Processor information when registers are: EAX 0Dh
     * */
    const OptionalField<std::string>& get_eax_0dh() const {
        return m_eax_0dh;
    }


    /*!
     * @brief Set CPU ID data for registers: EAX 0Dh
     * @param[in] eax_0dh Binary value of CPU ID for register: EAX 0Dh
     * */
    void set_eax_0dh(const OptionalField<std::string>& eax_0dh) {
        m_eax_0dh = eax_0dh;
    }


    /*!
     * @brief Get CPU ID data when registers are: EAX 0Fh
     * @return Processor information when registers are: EAX 0Fh
     * */
    const OptionalField<std::string>& get_eax_0fh() const {
        return m_eax_0fh;
    }


    /*!
     * @brief Set CPU ID data for registers: EAX 0Fh
     * @param[in] eax_0fh Binary value of CPU ID for register: EAX 0Fh
     * */
    void set_eax_0fh(const OptionalField<std::string>& eax_0fh) {
        m_eax_0fh = eax_0fh;
    }


    /*!
     * @brief Get CPU ID data when registers are: EAX 10h
     * @return Processor information when registers are: EAX 10h
     * */
    const OptionalField<std::string>& get_eax_10h() const {
        return m_eax_10h;
    }


    /*!
     * @brief Set CPU ID data for registers: EAX 10h
     * @param[in] eax_10h Binary value of CPU ID for register: EAX 10h
     * */
    void set_eax_10h(const OptionalField<std::string>& eax_10h) {
        m_eax_10h = eax_10h;
    }


    /*!
     * @brief Get CPU ID data when registers are: EAX 14h
     * @return Processor information when registers are: EAX 14h
     * */
    const OptionalField<std::string>& get_eax_14h() const {
        return m_eax_14h;
    }


    /*!
     * @brief Set CPU ID data for registers: EAX 14h
     * @param[in] eax_14h Binary value of CPU ID for register: EAX 14h
     * */
    void set_eax_14h(const OptionalField<std::string>& eax_14h) {
        m_eax_14h = eax_14h;
    }


    /*!
     * @brief Get CPU ID data when registers are: EAX 15h
     * @return Processor information when registers are: EAX 15h
     * */
    const OptionalField<std::string>& get_eax_15h() const {
        return m_eax_15h;
    }


    /*!
     * @brief Set CPU ID data for registers: EAX 15h
     * @param[in] eax_15h Binary value of CPU ID for register: EAX 15h
     * */
    void set_eax_15h(const OptionalField<std::string>& eax_15h) {
        m_eax_15h = eax_15h;
    }


    /*!
     * @brief Get CPU ID data when registers are: EAX 16h
     * @return Processor information when registers are: EAX 16h
     * */
    const OptionalField<std::string>& get_eax_16h() const {
        return m_eax_16h;
    }


    /*!
     * @brief Set CPU ID data for registers: EAX 16h
     * @param[in] eax_16h Binary value of CPU ID for register: EAX 16h
     * */
    void set_eax_16h(const OptionalField<std::string>& eax_16h) {
        m_eax_16h = eax_16h;
    }


    /*!
     * @brief Get CPU ID data when registers are: EAX 17h ECX 00h
     * @return Processor information when registers are: EAX 17h ECX 00h
     * */
    const OptionalField<std::string>& get_eax_17h_ecx_00h() const {
        return m_eax_17h_ecx_00h;
    }


    /*!
     * @brief Set CPU ID data for registers: EAX 17h ECX 00h
     * @param[in] eax_17h_ecx_00h Binary value of CPU ID for register: EAX 17h ECX 00h
     * */
    void set_eax_17h_ecx_00h(const OptionalField<std::string>& eax_17h_ecx_00h) {
        m_eax_17h_ecx_00h = eax_17h_ecx_00h;
    }


    /*!
     * @brief Get CPU ID data when registers are: EAX 17h ECX 01h
     * @return Processor information when registers are: EAX 17h ECX 01h
     * */
    const OptionalField<std::string>& get_eax_17h_ecx_01h() const {
        return m_eax_17h_ecx_01h;
    }


    /*!
     * @brief Set CPU ID data for registers: EAX 17h ECX 01h
     * @param[in] eax_17h_ecx_01h Binary value of CPU ID for register: EAX 17h ECX 01h
     * */
    void set_eax_17h_ecx_01h(const OptionalField<std::string>& eax_17h_ecx_01h) {
        m_eax_17h_ecx_01h = eax_17h_ecx_01h;
    }


    /*!
     * @brief Get CPU ID data when registers are: EAX 17h ECX 02h
     * @return Processor information when registers are: EAX 17h ECX 02h
     * */
    const OptionalField<std::string>& get_eax_17h_ecx_02h() const {
        return m_eax_17h_ecx_02h;
    }


    /*!
     * @brief Set CPU ID data for registers: EAX 17h ECX 02h
     * @param[in] eax_17h_ecx_02h Binary value of CPU ID for register: EAX 17h ECX 02h
     * */
    void set_eax_17h_ecx_02h(const OptionalField<std::string>& eax_17h_ecx_02h) {
        m_eax_17h_ecx_02h = eax_17h_ecx_02h;
    }


    /*!
     * @brief Get CPU ID data when registers are: EAX 17h ECX 03h
     * @return Processor information when registers are: EAX 17h ECX 03h
     * */
    const OptionalField<std::string>& get_eax_17h_ecx_03h() const {
        return m_eax_17h_ecx_03h;
    }


    /*!
     * @brief Set CPU ID data for registers: EAX 17h ECX 03h
     * @param[in] eax_17h_ecx_03h Binary value of CPU ID for register: EAX 17h ECX 03h
     * */
    void set_eax_17h_ecx_03h(const OptionalField<std::string>& eax_17h_ecx_03h) {
        m_eax_17h_ecx_03h = eax_17h_ecx_03h;
    }


    /*!
     * @brief Get CPU ID data when registers are: EAX 80000000h
     * @return Processor information when registers are: EAX 80000000h
     * */
    const OptionalField<std::string>& get_eax_80000000h() const {
        return m_eax_80000000h;
    }


    /*!
     * @brief Set CPU ID data for registers: EAX 80000000h
     * @param[in] eax_80000000h Binary value of CPU ID for register: EAX 80000000h
     * */
    void set_eax_80000000h(const OptionalField<std::string>& eax_80000000h) {
        m_eax_80000000h = eax_80000000h;
    }


    /*!
     * @brief Get CPU ID data when registers are: EAX 80000001h
     * @return Processor information when registers are: EAX 80000001h
     * */
    const OptionalField<std::string>& get_eax_80000001h() const {
        return m_eax_80000001h;
    }


    /*!
     * @brief Set CPU ID data for registers: EAX 80000001h
     * @param[in] eax_80000001h Binary value of CPU ID for register: EAX 80000001h
     * */
    void set_eax_80000001h(const OptionalField<std::string>& eax_80000001h) {
        m_eax_80000001h = eax_80000001h;
    }


    /*!
     * @brief Get CPU ID data when registers are: EAX 80000002h
     * @return Processor information when registers are: EAX 80000002h
     * */
    const OptionalField<std::string>& get_eax_80000002h() const {
        return m_eax_80000002h;
    }


    /*!
     * @brief Set CPU ID data for registers: EAX 80000002h
     * @param[in] eax_80000002h Binary value of CPU ID for register: EAX 80000002h
     * */
    void set_eax_80000002h(const OptionalField<std::string>& eax_80000002h) {
        m_eax_80000002h = eax_80000002h;
    }


    /*!
     * @brief Get CPU ID data when registers are: EAX 80000003h
     * @return Processor information when registers are: EAX 80000003h
     * */
    const OptionalField<std::string>& get_eax_80000003h() const {
        return m_eax_80000003h;
    }


    /*!
     * @brief Set CPU ID data for registers: EAX 80000003h
     * @param[in] eax_80000003h Binary value of CPU ID for register: EAX 80000003h
     * */
    void set_eax_80000003h(const OptionalField<std::string>& eax_80000003h) {
        m_eax_80000003h = eax_80000003h;
    }


    /*!
     * @brief Get CPU ID data when registers are: EAX 80000004h
     * @return Processor information when registers are: EAX 80000004h
     * */
    const OptionalField<std::string>& get_eax_80000004h() const {
        return m_eax_80000004h;
    }


    /*!
     * @brief Set CPU ID data for registers: EAX 80000004h
     * @param[in] eax_80000004h Binary value of CPU ID for register: EAX 80000004h
     * */
    void set_eax_80000004h(const OptionalField<std::string>& eax_80000004h) {
        m_eax_80000004h = eax_80000004h;
    }


    /*!
     * @brief Get CPU ID data when registers are: EAX 80000006h
     * @return Processor information when registers are: EAX 80000006h
     * */
    const OptionalField<std::string>& get_eax_80000006h() const {
        return m_eax_80000006h;
    }


    /*!
     * @brief Set CPU ID data for registers: EAX 80000006h
     * @param[in] eax_80000006h Binary value of CPU ID for register: EAX 80000006h
     * */
    void set_eax_80000006h(const OptionalField<std::string>& eax_80000006h) {
        m_eax_80000006h = eax_80000006h;
    }


    /*!
     * @brief Get CPU ID data when registers are: EAX 80000007h
     * @return Processor information when registers are: EAX 80000007h
     * */
    const OptionalField<std::string>& get_eax_80000007h() const {
        return m_eax_80000007h;
    }


    /*!
     * @brief Set CPU ID data for registers: EAX 80000007h
     * @param[in] eax_80000007h Binary value of CPU ID for register: EAX 80000007h
     * */
    void set_eax_80000007h(const OptionalField<std::string>& eax_80000007h) {
        m_eax_80000007h = eax_80000007h;
    }


    /*!
     * @brief Get CPU ID data when registers are: EAX 80000008h
     * @return Processor information when registers are: EAX 80000008h
     * */
    const OptionalField<std::string>& get_eax_80000008h() const {
        return m_eax_80000008h;
    }


    /*!
     * @brief Set CPU ID data for registers: EAX 80000008h
     * @param[in] eax_80000008h Binary value of CPU ID for register: EAX 80000008h
     * */
    void set_eax_80000008h(const OptionalField<std::string>& eax_80000008h) {
        m_eax_80000008h = eax_80000008h;
    }


    /*!
     * @brief construct an object of class CpuId from JSON
     *
     * @param json the json::Json deserialized to object
     *
     * @return the newly constructed CpuId object
     */
    static ExtendedCpuId from_json(const json::Json& json);


    /*!
     * @brief transform the object to JSon
     *
     * @return the object serialized to json::Json
     */
    json::Json to_json() const;


private:
    OptionalField<std::string> m_eax_00h{};
    OptionalField<std::string> m_eax_01h{};
    OptionalField<std::string> m_eax_02h{};
    OptionalField<std::string> m_eax_03h{};
    OptionalField<std::string> m_eax_04h{};
    OptionalField<std::string> m_eax_05h{};
    OptionalField<std::string> m_eax_06h{};
    OptionalField<std::string> m_eax_07h{};
    OptionalField<std::string> m_eax_09h{};
    OptionalField<std::string> m_eax_0ah{};
    OptionalField<std::string> m_eax_0bh{};
    OptionalField<std::string> m_eax_0dh{};
    OptionalField<std::string> m_eax_0fh{};
    OptionalField<std::string> m_eax_10h{};
    OptionalField<std::string> m_eax_14h{};
    OptionalField<std::string> m_eax_15h{};
    OptionalField<std::string> m_eax_16h{};
    OptionalField<std::string> m_eax_17h_ecx_00h{};
    OptionalField<std::string> m_eax_17h_ecx_01h{};
    OptionalField<std::string> m_eax_17h_ecx_02h{};
    OptionalField<std::string> m_eax_17h_ecx_03h{};
    OptionalField<std::string> m_eax_80000000h{};
    OptionalField<std::string> m_eax_80000001h{};
    OptionalField<std::string> m_eax_80000002h{};
    OptionalField<std::string> m_eax_80000003h{};
    OptionalField<std::string> m_eax_80000004h{};
    OptionalField<std::string> m_eax_80000006h{};
    OptionalField<std::string> m_eax_80000007h{};
    OptionalField<std::string> m_eax_80000008h{};

};

}
}
}

