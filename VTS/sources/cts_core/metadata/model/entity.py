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
from cts_core.metadata.model.navigation_property import NavigationProperty
from cts_core.metadata.model.property import Property

ANNOTATION = "annotation"
PROPERTY = "property"
NAVIGATION_PROPERTY = "navigationproperty"
BASE_TYPE = "basetype"
NAME = "name"
ABSTRACT = "abstract"


class Entity:
    def __init__(self, namespace_name, entity_soup):
        self.name = Commons.parse_type_name(entity_soup[NAME], namespace_name)
        try:
            self.base_type = Commons.parse_type_name(entity_soup[BASE_TYPE], namespace_name)
        except KeyError:
            # does not have base type (item?)
            self.base_type = None
        self.abstract = entity_soup.get(ABSTRACT, False)
        self.annotation = [Annotation(annotation_soup) for annotation_soup in
                            entity_soup.find_all(ANNOTATION, recursive=False)]
        self.properties = [Property(namespace_name, property_soup) for property_soup in
                           entity_soup.find_all(PROPERTY, recursive=False)]
        self.navigation_properties = [NavigationProperty(namespace_name, navigation_property_soup) for
                                      navigation_property_soup in
                                      entity_soup.find_all(NAVIGATION_PROPERTY, recursive=False)]

    def base_entity_on(self, base_entity):
        self.annotation += base_entity.annotation
        self.properties += base_entity.properties
        self.navigation_properties += base_entity.navigation_properties
