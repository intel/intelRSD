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

if(NOT CPACK_PACKAGE_VERSION_MAJOR)
    set(CPACK_PACKAGE_VERSION_MAJOR 1)
endif()

if (NOT CPACK_PACKAGE_VERSION_MINOR)
    set(CPACK_PACKAGE_VERSION_MINOR 1)
endif()

if (NOT CPACK_PACKAGE_VERSION_PATCH)
    set(CPACK_PACKAGE_VERSION_PATCH 0)
endif()

set(CPACK_PACKAGE_NAME "PSME_FW")
set(CPACK_SOURCE_GENERATOR "TGZ;TBZ2")
set(CPACK_SOURCE_IGNORE_FILES
    "/build\\\\..*/;/build/;/simulator/;/common/valgrind-cxx/src/thread.cpp;/third_party/;/common/cy7c/;.ycm_extra_conf.p.*;.gitignore;CODING_STYLE.md")

if(CPACK_SOURCE_AGENT_STUBS_ONLY)
    # include agents-stubs - ignore /agent folder
    message(WARNING "'agent' folder not included for cpack.")
    set(CPACK_SOURCE_IGNORE_FILES ${CPACK_SOURCE_IGNORE_FILES}
        "${PROJECT_SOURCE_DIR}/agent/;/cmake/FindIES.cmake;/cmake/InstallIES.cmake;/cmake/InstallLvm2App.cmake;/cmake/InstallSysfs.cmake;/cmake/InstallOpenIPMI.cmake")
else()
    # include agents - ignore /agent-stubs folder
    message(WARNING "'agent-stubs' folder not included for cpack.")
    set(CPACK_SOURCE_IGNORE_FILES ${CPACK_SOURCE_IGNORE_FILES} "${PROJECT_SOURCE_DIR}/agent-stubs/")
endif()

include(CPack)
