/*!
 * @brief Base class for generic transaction handlers.
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
 * @file base_transaction_handler.hpp
 */

#pragma once

#include <functional>
#include <utility>
#include <memory>

namespace utils {
namespace transaction {

/*!
 * @brief A base class for transactional operations' handlers.
 */
class BaseTransactionHandler {
public:

    using Ptr = std::shared_ptr<BaseTransactionHandler>;
    using FunctionPair = std::pair<std::function<void()>, std::function<void()>>;

    /*!
     * @brief Register a pair of operations.
     *
     * @param functions a pair of operations, where functions.first is a commit and functions.second is a rollback.
     */
    virtual void add_handler_pair(FunctionPair functions) = 0;

    /*!
     * @brief Perform all commit operations.
     */
    virtual void commit() = 0;

    /*!
     * @brief Perform rollback operations.
     */
    virtual void rollback() const = 0;

    /*!
     * @brief Defaulted destructor.
     */
    virtual ~BaseTransactionHandler() = default;
};

}
}
