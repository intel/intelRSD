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

from cts_core.commons.error import cts_error
from cts_core.metadata.model.metadata_types.primitive_types.primitive_type import PrimitiveType
from cts_core.validation.validation_status import ValidationStatus

@PrimitiveType.register(".".join([PrimitiveType.EDM, "Guid"]))
class Guid(PrimitiveType):
    def __init__(self):
        PrimitiveType.__init__(self)
        self.regex = "^[0-9a-fA-F]{8}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{4}-[0-9a-fA-F]{12}$"

    def validate(self, resource, resource_path, annotations=None):
        """
        :type annotations: dict
        :type resource: str
        :type resource_path: str
        :rtype: str
        """
        if annotations is None:
            annotations = dict()

        if type(resource) not in [str, basestring, unicode]:
            cts_error("Value of property {resource_path} is not string.", resource_path=resource_path)
            return ValidationStatus.FAILED
        else:
            if not re.match(self.regex, resource):
                cts_error("Value of property {resource_path} is does not match Guid regular expression.",
                          resource_path=resource_path)
                return ValidationStatus.FAILED

            try:
                if re.match(annotations["Validation.Pattern"].value, resource):
                    return ValidationStatus.PASSED
                else:
                    cts_error("Value of property {resource_path} does not match annotated pattern {pattern}",
                              resource_path=resource_path, pattern=str(annotations["Validation.Pattern"].value))
                    return ValidationStatus.FAILED
            except KeyError:
                return ValidationStatus.PASSED

    def generate_values(self, annotations):
        """
        :type annotations: dict [str, cts_core.metadata.model.annotation.Annotation]
        :rtype: list
        """
        return ["c5ced6a-5e4e-4a90-98d3-7a9fe65c7ca4", "AAAAAAA-AAAA-AAAA-AAAA-AAAAAAAAAAAA",
                "0000000-0000-0000-0000-000000000000"]
