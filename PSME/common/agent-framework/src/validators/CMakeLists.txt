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

add_library(agent-framework-validators STATIC
    procedure_validator.cpp

    checkers/validity_checker.cpp
    checkers/composite_validity_checker.cpp
    checkers/optional_validity_checker.cpp
    checkers/null_allowed_validity_checker.cpp
    checkers/number_validity_checker.cpp
    checkers/jsonrpc_validity_checker.cpp
    checkers/uuid_validity_checker.cpp
    checkers/array_validity_checker.cpp
    checkers/always_fail_validity_checker.cpp
    checkers/enum_validity_checker.cpp
    checkers/attribute_validity_checker.cpp
    checkers/regex_validity_checker.cpp
)

target_link_libraries(agent-framework-validators
    PUBLIC
    agent-framework-exceptions

    PRIVATE
    common-include
)

target_include_directories(agent-framework-validators
    PUBLIC
    ${AGENT_FRAMEWORK_DIR}/include
)