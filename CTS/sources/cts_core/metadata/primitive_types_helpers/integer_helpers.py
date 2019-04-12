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
from cts_core.metadata.primitive_types_helpers.base_primitive_type_helper import EDM
from cts_core.metadata.primitive_types_helpers.number_helper import NumberHelper


class IntegerHelper(NumberHelper):
    def __init__(self):
        NumberHelper.__init__(self)

        self.PYTHON_BASE_TYPE = int


class ByteHelper(IntegerHelper):
    EDM_TYPE_NAME = ".".join([EDM, "Byte"])

    def __init__(self):
        IntegerHelper.__init__(self)

        self.MINIMUM_VALUE = 0
        self.MAXIMUM_VALUE = 2 ** 8


class Int16Helper(IntegerHelper):
    EDM_TYPE_NAME = ".".join([EDM, "Int16"])

    def __init__(self):
        IntegerHelper.__init__(self)

        self.MINIMUM_VALUE = (-1) * (2 ** 15)
        self.MAXIMUM_VALUE = (2 ** 15) - 1


class Int32Helper(IntegerHelper):
    EDM_TYPE_NAME = ".".join([EDM, "Int32"])

    def __init__(self):
        IntegerHelper.__init__(self)

        self.MINIMUM_VALUE = (-1) * (2 ** 31)
        self.MAXIMUM_VALUE = (2 ** 31) - 1


class Int64Helper(IntegerHelper):
    EDM_TYPE_NAME = ".".join([EDM, "Int64"])

    def __init__(self):
        IntegerHelper.__init__(self)

        self.MINIMUM_VALUE = (-1) * (2 ** 63)
        self.MAXIMUM_VALUE = (2 ** 63) - 1


class SByteHelper(IntegerHelper):
    EDM_TYPE_NAME = ".".join([EDM, "SByte"])

    def __init__(self):
        IntegerHelper.__init__(self)

        self.MINIMUM_VALUE = (-1) * (2 ** 7)
        self.MAXIMUM_VALUE = (2 ** 7) - 1
