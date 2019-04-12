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

if (CMAKE_CXX_COMPILER_ID MATCHES GNU|Clang)
    set_source_files_properties(
        signal.cpp
        PROPERTIES
        COMPILE_FLAGS "-Wno-old-style-cast"
    )
endif ()

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
