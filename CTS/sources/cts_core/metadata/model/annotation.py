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

import collections
import copy

from bs4 import Tag

from cts_core.metadata.comparator import Comparator

TERM = "term"
QUALIFIER = "qualifier"

DynamicPropertyPattern = collections.namedtuple('DynamicPropertyPattern', ['pattern', 'type'])


class Annotation:
    ADDITIONAL_PROPERTIES = "OData.AdditionalProperties"
    DYNAMIC_PROPERTY_PATTERNS = "Redfish.DynamicPropertyPatterns"
    PATTERN = "Pattern"
    TYPE = "Type"
    REDFISH_URIS = "Redfish.Uris"

    def __init__(self, annotation_soup):
        """
        :type annotation_soup: bs4.element.Tag

        """
        self.term = None
        self.redfish_uris = []
        self.qualifier = None
        self.value = None
        self.value_type = None
        self.dynamic_property_patterns = []
        self.annotations_attributes_list = annotation_soup.attrs
        try:
            annotations_attributes_list = copy.deepcopy(annotation_soup.attrs)
            self.term = annotations_attributes_list.pop(TERM, None)
            self.qualifier = annotations_attributes_list.pop(QUALIFIER, None)

            if self.term == Annotation.REDFISH_URIS:
                self._parse_redfish_uris_patters(annotation_soup)
            elif self.term == Annotation.DYNAMIC_PROPERTY_PATTERNS:
                self.parse_dynamic_property_patterns(annotation_soup)
            else:
                self.value_type, self.value = annotations_attributes_list.items()[0]
        except IndexError:
            pass

    def _parse_redfish_uris_patters(self, annotation_soup):
        for redfish_uris_tag in [redfish_uris_tag for redfish_uris_tag in annotation_soup.contents
                                 if isinstance(redfish_uris_tag, Tag)]:
            for content_element in [content_element.contents[0] for content_element in redfish_uris_tag.contents
                                    if isinstance(content_element, Tag)]:
                self.redfish_uris.append(content_element)

    def parse_dynamic_property_patterns(self, annotation_soup):
        self.dynamic_property_patterns = []
        for collection in annotation_soup.find_all('collection'):
            for record in collection.find_all('record'):
                pattern = odata_type = None
                for propertyvalue in record.find_all('propertyvalue'):
                    try:
                        property = propertyvalue.attrs['property']
                        value = propertyvalue.attrs['string']

                        if property == Annotation.PATTERN:
                            pattern = value
                        elif property == Annotation.TYPE:
                            odata_type = value
                    except IndexError:
                        pass

                if pattern is not None and odata_type is not None:
                    self.dynamic_property_patterns.append(DynamicPropertyPattern(pattern, odata_type))

    def compare(self, other, level):
        """
        :type other: cts_core.metadata.model.annotation.Annotation
        :rtype: int
        """
        cmp = Comparator(level)
        cmp.message_both("Annotation: {name}".format(name=self.term))

        attributes = set(self.annotations_attributes_list.keys())
        attributes_other = set(other.annotations_attributes_list.keys())
        all = sorted(list(attributes.union(attributes_other)))
        both = attributes.intersection(attributes_other)
        fmt = "  {name}={value}"
        for attribute in all:
            if attribute in both:
                if self.annotations_attributes_list[attribute] != other.annotations_attributes_list[attribute]:
                    cmp.set_not_equal()
                    cmp.message_left(fmt.format(name=attribute, value=self.annotations_attributes_list[attribute]))
                    cmp.message_right(fmt.format(name=attribute, value=other.annotations_attributes_list[attribute]))
            elif attribute in attributes:
                cmp.set_not_equal()
                cmp.message_left(fmt.format(name=attribute, value=self.annotations_attributes_list[attribute]))
                cmp.message_right('?')
            else:
                cmp.set_not_equal()
                cmp.message_left('?')
                cmp.message_right(fmt.format(name=attribute, value=other.annotations_attributes_list[attribute]))
        return cmp
