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
 * @file default_transaction_handler.cpp
 */

#include "utils/transaction/default_transaction_handler.hpp"
#include "logger/logger_factory.hpp"

#include <algorithm>

namespace utils {
namespace transaction {

DefaultTransactionHandler::~DefaultTransactionHandler() {
}

void DefaultTransactionHandler::add_handler_pair(BaseTransactionHandler::FunctionPair functions) {
    m_handler.emplace_back(std::move(functions));
}

void DefaultTransactionHandler::commit() {
    executed = 0;
    std::for_each(m_handler.begin(), m_handler.end(), [this](const TransactionContainer::value_type& pair) {
        ++executed;
        pair.first();
    });
}

void DefaultTransactionHandler::rollback() const {
    std::for_each(m_handler.rbegin() + (m_handler.size() - executed), m_handler.rend(),
        [](const TransactionContainer::value_type& pair) {
            try {
                pair.second();
            }
            catch (const std::exception& e) {
                // there is nothing to be done if a rollback operation fails
                log_warning("transaction", "Failed during rollback: " << e.what());
            }
        });
}

}
}
