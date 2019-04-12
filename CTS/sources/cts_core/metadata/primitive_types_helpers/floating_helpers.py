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

from cts_core.metadata.primitive_types_helpers.base_primitive_type_helper import BasePrimitiveTypeHelper
from cts_core.metadata.primitive_types_helpers.number_helper import NumberHelper


class FloatingNumberHelper(NumberHelper):
    PYTHON_BASE_TYPE = float

    def __init__(self):
        NumberHelper.__init__(self)

        self.FLOATING_POINT_PRECISION = None

    def _validate_precision(self, value):
        if self.FLOATING_POINT_PRECISION is not None:
            value = str(value)
            if "." in value:
                floating_part = value.split(".")[1]
                return len(floating_part) > self.FLOATING_POINT_PRECISION

        return True

    def validate(self, value):
        if not BasePrimitiveTypeHelper.validate(self, value):
            return False

        return self._validate_range(value)


class DoubleHelper(FloatingNumberHelper):
    def __init__(self):
        FloatingNumberHelper.__init__(self)

        self.FLOATING_POINT_PRECISION = 15


class SingleHelper(FloatingNumberHelper):
    def __init__(self):
        FloatingNumberHelper.__init__(self)

        self.FLOATING_POINT_PRECISION = 7


class DecimalHelper(FloatingNumberHelper):
    def __init__(self):
        FloatingNumberHelper.__init__(self)

        self.FLOATING_POINT_PRECISION = 3
