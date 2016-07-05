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

if (NOT MICROHTTPD_FOUND)
    find_file(microhttpd_library libmicrohttpd.so
        PATHS ${CMAKE_BINARY_DIR}/lib
    )
    find_library(microhttpd_library microhttpd)
    find_path(microhttpd_include "microhttpd.h"
        PATHS ${CMAKE_BINARY_DIR}/include
    )

    if (microhttpd_library AND microhttpd_include)
        add_library(microhttpd IMPORTED SHARED)
        set_target_properties(microhttpd PROPERTIES
            IMPORTED_LOCATION ${microhttpd_library}
        )

        get_filename_component(MICROHTTPD_LIBRARY_DIRS ${microhttpd_library}
            DIRECTORY
        )

        set(MICROHTTPD_LIBRARIES microhttpd)
        set(MICROHTTPD_INCLUDE_DIRS ${microhttpd_include})
        set(MICROHTTPD_FOUND TRUE)
    endif()
endif()
