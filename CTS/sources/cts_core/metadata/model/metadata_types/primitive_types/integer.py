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


class Integer(PrimitiveType):
    def __init__(self, size):
        PrimitiveType.__init__(self)
        self.size = size

    def validate(self, resource, resource_path, annotations=None):
        """
        :type annotations: dict
        :type resource: *
        :type resource_path: str
        :rtype: str
        """
        if annotations is None:
            annotations = dict()

        if not isinstance(resource, (int, long)):
            cts_error("Value {value} of property {resource_path} is not an Integer but {typ}.",
                      value=resource, resource_path=resource_path, typ=type(resource))
            return ValidationStatus.FAILED
        elif self._min_range_value <= resource <= self._max_range_value:
            if "Validation.Minimum" in annotations:
                try:
                    minimum = int(annotations["Validation.Minimum"].value)
                except:
                    minimum = self._max_range_value
            else:
                minimum = self._min_range_value

            if "Validation.Maximum" in annotations:
                try:
                    maximum = int(annotations["Validation.Maximum"].value)
                except:
                    maximum = self._max_range_value
            else:
                maximum = self._max_range_value

            if minimum <= resource <= maximum:
                return ValidationStatus.PASSED
            else:
                cts_error("Value of property {resource_path} does not follow annotated range <{min}, {max}>",
                          resource_path=resource_path, min=str(minimum), max=str(maximum))
                return ValidationStatus.FAILED

        else:
            cts_error("Value {value} of property {resource_path} does not match range <{minimum}, {maximum}>.",
                      value=str(resource),
                      resource_path=resource_path,
                      minimum=str(self._min_range_value),
                      maximum=str(self._max_range_value))
            return ValidationStatus.FAILED

    @property
    def _min_range_value(self):
        """
        :rtype: int
        """
        return -1 * (2 ** (self.size - 1))

    @property
    def _max_range_value(self):
        """
        :rtype: int
        """
        return 2 ** (self.size - 1) - 1

    def generate_values(self, annotations):
        """
        :type annotations: dict [str, cts_core.metadata.model.annotation.Annotation]
        :rtype: list
        """
        if "Validation.Minimum" in annotations:
            minimum = int(annotations["Validation.Minimum"].value)
        else:
            minimum = int(self._min_range_value)

        if "Validation.Maximum" in annotations:
            maximum = int(annotations["Validation.Maximum"].value)
        else:
            maximum = int(self._max_range_value)

        return set([i for i in [minimum, maximum, 0, (minimum + maximum) / 2, 1, (minimum + maximum) / 2 + 1] if
                minimum <= i <= maximum])


@PrimitiveType.register("%s.Int%s" % (PrimitiveType.EDM, 16))
class Integer16(Integer):
    def __init__(self):
        Integer.__init__(self, 16)

@PrimitiveType.register("%s.Int%s" % (PrimitiveType.EDM, 32))
class Integer32(Integer):
    def __init__(self):
        Integer.__init__(self, 32)

@PrimitiveType.register("%s.Int%s" % (PrimitiveType.EDM, 64))
class Integer64(Integer):
    def __init__(self):
        Integer.__init__(self, 64)
