/*!
 * @brief Response Errors for Set and Get OOB Boot Options.
 *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file ipmi/command/sdv/iscsi_oob_boot/errors.cpp
 */

#include "ipmi/command/sdv/iscsi_oob_boot/errors.hpp"


using ipmi::CompletionCode;
using namespace ipmi::command::sdv;

constexpr const char InvalidParameterVersion::ERROR_STRING[];
constexpr const CompletionCode InvalidParameterVersion::ERROR_CODE;

InvalidParameterVersion::~InvalidParameterVersion() {}

constexpr const char InvalidIscsiOobResponseLength::ERROR_STRING[];
constexpr const CompletionCode InvalidIscsiOobResponseLength::ERROR_CODE;

InvalidIscsiOobResponseLength::~InvalidIscsiOobResponseLength() {}

constexpr const char HeapResponseLengthMismatch::ERROR_STRING[];
constexpr const CompletionCode HeapResponseLengthMismatch::ERROR_CODE;

HeapResponseLengthMismatch::~HeapResponseLengthMismatch() {}
