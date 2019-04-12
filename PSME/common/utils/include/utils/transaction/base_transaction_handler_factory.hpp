/*!
 * @brief Interface for the transaction handler factory.
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
 * @file base_transaction_handler_factory.hpp
 */

#pragma once

#include "base_transaction_handler.hpp"

#include <memory>

namespace utils {
namespace transaction {

/*!
 * Interface for drive handler factories. Their responsibility is to provide proper drive handlers
 * depending on the drive capabilities.
 */
class BaseTransactionHandlerFactory {
public:

    /*!
     * @brief virtual Destructor
     */
    virtual ~BaseTransactionHandlerFactory() = default;

    /*!
     * @brief Returns a rollback handler.
     * @return Rollback handler shared pointer
     */
    virtual std::shared_ptr<BaseTransactionHandler> get_handler() const = 0;
};

}
}
