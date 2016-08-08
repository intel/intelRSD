"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016 Intel Corporation
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

from cts_core.metadata.primitive_types_helpers.base_primitive_type_helper import BasePrimitiveTypeHelper


class NumberHelper(BasePrimitiveTypeHelper):
    def __init__(self):
        BasePrimitiveTypeHelper.__init__(self)

        self.MINIMUM_VALUE = None
        self.MAXIMUM_VALUE = None

    def _validate_range(self, value):
        if self.MINIMUM_VALUE is not None and value < self.MINIMUM_VALUE:
            return False

        if self.MAXIMUM_VALUE is not None and value > self.MAXIMUM_VALUE:
            return False

        return True

    def validate(self, value):
        if not BasePrimitiveTypeHelper.validate(self, value):
            return False

        return self._validate_range(value)

    def generate_value(self, avoided_value=None, minimum=None, maximum=None):
        try:
            minimum = int(minimum)
        except (ValueError, TypeError):
            minimum = -10000 if self.MINIMUM_VALUE is None else self.MINIMUM_VALUE

        try:
            maximum = int(maximum)
        except (ValueError, TypeError):
            maximum = 10000 if self.MAXIMUM_VALUE is None else self.MAXIMUM_VALUE

        if type(avoided_value) == float:
            middle_point = (minimum + maximum) / 2 + 2
        elif type(avoided_value) == int:
            middle_point = int(minimum + maximum) / 2 + 2
        else:
            middle_point = 1

        return list({i for i in [minimum, middle_point, avoided_value, maximum] if i is not None})
