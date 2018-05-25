/*!
 * @brief Mock for the NVME transaction handler.
 *
 * @copyright Copyright (c) 2017-2018 Intel Corporation
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
 * @header{Files}
 * @file mock_transaction_handler.hpp
 */

#pragma once

#include "gmock/gmock.h"
#include "tools/base_transaction_handler.hpp"

class MockTransactionHandler : public agent::nvme::tools::BaseTransactionHandler {
public:

    MOCK_METHOD1(add_handler_pair, void(agent::nvme::tools::BaseTransactionHandler::func_pair));
    MOCK_METHOD0(commit, void());
    MOCK_CONST_METHOD0(rollback, void());
};
