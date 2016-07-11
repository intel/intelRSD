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

from cts_core.metadata_validation.get.validators.validator import BaseTypeValidator


class EnumTypeValidator(BaseTypeValidator):
    def validate(self, property_type, property_value):
        try:
            enum_type_description = self._metadata_container.types[property_type]
        except KeyError:
            print "ERROR:: Unknown type %s" % property_type
            return False

        possible_values_list = [member.name for member in enum_type_description.members]

        if property_value in possible_values_list:
            return True
        else:
            print "ERROR:: Field %s has unallowed value " % self.variable_path
            return False
