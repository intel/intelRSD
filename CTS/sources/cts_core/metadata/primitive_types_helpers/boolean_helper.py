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

from random import randint

from cts_core.metadata.primitive_types_helpers.base_primitive_type_helper import BasePrimitiveTypeHelper


class BooleanHelper(BasePrimitiveTypeHelper):
    EDM_TYPE_NAME = "Edm.Boolean"
    PYTHON_BASE_TYPE = bool

    def generate_value(self, avoided_value=None):
        if avoided_value is None:
            return bool(randint(0, 1))

        return not avoided_value
