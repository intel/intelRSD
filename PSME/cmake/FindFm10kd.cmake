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


find_file(FM10K_ACL_LIBRARY acl_nl_cmds.h
        PATHS "/usr"
    )

if (FM10K_ACL_LIBRARY)
    set(FM10K_ACL_FOUND TRUE)
else()
    set(FM10K_ACL_FOUND FALSE)
    message(WARNING "PSME Network will not be compiled, a header from fm10kd-devel package is missing.")
endif()

