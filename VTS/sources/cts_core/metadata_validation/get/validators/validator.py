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


class BaseValidator:
    def __init__(self, metadata_container):
        self._metadata_container = metadata_container

    def verify_property(self, property_value):
        return True, list()


class PropertyValidator(BaseValidator):
    def __init__(self, property_description, metadata_container):
        BaseValidator.__init__(self, metadata_container)

        self._property_description = property_description


class AnnotationValidator(BaseValidator):
    def __init__(self, annotation_description, metadata_container):
        BaseValidator.__init__(self, metadata_container)

        self._annotation_description = annotation_description


class BaseTypeValidator(BaseValidator):
    def __init__(self, variable_path, metadata_container):
        BaseValidator.__init__(self, metadata_container)

        if not variable_path:
            variable_path = []

        self.variable_path = "::".join([i for i in variable_path if i])
