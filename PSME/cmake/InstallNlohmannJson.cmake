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

function(install_nlohmann_json)
    assure_package(nlohmann_json 2.1.1 "https://github.com/nlohmann/json/releases/download/v2.1.1/json.hpp" "871e326cf493d17bce4578656759283d")

    file(INSTALL ${source_dir}/json.hpp
        DESTINATION ${CMAKE_BINARY_DIR}/include
    )

    configure_file(
        ${CMAKE_CURRENT_LIST_DIR}/../tools/pkg-config/nlohmann_json.pc.in
        ${CMAKE_BINARY_DIR}/pkg-config/nlohmann_json.pc
        @ONLY
    )
endfunction()

install_nlohmann_json()
