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

# Look for the header file.
find_path(LIBGCRYPT_INCLUDE_DIR
    NAMES gcrypt.h
    PATHS ${CMAKE_BINARY_DIR}/include
)
mark_as_advanced(LIBGCRYPT_INCLUDE_DIR)

# Look for the library
find_library(LIBGCRYPT_LIBRARY
    NAMES libgcrypt.so
    PATHS ${CMAKE_BINARY_DIR}/lib
)
mark_as_advanced(LIBGCRYPT_LIBRARY)

# handle the QUIETLY and REQUIRED arguments and set LIBGCRYPT_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Libgcrypt
    REQUIRED_VARS LIBGCRYPT_LIBRARY LIBGCRYPT_INCLUDE_DIR
)

if(LIBGCRYPT_FOUND)
  set(LIBGCRYPT_LIBRARIES ${LIBGCRYPT_LIBRARY})
  set(LIBGCRYPT_INCLUDE_DIRS ${LIBGCRYPT_INCLUDE_DIR})
endif()
