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

if (NOT NCURSES_FOUND)
    find_file(ncurses_library libncurses.so
        PATHS ${CMAKE_BINARY_DIR}/lib
    )
    find_library(ncurses_library ncurses)
    find_path(ncurses_include "ncurses.h"
        PATH_SUFFIXES ncurses
        PATHS ${CMAKE_BINARY_DIR}/include
    )

    if (ncurses_library AND ncurses_include)
        add_library(ncurses IMPORTED STATIC)
        set_target_properties(ncurses PROPERTIES
            IMPORTED_LOCATION ${ncurses_library}
        )

        get_filename_component(NCURSES_LIBRARY_DIRS ${ncurses_library}
            DIRECTORY
        )

        set(NCURSES_LIBRARIES ncurses)
        set(NCURSES_INCLUDE_DIRS ${ncurses_include})
        set(NCURSES_FOUND TRUE)
    endif()
endif()
