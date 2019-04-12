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

from cts_core.metadata.model.property import Property

NAME = "name"
TYPE = "type"
CONTAINS_TARGET = "containstarget"
ANNOTATION = "annotation"
COLLECTION = "Collection("


class NavigationProperty(Property):
    def __init__(self, metadata_container, namespace_name, navigation_property_soup, qualifiers):
        """
        :type metadata_container: cts_core.metadata.metadata_container.MetadataContainer
        :type namespace_name: str
        :type navigation_property_soup: bs4.element.Tag
        """
        Property.__init__(self, metadata_container, namespace_name, navigation_property_soup, qualifiers)

        self.contains_target = navigation_property_soup.get(CONTAINS_TARGET, False)


    @property
    def annotations(self):
        """
        :rtype: dict[str, cts_core.metadata.model.annotation.Annotation]
        """
        return self._annotations

    @property
    def is_required(self):
        """
        :rtype: bool
        """
        return "Redfish.Required" in self.annotations
