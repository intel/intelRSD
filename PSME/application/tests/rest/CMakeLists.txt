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

add_gtest(rest psme-rest-server
    test_runner.cpp

    endpoints/utils.cpp
    endpoints/query_entries_test.cpp
    endpoints/id_parsing_test.cpp
    endpoints/utils_path_builder_test.cpp
    # Because of linking problems, GenericHandlerTest is run by FabricHandlersTest
    #model/handler/generic_handler_test.cpp
    model/handler/fabric_handlers_test.cpp
    model/find_test.cpp
    server/mux/split_path_test.cpp
    server/multiplexer_test.cpp
    ssdp/ssdp_config_loader_test.cpp
    utils/health_rollup_test.cpp
    error/error_factory_test.cpp
    validator/json_validator_test.cpp
)

target_link_libraries(${test_target}
    ssdp-config-loader
    application-rest
    application-core
    agent-framework
    ${CURL_LIBRARIES}
    ${MICROHTTPD_LIBRARIES}
    ssdp
    ${LOGGER_LIBRARIES}
    ${SAFESTRING_LIBRARIES}
    ${UUID_LIBRARIES}
)

set_source_files_properties(
    model/handler/generic_handler_test.cpp
    PROPERTIES COMPILE_FLAGS "-Wno-exit-time-destructors -Wno-global-constructors"
)
