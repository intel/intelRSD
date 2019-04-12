/*!
 * @brief ISCSI MDR entry point interface.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file iscsi_mdr_entry_point.hpp
 */

#pragma once
#include "mdr/generic_entry_point.hpp"
#include <stdexcept>
#include <memory>
#include <cstdint>

namespace iscsi {

namespace parser {

/*!
 * @brief iSCSI entry point interface.
 */
class IscsiMdrEntryPoint : public mdr::GenericEntryPoint {
public:
    using Ptr = std::unique_ptr<IscsiMdrEntryPoint>;

    /*!
     * @brief Exception type that will be thrown when error condition is
     * discovered while parsing iSCSI MDR blob.
     */
    class Exception: public std::runtime_error {
    public:
        /*!
         * @brief constructs exception object
         * @param what_arg Describes what happened
         */
        explicit Exception(const std::string &what_arg) :
            std::runtime_error("iSCSI MDR entry point - " + what_arg) {
        }

        Exception(const Exception &) = default;
        Exception& operator=(const Exception&) = default;
        Exception(Exception&&) = default;
        Exception& operator=(Exception&&) = default;

        virtual ~Exception();
    };

    /*!
     * @brief Create Entry point structure pointer
     * @param buf pointer to the bytes to be parsed
     * @param buf_size number of bytes to be parsed
     * @return SmbiosEntryPoint pointer
     */
    static IscsiMdrEntryPoint::Ptr create(const std::uint8_t* buf, const size_t buf_size);

    /*!
     * @brief Not implemented, this method always throws.
     * @return None.
     */
    [[ noreturn ]]
    std::uint64_t get_table_data_offset(const std::string&) const override;

    /*!
     * @brief Not implemented, this method always throws.
     * @return None.
     */
    [[ noreturn ]]
    uint64_t get_table_data_end_offset(const std::string&) const override;

    /*!
     * @brief Not implemented, this method always throws.
     * @return None.
     */
    [[ noreturn ]]
    void set_struct_table_address(std::uint64_t) override;

    /*!
     * @brief Not implemented, this method always throws.
     * @return None.
     */
    [[ noreturn ]]
    void set_struct_table_end_address(std::uint64_t) override;

    /*! Destructor */
    virtual ~IscsiMdrEntryPoint();

};

}  // namespace parser

}  // namespace iscsi
