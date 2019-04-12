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

from cts_core.metadata.comparator import Comparator
from cts_core.metadata.model.annotation import Annotation
from cts_core.validation.validation_status import ValidationStatus

ANNOTATION = "annotation"


class MetadataModel(object):
    def __init__(self, metadata_container, soup, qualifiers=None):
        """
        :type metadata_container: cts_core.metadata.metadata_container.MetadataContainer
        :type soup: bs4.element.Tag
        """
        self.metadata_container = metadata_container
        self.qualifiers = list() if qualifiers is None else list(qualifiers)

        all_annotations = [Annotation(annotation) for annotation in soup.find_all(ANNOTATION, recursive=False)]

        #filter out those with other qualifiers
        self.qualifiers.append(None)
        annotations = [annotation for annotation in all_annotations if annotation.qualifier in self.qualifiers]

        self._annotations = {annotation.term: annotation for annotation in annotations}

    @abstractmethod
    def validate(self, resource, resource_path, annotations=None):
        """
        :type annotations:
        :type resource: dict
        :type resource_path: str
        :rtype: str
        """
        return ValidationStatus.PASSED

    def compare(self, other):
        return Comparator()
