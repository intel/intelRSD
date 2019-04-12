"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION
"""

import re

import rstr

from cts_core.metadata.primitive_types_helpers.base_primitive_type_helper import BasePrimitiveTypeHelper
from cts_core.metadata.primitive_types_helpers.integer_helpers import EDM


class StringHelper(BasePrimitiveTypeHelper):
    EDM_TYPE_NAME = ".".join([EDM, "String"])
    PYTHON_BASE_TYPE = unicode

    def __init__(self):
        BasePrimitiveTypeHelper.__init__(self)

        self.REGEX = None

    def _validate_regex(self, value):
        if self.REGEX is None:
            return True

        return re.match(self.REGEX, value)

    def generate_value(self, avoided_value):
        if not self.REGEX:
            return "Example String Value"
        return rstr.xeger(self.REGEX)


class DateHelper(StringHelper):
    EDM_TYPE_NAME = ".".join([EDM, "Date"])

    def __init__(self):
        StringHelper.__init__(self)

        self.REGEX = "^[0-9]{4}-(0[1-9]|1[0-2])-(0[1-9]|[1-2][0-9]|3[0-1])[tT]([0-1][0-9]|2[0-3]):[0-5][0-9](:[0-5][0-9](\.|)|)"


class DateTimeOffsetHelper(DateHelper):
    EDM_TYPE_NAME = ".".join([EDM, "DateTimeOffset"])


class GuidHelper(StringHelper):
    EDM_TYPE_NAME = ".".join([EDM, "Guid"])

    def __init__(self):
        StringHelper.__init__(self)

        self.REGEX = "^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}$"
