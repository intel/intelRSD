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

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/examples)

macro(add_logger_example target exten)
    add_executable(logger_example_${target} logger_example_${target}.${exten})
    target_link_libraries(
        logger_example_${target}
        logger
        pthread
        ${SAFESTRING_LIBRARIES}
        )
endmacro()

add_logger_example("stdout" "c")
add_logger_example("buffer" "c")
add_logger_example("streams" "c")
add_logger_example("cpp" "cpp")
add_logger_example("udp_receiving" "c")

include_directories(SYSTEM
    ${SAFESTRING_INCLUDE_DIRS}
)

if (CMAKE_C_COMPILER_ID MATCHES GNU|Clang)
    set_source_files_properties(
        logger_example_stdout.c
        logger_example_buffer.c
        logger_example_streams.c
        logger_example_udp_receiving.c
        PROPERTIES
        COMPILE_FLAGS "-std=gnu11 -Wno-disabled-macro-expansion"
    )
endif()
