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

@PrimitiveType.register("Null")
class Null(PrimitiveType):
    def __init__(self):
        PrimitiveType.__init__(self)

    def validate(self, resource, resource_path, annotations=None):
        """
        :type annotations: dict
        :type resource: *
        :type resource_path: str
        :rtype: str
        """
        if resource != None:
            cts_error("Value of property {resource_path} is not null but {resource}",
                      resource_path=resource_path, resource=resource)
            return ValidationStatus.FAILED
        else:
            return  ValidationStatus.PASSED

    def generate_values(self, annotations):
        """
        :type annotations: dict [str, cts_core.metadata.model.annotation.Annotation]
        :rtype: list
        """
        return [None]
