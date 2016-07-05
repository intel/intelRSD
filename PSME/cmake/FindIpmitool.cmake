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

if (NOT IPMITOOL_FOUND)

    find_file(ipmitool_library libipmicmd.a
        PATHS ${CMAKE_BINARY_DIR}/lib
    )

    find_library(ipmitool_library ipmicmd)

    find_path(ipmitool_include "ipmitool/ipmi_intf.h" "ipmitool/ipmi_sol.h"
        PATHS ${CMAKE_BINARY_DIR}/include
    )

    if (ipmitool_library AND ipmitool_include)
        add_library(ipmicmd IMPORTED STATIC)

        set_target_properties(ipmicmd PROPERTIES
            IMPORTED_LOCATION ${ipmitool_library}
        )

        get_filename_component(IPMITOOL_LIBRARY_DIRS ${ipmitool_library}
            DIRECTORY
        )

        set(IPMITOOL_LIBRARIES ipmicmd)
        set(IPMITOOL_INCLUDE_DIRS ${ipmitool_include})
        set(IPMITOOL_FOUND TRUE)
    endif()
endif()

