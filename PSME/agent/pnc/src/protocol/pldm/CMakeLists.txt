# <license_header>
#
# Copyright (c) 2019 Intel Corporation
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
    command/get_pldm_version.cpp
    command/get_pdr_repository_info.cpp
    command/get_fru_record_set_pdr.cpp
    command/get_pdr.cpp
    command/get_pldm_commands.cpp
    command/get_terminus_uid.cpp
    command/pldm_command.cpp
    command/get_terminus_locator_pdr.cpp

    utils/pldm_utils.cpp

    pldm_tool.cpp
    pldm_error.cpp)


add_library(pnc-pldm STATIC ${SOURCES})

target_link_libraries(pnc-pldm PRIVATE
    utils)