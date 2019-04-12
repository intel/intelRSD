/*!
 * @brief Default transaction handler implementation.
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation
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
 * @file default_transaction_handler.hpp
 */

#pragma once

#include <vector>
#include "base_transaction_handler.hpp"

namespace utils {
namespace transaction {

/*!
 * @brief Default transaction handler implementation.
 */
class DefaultTransactionHandler final : public BaseTransactionHandler {
public:

    /*!
     * @brief Defaulted destructor.
     */
    DefaultTransactionHandler() = default;

    /*!
     * @brief Deleted copy constructor.
     */
    DefaultTransactionHandler(const DefaultTransactionHandler&) = delete;

    /*!
     * @brief Deleted move constructor.
     */
    DefaultTransactionHandler(DefaultTransactionHandler&&) = delete;

    /*!
     * @brief Deleted assignment operator.
     */
    DefaultTransactionHandler& operator=(const DefaultTransactionHandler&) = delete;

    /*!
     * @brief Deleted move assignment operator.
     */
    DefaultTransactionHandler& operator=(DefaultTransactionHandler&&) = delete;

    /*!
     * @brief Defaulted destructor.
     */
    ~DefaultTransactionHandler();

    /*!
     * @brief Register a pair of operations.
     *
     * @param functions a pair of operations, where functions.first is a commit and functions.second is a rollback.
     */
    void add_handler_pair(FunctionPair functions) override;

    /*!
     * @brief Perform all commit operations.
     */
    void commit() override;

    /*!
     * @brief Perform rollback operations.
     */
    void rollback() const override;

private:
    using TransactionContainer = std::vector<FunctionPair>;

    TransactionContainer m_handler{};
    TransactionContainer::size_type executed{};
};

}
}
