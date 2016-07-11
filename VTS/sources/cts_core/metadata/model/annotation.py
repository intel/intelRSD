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

import copy

TERM = "term"


class Annotation:
    def __init__(self, annotation_soup):
        self.term = annotation_soup[TERM]
        try:
            annotations_attributes_list = copy.deepcopy(annotation_soup.attrs)
            annotations_attributes_list.pop(TERM, None)
            self.value_type, self.value = annotations_attributes_list.items()[0]
        except IndexError:
            # using default values of term
            self.value = None
            self.value_type = None
