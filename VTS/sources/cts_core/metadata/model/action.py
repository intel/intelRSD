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
from cts_core.metadata.model.property import Property
from cts_core.metadata.commons import Commons

IS_BOUND = "isbound"
PROPERTY = "property"
ANNOTATION = "annotation"


class Action:
    def __init__(self, namespace_name, action_soup):
        self.name = Commons.parse_type_name(action_soup["name"], namespace_name)
        try:
            self.is_bound = action_soup[IS_BOUND] == "true"
        except KeyError:
            self.is_bound = False

        self.properties = [Property(namespace_name, property_soup) for property_soup in
                           action_soup.find_all(PROPERTY, recursive=False)]

        self.annotation = [Annotation(annotation_soup) for annotation_soup in
                            action_soup.find_all(ANNOTATION, recursive=False)]
