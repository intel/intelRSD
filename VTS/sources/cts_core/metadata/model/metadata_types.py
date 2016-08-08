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

from cts_core.metadata.model.annotation import Annotation
from cts_core.metadata.commons import Commons
from cts_core.metadata.metadata_enums import ValueTypesCategory
from cts_core.metadata.model.navigation_property import NavigationProperty
from cts_core.metadata.model.property import Property

NAME = "name"
ANNOTATION = "annotation"
MEMBER = "member"
PROPERTY = "property"
UNDERLYING_TYPE = "underlyingtype"
NAVIGATION_PROPERTY = "navigationproperty"
BASE_TYPE = "basetype"


class Type:
    """
    base types class
    """
    def __init__(self, namespace_name, type_soup):
        self.name = Commons.parse_type_name(type_soup[NAME], namespace_name)
        self.annotations = [Annotation(annotation) for annotation in type_soup.find_all(ANNOTATION, recursive=False)]
        self.type = None


class EnumType(Type):
    class Member:
        def __init__(self, member_soup):
            self.name = member_soup[NAME]
            self.annotations = [Annotation(annotation) for annotation in
                                member_soup.find_all(ANNOTATION, recursive=False)]

    def __init__(self, namespace_name, type_soup):
        Type.__init__(self, namespace_name, type_soup)

        self.type = ValueTypesCategory.ENUM_TYPE

        self.members = list()
        self.members = [EnumType.Member(member_soup) for member_soup in type_soup.find_all(MEMBER)]


class ComplexType(Type):
    def __init__(self, namespace_name, type_soup):
        Type.__init__(self, namespace_name, type_soup)
        try:
            self.base_type = Commons.parse_type_name(type_soup[BASE_TYPE], namespace_name)
        except KeyError:
            pass

        self.type = ValueTypesCategory.COMPLEX_TYPE
        self.properties = [Property(namespace_name, property_soup) for property_soup in
                           type_soup.find_all(PROPERTY, recursive=False)]
        self.navigation_properties = [NavigationProperty(namespace_name, property_soup) for property_soup in
                                      type_soup.find_all(NAVIGATION_PROPERTY, recursive=False)]

        self.additional_items = True
        self.set_additional_items()

    def set_additional_items(self):
        self.additional_items = True
        for annotation in self.annotations:
            if annotation.term == "OData.AdditionalProperties":
                self.additional_items = (annotation.value.lower() == "true")
                return


class TypeDefinition(Type):
    def __init__(self, namespace_name, type_soup):
        Type.__init__(self, namespace_name, type_soup)

        self.type = ValueTypesCategory.TYPE_DEFINITION
        base_type = type_soup[UNDERLYING_TYPE]
        self.base_type = Commons.parse_type_name(base_type, namespace_name) if base_type else base_type
