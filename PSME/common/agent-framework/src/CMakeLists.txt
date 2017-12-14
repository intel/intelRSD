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

set(SOURCES
    version.cpp
    logger_ext.cpp
    logger_loader.cpp
    signal.cpp
    service_uuid.cpp
)

add_library(agent-framework STATIC ${SOURCES}
    $<TARGET_OBJECTS:discovery>
    $<TARGET_OBJECTS:module>
    $<TARGET_OBJECTS:state_machine>
    $<TARGET_OBJECTS:command>
    $<TARGET_OBJECTS:eventing>
    $<TARGET_OBJECTS:threading>
    $<TARGET_OBJECTS:registration>
    $<TARGET_OBJECTS:agent-action>
    $<TARGET_OBJECTS:exceptions>
    $<TARGET_OBJECTS:validators>
    $<TARGET_OBJECTS:tree-stability>
)

target_link_libraries(agent-framework
    json-cxx
    json-rpc
    json
    md5
)

install(TARGETS
    agent-framework
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

if (CMAKE_CXX_COMPILER_ID MATCHES GNU|Clang)
    set_source_files_properties(
        signal.cpp
        PROPERTIES
        COMPILE_FLAGS "-Wno-old-style-cast"
    )
endif()

add_subdirectory(action)
add_subdirectory(exceptions)
add_subdirectory(threading)
add_subdirectory(command)
add_subdirectory(module)
add_subdirectory(state_machine)
add_subdirectory(eventing)
add_subdirectory(registration)
add_subdirectory(discovery)
add_subdirectory(validators)
add_subdirectory(tree_stability)
