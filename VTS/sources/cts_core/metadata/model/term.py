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

NAME = "name"
TYPE = "type"
ANNOTATION = "annotation"


class Term:
    def __init__(self, namespace_name, term_soup):
        self.name = term_soup[NAME]
        if "." not in self.name:
            self.name = ".".join([namespace_name, self.name])

        self.type = term_soup[TYPE]
        self.annotations = [Annotation(annotation) for annotation in term_soup.find_all(ANNOTATION,
                                                                                        recursive=False)]
