# <license_header>
#
# Copyright (c) 2016 Intel Corporation
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
    message.cpp
    socket.cpp
    utils.cpp
    exceptions.cpp
)

add_library(netlink STATIC ${SOURCES})

install(TARGETS netlink
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

set_source_files_properties(
    message.cpp
    socket.cpp
    utils.cpp
    COMPILE_FLAGS "-Wno-missing-field-initializers -Wno-old-style-cast -Wno-cast-align -Wno-deprecated")

if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set_source_files_properties(
        message.cpp
        socket.cpp
        COMPILE_FLAGS "-Wno-old-style-cast -Wno-missing-field-initializers -Wno-sign-conversion")
endif()

