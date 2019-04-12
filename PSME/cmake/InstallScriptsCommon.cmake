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

# Creates a list called ARGS and addds standard + compiler options to it
macro(init_cmake_args ARGS_LIST)
    if ("${ARGS_LIST}" STREQUAL "")
        message(FATAL_ERROR "init_cmake_args requires precisely one argument for CMake arguments list variable! argv1 is ${ARGS_LIST}")
    endif("${ARGS_LIST}" STREQUAL "")

    set(${ARGS_LIST})

    list(APPEND ${ARGS_LIST} -DCMAKE_PREFIX_PATH:PATH=${CMAKE_BINARY_DIR})
    list(APPEND ${ARGS_LIST} -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR})

    if (CMAKE_CROSSCOMPILING)
        if (CMAKE_TOOLCHAIN_FILE AND EXISTS ${CMAKE_TOOLCHAIN_FILE})
            list(APPEND ${ARGS_LIST} -DCMAKE_TOOLCHAIN_FILE:FILEPATH=${CMAKE_TOOLCHAIN_FILE})
        endif()

        list(APPEND ${ARGS_LIST} -DCMAKE_FIND_ROOT_PATH:PATH=${CMAKE_BINARY_DIR})
    endif()

    if (CMAKE_BUILD_TYPE)
        list(APPEND ${ARGS_LIST} -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE})
    endif()

    list(APPEND ${ARGS_LIST} -DCMAKE_C_FLAGS_DEBUG:PATH=${CMAKE_C_FLAGS_DEBUG})
    list(APPEND ${ARGS_LIST} -DCMAKE_C_FLAGS_RELEASE:PATH=${CMAKE_C_FLAGS_RELEASE})
    list(APPEND ${ARGS_LIST} -DCMAKE_C_FLAGS_COVERAGE:PATH=${CMAKE_C_FLAGS_COVERAGE})

    list(APPEND ${ARGS_LIST} -DCMAKE_CXX_FLAGS_DEBUG:PATH=${CMAKE_CXX_FLAGS_DEBUG})
    list(APPEND ${ARGS_LIST} -DCMAKE_CXX_FLAGS_RELEASE:PATH=${CMAKE_CXX_FLAGS_RELEASE})
    list(APPEND ${ARGS_LIST} -DCMAKE_CXX_FLAGS_COVERAGE:PATH=${CMAKE_CXX_FLAGS_COVERAGE})

    list(APPEND ${ARGS_LIST} -DCMAKE_EXE_LINKER_FLAGS_DEBUG:PATH=${CMAKE_EXE_LINKER_FLAGS_DEBUG})
    list(APPEND ${ARGS_LIST} -DCMAKE_EXE_LINKER_FLAGS_RELEASE:PATH=${CMAKE_EXE_LINKER_FLAGS_RELEASE})
    list(APPEND ${ARGS_LIST} -DCMAKE_EXE_LINKER_FLAGS_COVERAGE:PATH=${CMAKE_EXE_LINKER_FLAGS_COVERAGE})

    list(APPEND ${ARGS_LIST} --no-warn-unused-cli)
endmacro(init_cmake_args ARGS_LIST)



function(check_executable exec)
    # which is not "very portable" method of checking if exec is runnable.
    execute_process(
        COMMAND which ${exec}
        OUTPUT_VARIABLE out
        RESULT_VARIABLE ret
    )
    if (NOT ${ret} EQUAL 0)
        message(FATAL_ERROR "Cannot execute ${exec}. Is appropriate package installed?")
    endif()
endfunction()



# TODO add md5 for downloaded files..
macro(assure_package pkg ver link)
    set(patch_dir "${CMAKE_CURRENT_LIST_DIR}/../tools/patches")

    # get target source directory
    set(source_name "${pkg}-${ver}")
    if (EXISTS ${patch_dir}/${source_name}.patch)
        set(patch ${patch_dir}/${source_name}.patch)
    else()
        file(GLOB patches
            ${patch_dir}/${source_name}*.patch
        )
        list(LENGTH patches num)
        if (num EQUAL 1)
            list(GET patches 0 patch)
            string(REPLACE "${patch_dir}/" "" source_name ${patch})
            string(REPLACE ".patch" "" source_name ${source_name})
        else()
            set(patch)
        endif()
    endif()
    set(source_dir ${CMAKE_BINARY_DIR}/${source_name})


    # get extension, from the link
    set(extension "")
    foreach(ext "zip" "tar\\..*" "tar" "hpp")
        if ("${link}" MATCHES ".*\\.${ext}$")
            string(REGEX MATCH "${ext}$$" extension ${link})
            break()
        endif()
    endforeach()

    if (NOT ${PKG}_VERSION)
        # download file to third_party directory
        set(downloaded_file ${CMAKE_CURRENT_LIST_DIR}/../third_party/${pkg}-${ver}.${extension})
        if (DEFINED DOWNLOAD_THIRDPARTY)
            if (NOT DOWNLOAD_THIRDPARTY AND NOT EXISTS ${downloaded_file})
                message(FATAL_ERROR "${pkg}-${ver}.${extension} has not been downloaded")
            endif()
        endif()
        if (DOWNLOAD_THIRDPARTY OR NOT EXISTS ${downloaded_file})
            if (DOWNLOAD_${link} MATCHES "^https://.*$")
                if (NOT DEFINED ENV{HTTPS_PROXY})
                    message(STATUS "No HTTPS_PROXY environment variable defined")
                endif()
            elseif (${link} MATCHES "^http://.*$")
                if (NOT DEFINED ENV{HTTP_PROXY})
                    message(STATUS "No HTTP_PROXY environment variable defined")
                endif()
            elseif (${link} MATCHES "^ftp://.*$")
                if (NOT DEFINED ENV{FTP_PROXY})
                    message(STATUS "No FTP_PROXY environment variable defined")
                endif()
            endif()
            message(STATUS "Downloading ${link}")
            file(DOWNLOAD
                ${link}
                ${downloaded_file}
                SHOW_PROGRESS
                STATUS status
            )

            set(MD5 ${ARGN})
            if (MD5)
                check_executable("md5sum")
                execute_process(
                    COMMAND md5sum ${downloaded_file}
                    OUTPUT_VARIABLE result
                )
                string(REGEX MATCH "[0-9a-z]*" COMPUTED "${result}")
                if (NOT ${COMPUTED} STREQUAL ${MD5})
                    message(FATAL_ERROR "Incorrect md5 of ${pkg}-${ver}.${extension}, calculated ${COMPUTED}, expected ${MD5}")
                endif()
            endif()

            list(GET status 0 result)
            if (NOT ${result} EQUAL 0)
                if (EXISTS ${downloaded_file})
                    file(REMOVE ${downloaded_file})
                endif()
                list(GET status 1 message)
                message(FATAL_ERROR "Cannot download ${pkg}-${ver}, error ${result} (${message})")
            endif()
        endif()

        file(REMOVE  ${source_dir})
        file(MAKE_DIRECTORY ${source_dir})
        if ("${extension}" STREQUAL "hpp")
            string(REGEX MATCH "[^/]*$" file_name "${link}")
            configure_file(
                ${CMAKE_CURRENT_LIST_DIR}/../third_party/${pkg}-${ver}.${extension} ${source_dir}/${file_name}
                COPYONLY
            )
        else()
            # extract files from archive and "reduce" to appropriate level
            if ("${extension}" STREQUAL "zip")
                check_executable("unzip")
                execute_process(
                    COMMAND unzip ${downloaded_file}
                    WORKING_DIRECTORY ${source_dir}
                    ERROR_VARIABLE errors
                    RESULT_VARIABLE result
                )
            else()
                check_executable("tar")
                execute_process(
                    COMMAND ${CMAKE_COMMAND} -E tar xvf ${downloaded_file}
                    WORKING_DIRECTORY ${source_dir}
                    ERROR_VARIABLE errors
                    RESULT_VARIABLE result
                )
            endif()
            if (NOT ${result} EQUAL 0)
                message(FATAL_ERROR "Cannot extract ${pkg}-${ver}.${extension}, error: ${result}\n${errors}")
            endif()

            # "strip" one level from directories.. Unpacked
            file(GLOB files_to_move
                RELATIVE ${source_dir}
                ${source_dir}/*
            )
            list(LENGTH files_to_move num)
            if (num EQUAL 0)
                message(FATAL_ERROR "Empty archive for ${pkg}-${ver}")
            elseif (NOT num EQUAL 1)
                message(FATAL_ERROR "Wrong number of top-level files in ${pkg}-${ver} (${num}: ${files_to_move})")
            endif()
            list(GET files_to_move 0 top_level)
            file(GLOB files_to_move
                RELATIVE ${source_dir}/${top_level}
                ${source_dir}/${top_level}/*
            )
            foreach(name ${files_to_move})
                file(RENAME "${source_dir}/${top_level}/${name}" "${source_dir}/${name}")
            endforeach()
            file(REMOVE_RECURSE "${source_dir}/${top_level}")
        endif()

        # Patch the files..
        if (patch)
            check_executable("patch")
            execute_process(
                COMMAND patch -p0 -i ${patch}
                WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                RESULT_VARIABLE result
            )
            if (NOT ${result} EQUAL 0)
                message(FATAL_ERROR "Error occurs when patching ${pkg}-${ver}")
            endif()
        endif()
    endif()

    # create build directory
    set(binary_dir ${source_dir}/build)
    file(MAKE_DIRECTORY ${binary_dir})

    string(REPLACE "${pkg}-" "" source_version ${source_name})
    string(TOUPPER ${pkg} PKG)
    set(${PKG}_VERSION ${source_version} CACHE INTERNAL "${pkg} version")

    # output variables:
    # - ${PKG}_VERSION: version of the package. If patched, patch name is appended.
    # - source_name: directory name with compiled package (with version).
    # - source_dir: path with compiled package
    # - binary_dir: path with compilation results
endmacro()


macro(set_package_short_name PKG PACKAGE)
    if (${PACKAGE} STREQUAL "GoogleMock")
        set(${PKG} "GMOCK")
    elseif (${PACKAGE} STREQUAL "GoogleTest")
        set(${PKG} "GTEST")
    elseif (${PACKAGE} STREQUAL "DevMapper")
        set(${PKG} "LVM2DEVMAPPER")
    else()
        string(TOUPPER ${PACKAGE} ${PKG})
    endif()
endmacro()


macro(build_and_find_afterward PACKAGE UPPER)
    message(STATUS "Building package ${PACKAGE}/${UPPER}")
    include(Install${PACKAGE})
    message(STATUS "Package ${PACKAGE}/${UPPER} built")

    # do something.. just to appears variables here
    check_executable("sleep")
    execute_process(COMMAND sleep 1)

    # find must find just compiled package
    message(STATUS "Looking for package ${PACKAGE}/${UPPER}")
    find_package(${PACKAGE})
    if (${UPPER}_FOUND)
        set(${UPPER}_BUILT TRUE CACHE INTERNAL "${PACKAGE}/${UPPER} package built")
    else()
        message(FATAL_ERROR "Package ${PACKAGE}/${UPPER} not found after build")
    endif()
endmacro()


function(install_third_party PACKAGE)
    set_package_short_name(UPPER ${PACKAGE})

    if (${UPPER}_BUILT)
        find_package(${PACKAGE})
    else()
        build_and_find_afterward(${PACKAGE} ${UPPER})
    endif()
endfunction()


function(find_or_install PACKAGE)
    set_package_short_name(UPPER ${PACKAGE})

    find_package(${PACKAGE})
    if (${UPPER}_FOUND)
        return()
    endif()

    build_and_find_afterward(${PACKAGE} ${UPPER})
endfunction()


function(find_package_local PACKAGE MODULE)
    set_package_short_name(UPPER ${PACKAGE})
    set(addlibs ${ARGN})

    if (${UPPER}_BUILT)
        message(STATUS "Package ${PACKAGE}/${UPPER}, module ${MODULE} already built")
        set(found TRUE)
    elseif (${UPPER}_FOUND)
        message(STATUS "Package ${PACKAGE}/${UPPER}, module ${MODULE} already found")
        set(found TRUE)
    elseif (addlibs)
        message(STATUS "Find package ${PACKAGE}/${UPPER}, module ${MODULE}, additional libraries ${addlibs}")
    else()
        message(STATUS  "Find package ${PACKAGE}/${UPPER}, module ${MODULE}")
    endif()

    if (NOT found)
        unset(${UPPER}_FOUND CACHE)
        unset(${UPPER}_LIBRARIES CACHE)
        unset(${UPPER}_LIBRARY_DIRS CACHE)
        unset(${UPPER}_INCLUDE_DIRS CACHE)

        # check if pkg-config for package exists.. or is available in tools/pkg-config directory
        # Some packages must be fixed: at least Sysfs has "hardcoded" file on Fedora
        check_executable("pkg-config")
        execute_process(
            COMMAND pkg-config ${MODULE}
            RESULT_VARIABLE result
        )
        if (NOT ${result} EQUAL 0)
            if (EXISTS ${CMAKE_CURRENT_LIST_DIR}/../tools/pkg-config/${MODULE}.pc)
                message(STATUS "Copy missing pkg-config for ${PACKAGE}")
                file(COPY
                    ${CMAKE_CURRENT_LIST_DIR}/../tools/pkg-config/${MODULE}.pc
                    DESTINATION ${CMAKE_BINARY_DIR}/pkg-config
                )
            endif()
        endif()
    endif()

    # check required flag
    if (${PACKAGE}_FIND_REQUIRED)
        set(required "REQUIRED")
    else()
        set(required "")
    endif()

    # get version condition
    if (NOT ${PACKAGE}_FIND_VERSION)
        set(find_args ${MODULE} ${required})
    elseif (${PACKAGE}_FIND_VERSION_EXACT)
        set(find_args ${MODULE}=${${PACKAGE}_FIND_VERSION} ${required})
    else()
        set(find_args ${MODULE}>=${${PACKAGE}_FIND_VERSION} ${required})
    endif()

    # create a lot of internal cache entries.. Some of them are not unset before, so must be cleared!!!
    unset(${PACKAGE}__FOUND CACHE)
    unset(${PACKAGE}__LIBRARIES CACHE)
    unset(${PACKAGE}__LIBRARY_DIRS CACHE)
    unset(${PACKAGE}__LDFLAGS CACHE)
    unset(${PACKAGE}__LDFLAGS_OTHER CACHE)
    unset(${PACKAGE}__INCLUDE_DIRS CACHE)
    unset(${PACKAGE}__CFLAGS CACHE)
    unset(${PACKAGE}__CFLAGS_OTHER CACHE)
    pkg_check_modules(${PACKAGE}_ ${find_args})
    if (NOT ${PACKAGE}__FOUND)
        if (required)
            message(FATAL_ERROR "Cannot find ${PACKAGE}")
        endif()
        return()
    endif()

    # find all necessary library files and create targets for them if not created
    set(added_libs ${ADDED_LIBRARIES})
    set(libraries)
    set(libdirs)
    foreach(library ${${PACKAGE}__LIBRARIES} ${addlibs})
        if (${library}_library)
            message(STATUS "  library ${library} already found: ${${library}_library}")
        else()
            # sets cache entry..
            find_library(${library}_library ${library}
                HINTS ${${PACKAGE}__LIBDIR}
            )
            if (NOT ${library}_library)
                if (required)
                    message(FATAL_ERROR "Cannot find library ${library} required for ${PACKAGE}/${UPPER}")
                endif()
                message(WARNING "Cannot find library ${library} required for ${PACKAGE}/${UPPER}")
                return()
            endif()
        endif()

        list(FIND added_libs ${library} index)
        if (index EQUAL -1)
            # library is found for the first time
            get_filename_component(${library}_libdir ${${library}_library} DIRECTORY CACHE)
            if (NOT ${library}_libdir)
                message(FATAL_ERROR "Cannot find component ${${library}_library}")
            endif()

            string(FIND ${${library}_library} ".so" index REVERSE)
            if (${index} EQUAL -1)
                message(STATUS "  add static ${library} from ${${library}_libdir}")
                add_library(${library} STATIC IMPORTED)
            else()
                message(STATUS "  add shared ${library} from ${${library}_libdir}")
                add_library(${library} SHARED IMPORTED)
            endif()
            set_target_properties(${library} PROPERTIES IMPORTED_LOCATION ${${library}_library})
            list(APPEND added_libs ${library})
        endif()

        list(APPEND libraries ${${library}_library})
        list(APPEND libdirs ${${library}_libdir})
    endforeach(library)

    set(ADDED_LIBRARIES ${added_libs} CACHE INTERNAL "Libraries added during CMake")

    if (libdirs)
        list(REMOVE_DUPLICATES libdirs)
    endif()

    set(incldirs ${${PACKAGE}__INCLUDE_DIRS} ${${PACKAGE}__INCLUDEDIR})
    message(STATUS "includes: ${incldirs}")
    if (NOT librarires)
	message(STATUS "libraries: package doesn't deliver any libraries")
        find_package_handle_standard_args(${UPPER} DEFAULT_MSG
            incldirs
        )
    else()
        message(STATUS "libraries: ${libraries}")
        find_package_handle_standard_args(${UPPER} DEFAULT_MSG
            libraries libdirs incldirs
        )
    endif()


    # set "output" cache entries
    if (${UPPER}_FOUND)
        set(${UPPER}_LIBRARIES ${libraries} CACHE INTERNAL "${PACKAGE}/${UPPER} libraries")
        set(${UPPER}_LIBRARY_DIRS ${libdirs} CACHE INTERNAL "${PACKAGE}/${UPPER} library dirs")
        set(${UPPER}_INCLUDE_DIRS ${incldirs} CACHE INTERNAL "${PACKAGE}/${UPPER} include dirs")
        set(${UPPER}_FOUND ${${UPPER}_FOUND} CACHE INTERNAL "${PACKAGE}/${UPPER} package checked")
    endif()
endfunction()



# module configuration

# list of added library-targets
unset(ADDED_LIBRARIES CACHE)
