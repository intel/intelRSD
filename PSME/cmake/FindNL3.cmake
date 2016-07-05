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

if (NOT NL3_FOUND)
    find_file(nl3_library libnl-3.so
        PATHS ${CMAKE_BINARY_DIR}/lib
    )

    find_file(nl3_route
        NAMES libnl-route-3.so libnl-route-3.so.200.so
        PATHS ${CMAKE_BINARY_DIR}/lib
    )

    find_library(nl3_library nl-3)

    find_library(nl3_route nl-route-3 libnl-route-3.so.200
        PATHS "/usr/lib/x86_64-linux-gnu/"
    )

    find_library(nl3_genl nl-genl-3 libnl-genl-3.so.200
        PATHS "/lib64"
    )

    find_path(nl3_include "netlink/netlink.h"
        PATHS ${CMAKE_BINARY_DIR}/include
        PATH_SUFFIXES "libnl3"
    )

    if (nl3_library AND nl3_route AND nl3_include)
        add_library(nl-3 IMPORTED SHARED)
        set_target_properties(nl-3 PROPERTIES
            IMPORTED_LOCATION ${nl3_library}
        )
        get_filename_component(NL3_LIBRARY_DIRS ${nl3_library}
            DIRECTORY
        )

        add_library(nl-route-3 IMPORTED SHARED)
        set_target_properties(nl-route-3 PROPERTIES
            IMPORTED_LOCATION ${nl3_route}
        )
        get_filename_component(NL3_ROUTE_LIBRARY_DIRS ${nl3_route}
            DIRECTORY
        )

        set(NL3_LIBRARIES nl-3 nl-route-3)

        if(nl3_genl)
            set(NL3_GENL_LIBRARIES nl-genl-3)
        else()
            message(WARNING "nl-genl-3 library was not found!")
        endif()

        set(NL3_INCLUDE_DIRS ${nl3_include})
        set(NL3_FOUND TRUE)
    else()
        message(WARNING "nl3 library or its headers were not found!")
    endif()
endif()
