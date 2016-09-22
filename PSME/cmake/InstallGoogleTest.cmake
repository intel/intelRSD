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

# TODO pass compiler flags

if (CMAKE_CROSSCOMPILING)
    message(WARNING "Not supported for crosscompaling.")
    return()
endif()

function(install_google_test_framework)
    include(ConfigurationPackage OPTIONAL)

    set(GTEST_SOURCE_PACKAGE
        "https://github.com/google/googletest/archive/release-1.7.0.zip"
    )

    set(source_package ${GTEST_SOURCE_PACKAGE})
    get_filename_component(source_package_fname ${source_package} NAME)
    set(source_package_name "googletest-${source_package_fname}")
    string(REGEX REPLACE ".zip" "" source_file ${source_package_name})

    set(source_dir ${CMAKE_BINARY_DIR}/${source_file})
    set(binary_dir ${source_dir}/build)
    if (NOT EXISTS ${source_dir})
        set(download_dir ${CMAKE_CURRENT_LIST_DIR}/../third_party)
        if (NOT EXISTS ${download_dir}/${source_package_name})
            file(DOWNLOAD
                ${source_package}
                ${download_dir}/${source_package_name}
                SHOW_PROGRESS
                STATUS download_result
            )
            if (NOT EXISTS ${download_dir}/${source_package_name})
                message(FATAL_ERROR "Cannot create source package ${source_package_name}")
            endif()
            list(GET download_result 0 result)
            if (NOT ${result} EQUAL 0)
                file(REMOVE ${download_dir}/${source_package_name})
                list(GET download_result 1 result)
                message(FATAL_ERROR "Cannot download package, error ${result}")
            endif()
        endif()

        execute_process(
            COMMAND unzip ${download_dir}/${source_package_name}
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            ERROR_VARIABLE errors
            RESULT_VARIABLE result
        )
        if (NOT ${result} EQUAL 0)
            message(FATAL_ERROR "Errors occure when extracting source bundle: ${result} ${errors}")
        endif()
        if (NOT EXISTS ${source_dir})
            message(FATAL_ERROR "${source_dir} does not exist")
        endif()
        file(MAKE_DIRECTORY ${binary_dir})
    endif()


    set(ARGS)
    list(APPEND ARGS -DCMAKE_PREFIX_PATH:PATH=${CMAKE_BINARY_DIR})
    list(APPEND ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR})

    if (CMAKE_CROSSCOMPILING)
        if (CMAKE_TOOLCHAIN_FILE AND EXISTS ${CMAKE_TOOLCHAIN_FILE})
            list(APPEND ARGS -DCMAKE_TOOLCHAIN_FILE:FILEPATH=${CMAKE_TOOLCHAIN_FILE})
        endif()

        list(APPEND ARGS -DCMAKE_FIND_ROOT_PATH:PATH=${CMAKE_BINARY_DIR})
    endif()

    if (CMAKE_BUILD_TYPE)
        list(APPEND ARGS -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE})
    endif()

    list(APPEND ARGS -DCMAKE_CXX_FLAGS:STRING=-fPIC)

    execute_process(
        COMMAND ${CMAKE_COMMAND} ${ARGS} ${source_dir}
        WORKING_DIRECTORY ${binary_dir}
        RESULT_VARIABLE result
    )

    if (NOT ${result} EQUAL 0)
        message(FATAL_ERROR "Error occurs when configure project")
    endif()

    execute_process(
        COMMAND ${CMAKE_COMMAND} --build ${binary_dir} --target all
            -- ${BUILD_EXTRA_ARGS}
        WORKING_DIRECTORY ${binary_dir}
        RESULT_VARIABLE result
    )

    if (NOT ${result} EQUAL 0)
        message(FATAL_ERROR "Error occurs when build project")
    endif()

    execute_process(
        COMMAND ${CMAKE_COMMAND} -E create_symlink googletest-release-1.7.0 gtest
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
    file(INSTALL ${binary_dir}/libgtest.a
        DESTINATION ${CMAKE_BINARY_DIR}/lib
    )
    file(INSTALL ${source_dir}/include
        DESTINATION ${CMAKE_BINARY_DIR}
    )
endfunction()

install_google_test_framework()
