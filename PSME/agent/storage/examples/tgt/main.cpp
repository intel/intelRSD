/*!
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2015-2017 Intel Corporation
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
 * @section DESCRIPTION
 *
 * @file main.cpp
 *
 * @brief Example of ISCSI lib use.
 * */
#include "iscsi/manager.hpp"
#include "iscsi/response.hpp"
#include "iscsi/target_parser.hpp"
#include <iostream>

using namespace agent::storage::iscsi::tgt;

int main(int argc, char* argv[]) {
    (void) argc;
    (void) argv;

    Manager manager;
    auto response = manager.show_targets();

    if (!response.is_valid()) {
        std::cout << "Invalid show target response" << std::endl;
        return -1;
    }

    auto& extra_data = response.get_extra_data();
    std::string iscsi_text(extra_data.cbegin(), extra_data.cend());
    TargetParser parser{};
    const auto targets = parser.parse(iscsi_text);

    for (const auto& target : targets) {
        std::cout << "Target: " << target->get_target_id() << " " <<
            target->get_target_iqn() << std::endl;
        std::cout << "Initiator: " << target->get_target_initiator() << std::endl;
        for (const auto& lun : target->get_luns()) {
            std::cout << "Lun: " << lun->get_lun() << " " <<
                                    lun->get_device_path() << std::endl;
        }
    }

    return 0;
}
