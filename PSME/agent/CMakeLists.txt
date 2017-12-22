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

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/compute)
    add_subdirectory(compute)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/storage)
    add_subdirectory(storage)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/network)
    add_subdirectory(network)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/chassis)
    add_subdirectory(chassis)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/pnc)
    add_subdirectory(pnc)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/rmm)
    add_subdirectory(rmm)
endif()
