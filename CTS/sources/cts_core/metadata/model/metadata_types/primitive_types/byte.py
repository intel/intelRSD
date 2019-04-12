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
from cts_core.commons.error import cts_error
from cts_core.metadata.model.metadata_types.primitive_types.primitive_type import PrimitiveType
from cts_core.validation.validation_status import ValidationStatus

@PrimitiveType.register(".".join([PrimitiveType.EDM, "Byte"]))
class Byte(PrimitiveType):
    def __init__(self):
        PrimitiveType.__init__(self)

    def validate(self, resource, resource_path, annotations=None):
        """
        :type annotations: dict
        :type resource: *
        :type resource_path: str
        :rtype: str
        """
        if type(resource) != int:
            cts_error("Value {value} of property {resource_path} is not an Integer but {typ}.",
                      value=resource, resource_path=resource_path, typ=type(resource))
            return ValidationStatus.FAILED
        elif 0 <= resource <= 255:
            return ValidationStatus.PASSED
        else:
            cts_error("Value of property {resource_path} is not correct byte value. Shall be in range <0,255>.",
                      resource_path=resource_path)
            return ValidationStatus.FAILED

    def generate_values(self, annotations):
        """
        :type annotations: dict [str, cts_core.metadata.model.annotation.Annotation]
        :rtype: list
        """
        if "Validation.Minimum" in annotations:
            minimum = int(annotations["Validation.Minimum"].value)
        else:
            minimum = 0

        if "Validation.Maximum" in annotations:
            maximum = int(annotations["Validation.Maximum"].value)
        else:
            maximum = 255

        return set([i for i in [minimum, maximum, (minimum + maximum) / 2, (minimum + maximum) / 2 + 1] if
                    minimum <= i <= maximum])
