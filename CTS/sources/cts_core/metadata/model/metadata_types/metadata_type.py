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
from abc import abstractmethod
from copy import deepcopy

from cts_core.metadata.comparator import Comparator

from cts_core.metadata.commons import Commons
from cts_core.metadata.model.metadata_model import MetadataModel


NAME = "name"


class MetadataType(MetadataModel):
    def __init__(self, metadata_container, namespace_name, type_soup, qualifiers=None):
        """
        :type metadata_container: cts_core.metadata.metadata_container.MetadataContainer
        :type namespace_name: str
        :type type_soup: bs4.element.Tag
        """
        MetadataModel.__init__(self, metadata_container, type_soup, qualifiers)

        self.name = Commons.parse_type_name(type_soup[NAME], namespace_name)
        self.type_category = None
        self.base_type = None

    @property
    def annotations(self):
        try:
            annotations = self.metadata_container.types[self.base_type].annotations.copy()
            annotations.update(self._annotations)
        except KeyError:
            annotations = self._annotations

        return annotations

    @abstractmethod
    def generate_values(self, annotations):
        """
        :type annotations: list [cts_core.metadata.model.annotation.Annotation]
        :rtype: list
        """
        pass

    def compare(self, other):
        return Comparator()
