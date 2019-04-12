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
from cts_core.metadata.commons import Commons
from cts_core.metadata.model.metadata_types.metadata_type import MetadataType
from cts_core.metadata.model.metadata_types.metadata_type_categories import MetadataTypeCategories
from cts_core.validation.validation_status import ValidationStatus

UNDERLYING_TYPE = "underlyingtype"


class TypeDefinition(MetadataType):
    def __init__(self, metadata_container, namespace_name, type_soup, qualifiers=None):
        """
        :type metadata_container: cts_core.metadata.metadata_container.MetadataContainer
        :type namespace_name: str
        :type type_soup: bs4.element.Tag
        """
        MetadataType.__init__(self, metadata_container, namespace_name, type_soup, qualifiers)

        self.type_category = MetadataTypeCategories.TYPE_DEFINITION

        self.base_type = Commons.parse_type_name(type_soup[UNDERLYING_TYPE], namespace_name)

    def validate(self, resource, resource_path, annotations=None):
        """
        :type annotations: dict
        :type resource: dict
        :type resource_path: str
        :rtype: str
        """
        try:
            return self.metadata_container.types[self.base_type].validate(resource, resource_path, annotations)
        except KeyError:
            cts_error("Unknown base type {base_type} for type {type_definition} " +
                      "during validation of resource {resource_path}",
                      base_type=self.base_type, type_definition=self.name, resource_path=resource_path)
            return ValidationStatus.FAILED

    def generate_values(self, annotations):
        """
        :type annotations: list [cts_core.metadata.model.annotation.Annotation]
        :rtype: list
        """
        return self.metadata_container.types[self.base_type].generate_values(annotations)
