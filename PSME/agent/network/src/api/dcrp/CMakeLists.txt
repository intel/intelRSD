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


set(DCRP_SOURCES
    remote_switch_info.cpp
   )

if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
    set_source_files_properties(
                                remote_switch_info.cpp
                                PROPERTIES
                                COMPILE_FLAGS "-Wno-old-style-cast -Wno-missing-field-initializers -Wno-sign-conversion"
                               )
endif()
add_library(api-fm10000-dcrp OBJECT ${DCRP_SOURCES})

