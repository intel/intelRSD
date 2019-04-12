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
from copy import deepcopy

from cts_core.commons.error import cts_error

from cts_core.metadata.model.annotation import Annotation
from cts_core.metadata.model.metadata_types.metadata_type import MetadataType
from cts_core.metadata.model.metadata_types.metadata_type_categories import MetadataTypeCategories
from cts_core.validation.validation_status import ValidationStatus

NAME = "name"
ANNOTATION = "annotation"
MEMBER = "member"
BASE_TYPE = "basetype"


class EnumMember:
    def __init__(self, member_soup):
        """
        :type member_soup: bs4.element.Tag
        """
        self.name = member_soup[NAME]
        self.annotations = [Annotation(annotation) for annotation in
                            member_soup.find_all(ANNOTATION, recursive=False)]


class EnumType(MetadataType):
    def __init__(self, metadata_container, namespace_name, type_soup, qualifiers=None):
        """
        :type metadata_container: cts_core.metadata.metadata_container.MetadataContainer
        :type namespace_name: str
        :type type_soup: bs4.element.Tag
        """
        MetadataType.__init__(self, metadata_container, namespace_name, type_soup, qualifiers)

        self.type_category = MetadataTypeCategories.ENUM_TYPE
        self.base_type = type_soup.get(BASE_TYPE, None)
        members = [EnumMember(member_soup) for member_soup in type_soup.find_all(MEMBER)]

        self._members = {member.name: member for member in members}

    @property
    def members(self):
        try:
            members = self.metadata_container.types[self.base_type].members.copy()
            members.update(self._members)
        except KeyError:
            members = self._members

        return members

    def validate(self, resource, resource_path, annotations=None):
        """
        :type annotations: dict
        :type resource: dict
        :type resource_path: str
        :rtype: str
        """
        if resource in self.members.keys():
            return ValidationStatus.PASSED
        else:
            cts_error("Property {property_path} has value {actual_value} that does not match requiremenets of enum type " +
                      "{enum_type_name}. Expected one of {expected_values}",
                      property_path=resource_path,
                      actual_value=str(resource),
                      enum_type_name=self.name,
                      expected_values=",".join(self.members.keys()))
            return ValidationStatus.FAILED

    def generate_values(self, annotations):
        """
        :type annotations: list [cts_core.metadata.model.annotation.Annotation]
        :rtype: list
        """
        return self.members.keys()
