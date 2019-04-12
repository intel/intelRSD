/*!
 * @brief Mock for the NVME transaction handler factory.
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
 * @file mock_transaction_handler_factory.hpp
 */

#pragma once



#include "gmock/gmock.h"
#include "utils/transaction/base_transaction_handler_factory.hpp"



class MockTransactionHandlerFactory : public ::utils::transaction::BaseTransactionHandlerFactory {
public:

    MOCK_CONST_METHOD0(get_handler, std::shared_ptr<::utils::transaction::BaseTransactionHandler>());
};
