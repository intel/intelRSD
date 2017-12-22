# <license_header>
#
# Copyright (c) 2015-2017 Intel Corporation
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


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set (SOURCES
    version.cpp
    app.cpp
    main.cpp
    )

include_directories(
    ${PSME_APPLICATION_DIR}/include
    ${DATABASE_INCLUDE_DIRS}
)

add_subdirectory(rest)
add_subdirectory(ssdp)
add_subdirectory(registration)
add_subdirectory(core)
add_subdirectory(eventing)

if (ENABLE_HTTPS)
    set(EXTRA_LIBS ${EXTRA_LIBS} ${GNUTLS_LIBRARIES})
endif()

add_executable(psme-rest-server ${SOURCES})

target_link_libraries(psme-rest-server
    ${LOGGER_LIBRARIES}
    application-rest
    application-core
    -Wl,--whole-archive application-eventing -Wl,--no-whole-archive
    -Wl,--whole-archive application-registration -Wl,--no-whole-archive
    ssdp-config-loader
    metadata
    ${UUID_LIBRARIES}
    ${MICROHTTPD_LIBRARIES}
    ${JSON_LIBRARIES}
    ${JSON_RPC_LIBRARIES}
    ${CONFIGURATION_LIBRARIES}
    ${JSONCXX_LIBRARIES}
    ${JSONPTR_LIBRARIES}
    ${BASE64_LIBRARIES}
    ${CURL_LIBRARIES}
    ${AGENT_FRAMEWORK_LIBRARIES}
    ${MD5_LIBRARIES}
    ${SSDP_LIBRARIES}
    ${NET_LIBRARIES}
    ${DATABASE_LIBRARIES}
    ${EXTRA_LIBS}
    ${SAFESTRING_LIBRARIES}
)

if (CMAKE_CXX_COMPILER_ID MATCHES GNU|Clang)
    set_source_files_properties(
        signal.cpp
        PROPERTIES
        COMPILE_FLAGS "-Wno-old-style-cast"
    )
endif()
