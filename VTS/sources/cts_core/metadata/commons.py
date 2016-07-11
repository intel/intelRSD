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


class Commons:
    """
    common class for metadata model
    """
    @staticmethod
    def parse_type_name(type_name, namespace_name):
        if "." not in type_name:
            type_name = ".".join([namespace_name, type_name])

        return type_name

    @staticmethod
    def compare_entity_types(expected_type, type_to_check, entities_container):
        if not type_to_check:
            # type to check is base, so in all hierarchy haven't been element in common
            return False

        if expected_type == type_to_check:
            # found equal entity type
            return True

        try:
            # try to find in checked type base element match
            return Commons.compare_entity_types(expected_type, entities_container[type_to_check].base_type)
        except KeyError:
            # unknown type
            return False
