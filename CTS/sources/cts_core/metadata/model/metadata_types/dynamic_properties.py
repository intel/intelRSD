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

import re


def match_dynamic_property(name, container_type_definition):
    for dynamic_property_pattern in container_type_definition.dynamic_property_patterns:
        pattern = dynamic_property_pattern.pattern
        type = dynamic_property_pattern.type

        if not pattern or pattern[0] != '^':
            pattern = '^' + pattern
        if pattern[-1] != '$':
            pattern = pattern + '$'

        if re.match(pattern, name) is not None:
            return type
    return None
