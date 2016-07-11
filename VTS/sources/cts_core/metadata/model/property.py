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

NAME = "name"
TYPE = "type"
NULLABLE = "nullable"
ANNOTATION = "annotation"
COLLECTION = "Collection("


class Property:
    def __init__(self, namespace_name, property_soup):
        self.name = property_soup[NAME]
        self.type = Commons.parse_type_name(property_soup[TYPE], namespace_name)
        self.is_collection = self.type.startswith(COLLECTION)
        self.type = self.type[len(COLLECTION):-1] if self.is_collection else self.type

        self.annotations = [Annotation(annotation) for annotation in
                            property_soup.find_all(ANNOTATION, recursive=False)]

        self.nullable = property_soup.get(NULLABLE, "true").lower() != "false"
        self.required = bool([annotation for annotation in
                              self.annotations if annotation.term == "Redfish.Required"])
