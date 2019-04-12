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
endif ()


add_gtest(test ssdp
    test_runner.cpp
    packet_test.cpp
    packet_factory_test.cpp
    packet_parser_test.cpp
    packet_validator_test.cpp
    uri_test.cpp
    message_queue_test.cpp
    ssdp_service_impl_test.cpp
    multicast_socket_mock.cpp
    message_test.cpp
    parser_common.cpp
    notification_test.cpp
    )

target_link_libraries(${test_target}
    ssdp
    )

add_custom_target(unittest_ssdp
    make
    )

add_custom_target(unittest_ssdp_run
    ctest --output-on-failure
    )
