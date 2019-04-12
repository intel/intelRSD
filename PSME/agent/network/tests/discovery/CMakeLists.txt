# <license_header>
#
# Copyright (c) 2015-2019 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# </license_header>

if (NOT GTEST_FOUND)
    return()
endif()

if (BUILD_EOS_SDK)
    add_gtest(discovery_test psme-network
        mock_utils.cpp
        mock_switch_info.cpp
        discovery_manager_test.cpp
        test_runner.cpp
    )

    generate_config_file(discovery_manager_test.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/configuration_full.hpp
        "NETWORK_FULL_CONFIGURATION"
        "Declaration of full configuration"
    )

    target_link_libraries(${test_target}
        eos
        network-utils
        ${UUID_LIBRARIES}
        ${LOGGER_LIBRARIES}
        configuration
        ${SAFESTRING_LIBRARIES}
    )
endif()
