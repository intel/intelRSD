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


if (NOT GLIB_FOUND)
    find_library(GLIB_LIBRARY "glib-2.0")

    find_path(GLIB_INCLUDE "glibconfig.h"
        PATHS ${CMAKE_BINARY_DIR}/include
        "/usr"
        PATH_SUFFIXES "lib64/glib-2.0/include"
                      "lib/x86_64-linux-gnu/glib-2.0/include"
    )

    if (GLIB_LIBRARY AND GLIB_INCLUDE)
        add_library("glib-2.0" IMPORTED SHARED)
        set_target_properties("glib-2.0" PROPERTIES
            IMPORTED_LOCATION ${GLIB_LIBRARY})
        get_filename_component(GLIB_LIBRARY_DIR ${GLIB_LIBRARY}
            DIRECTORY)
        set(GLIB_LIBRARIES "glib-2.0")
        set(GLIB_INCLUDE_DIRS ${GLIB_INCLUDE}
            /usr/include/glib-2.0
    )
        set(GLIB_FOUND TRUE)
    endif()
endif()
