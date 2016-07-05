# <license_header>
#
# Copyright (c) 2015-2016 Intel Corporation
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
    # Network Controller  implementation.
    network_controller.cpp
)

if (NL3_FOUND)
    set(FM10000_SOURCES ${SOURCES}
        # Fm10k Controller implementation.
        fm10000/network_controller.cpp
        fm10000/network_controller_manager.cpp
    )
    add_subdirectory(fm10000)
    add_library(hw-fm10000 OBJECT ${FM10000_SOURCES})
endif()
