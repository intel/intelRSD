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

@PrimitiveType.register(".".join([PrimitiveType.EDM, "Single"]))
class Single(PrimitiveType):
    def __init__(self):
        PrimitiveType.__init__(self)

    def validate(self, resource, resource_path, annotations=None):
        """
        :type annotations: dict
        :type resource: float
        :type resource_path: str
        :rtype: str
        """
        if annotations is None:
            annotations = dict()

        if type(resource) not in [float, int]:
            cts_error("Value of property {resource_path} is not floating point number.",
                      resource_path=resource_path)
            return ValidationStatus.FAILED

        if "Validation.Minimum" in annotations:
            try:
                minimum = float(annotations["Validation.Minimum"].value)
            except:
                minimum = float('-inf')
        else:
            minimum = float('-inf')

        if "Validation.Maximum" in annotations:
            try:
                maximum = float(annotations["Validation.Maximum"].value)
            except:
                maximum = float('inf')
        else:
            maximum = float('inf')

        if minimum <= resource <= maximum:
            return ValidationStatus.PASSED
        else:
            cts_error("Value {value} of property {resource_path} does not match range <{minimum}, {maximum}>.",
                      value=str(resource), resource_path=resource_path, minimum=str(minimum), maximum=str(maximum))
            return ValidationStatus.FAILED

    def generate_values(self, annotations):
        """
        :type annotations: dict [str, cts_core.metadata.model.annotation.Annotation]
        :rtype: list
        """
        if "Validation.Minimum" in annotations:
            minimum = annotations["Validation.Minimum"].value
        else:
            minimum = float(-200.4)

        if "Validation.Maximum" in annotations:
            maximum = annotations["Validation.Maximum"].value
        else:
            maximum = float(200.4)

        return set([i for i in [minimum, maximum, 0, (minimum + maximum) / 2, (minimum + maximum) / 2 + 1] if
                    minimum <= i <= maximum])
