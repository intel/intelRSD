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
from cts_core.metadata.metadata_enums import ValueTypesCategory


class Container(dict):
    pass


class NamespacesSoupsContainer(Container):
    pass


class TermsContainer(Container):
    pass


class EntitiesContainer(Container):
    def compare_entity_types(self, compared_type, reference_type):
        if not compared_type:
            # type to check is base, so in all hierarchy haven't been element in common
            return False

        if compared_type == reference_type:
            # found equal entity type
            return True

        try:
            # try to find in checked type base element match
            return self.compare_entity_types(self[compared_type].base_type, reference_type)
        except KeyError:
            return False
        except AttributeError:
            return False


class TypesContainer(Container):
    def get_type_category(self, type_name):
        try:
            return self[type_name].type
        except KeyError:
            if type_name.startswith("Edm."):
                return ValueTypesCategory.PRIMITIVE_TYPE
            else:
                return ValueTypesCategory.UNKNOWN_TYPE

    def get_base_type(self, type_name):
        if self.get_type_category(type_name) == ValueTypesCategory.TYPE_DEFINITION:
            return self.get_base_type(self[type_name].base_type)
        else:
            return type_name

    def get_type_annotations(self, type_name):
        annotations = list()
        try:
            annotations += self[type_name].annotations
        except KeyError:
            pass

        if self.get_type_category(type_name) == ValueTypesCategory.TYPE_DEFINITION:
            annotations += self.get_type_annotations(self[type_name].base_type)

        return annotations

    def compare_types(self, base_type, extended_type):
        if base_type == extended_type:
            return True
        else:
            try:
                return self.compare_types(base_type, self[extended_type].base_type)
            except:
                return False


class ActionsContainer(Container):
    pass


class MetadataContainer:
    def __init__(self):
        self.entities = EntitiesContainer()
        self.types = TypesContainer()
        self.terms = TermsContainer()
        self.actions = ActionsContainer()
