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

if (NOT SYSFS_FOUND)
    find_file(sysfs_library libsysfs.so
        PATHS ${CMAKE_BINARY_DIR}/lib
    )
    find_library(sysfs_library sysfs)
    find_path(sysfs_include libsysfs.h
        PATHS ${CMAKE_BINARY_DIR}/include
    )

    if (sysfs_library AND sysfs_include)
        add_library(sysfs IMPORTED SHARED)
        set_target_properties(sysfs PROPERTIES
            IMPORTED_LOCATION ${sysfs_library}
        )

    get_filename_component(SYSFS_LIBRARY_DIRS ${sysfs_library}
            DIRECTORY
        )

    set(SYSFS_LIBRARIES sysfs)
    set(SYSFS_INCLUDE_DIRS ${sysfs_include})
    set(SYSFS_FOUND TRUE)
    endif()
endif()
