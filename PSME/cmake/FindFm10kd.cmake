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

if (NOT FM10KD_FOUND)
    find_path(fm10kd_include acl_nl_cmds.h)

    if (fm10kd_include)
        set(FM10KD_INCLUDE_DIRS ${fm10kd_include})
        set(FM10KD_FOUND TRUE)
    endif()
endif()
