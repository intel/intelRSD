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

if (CMAKE_CROSSCOMPILING)
    message(WARNING "Not supported for crosscompaling.")
    return()
endif()

if (NOT LIBXMLPP_FOUND)

    find_library(LIBXMLPP_LIBRARY "xml++-2.6")

    find_path(LIBXMLPP_INCLUDE
        FILES "libxml++config.h"
        PATHS ${CMAKE_BINARY_DIR}/include "/usr"
        PATH_SUFFIXES "lib64/libxml++-2.6/include"
                      "lib/libxml++-2.6/include"
                      "lib/x86_64-linux-gnu/libxml++-2.6/include"
    )

    if (LIBXMLPP_LIBRARY AND LIBXMLPP_INCLUDE)
        add_library("libxml++-2.6" IMPORTED SHARED)
        set_target_properties("libxml++-2.6" PROPERTIES
            IMPORTED_LOCATION ${LIBXMLPP_LIBRARY}
        )

        get_filename_component(LIBXMLPP_LIBRARY_DIRS ${LIBXMLPP_LIBRARY}
            DIRECTORY
        )

        set(LIBXMLPP_LIBRARIES "xml++-2.6")
        set(LIBXMLPP_INCLUDE_DIRS ${LIBXMLPP_INCLUDE}
            /usr/include/libxml++-2.6
        )
        set(LIBXMLPP_FOUND TRUE)
    endif()
endif()

