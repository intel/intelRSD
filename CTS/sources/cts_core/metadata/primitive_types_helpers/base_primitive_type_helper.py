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

from abc import abstractmethod

EDM = "Edm"


class BasePrimitiveTypeHelper:
    EDM_TYPE_NAME = None
    PYTHON_BASE_TYPE = None

    def __init__(self):
        pass

    def validate(self, value):
        if self.PYTHON_BASE_TYPE == float and type(value) == int:
            # int is also kind of float
            value = float(value)

        if type(value) != self.PYTHON_BASE_TYPE:
            return False

        return self._validate_value(value)

    def _validate_value(self, value):
        return True

    @abstractmethod
    def generate_value(self, current_value):
        pass
