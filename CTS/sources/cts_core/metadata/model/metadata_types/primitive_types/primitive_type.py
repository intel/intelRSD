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

from cts_core.metadata.model.metadata_model import MetadataModel
from cts_core.metadata.model.metadata_types.metadata_type_categories import MetadataTypeCategories
from cts_core.validation.validation_status import ValidationStatus


class PrimitiveTypeMetaclass(type):
    def __init__(cls, name, bases, dct):
        cls.PRIMITIVE_TYPES_LIST = list()
        cls.PRIMITIVE_TYPE_NAME_LIST = list()

    def register(cls, type_name):
        if type_name is None:
            raise Exception("Type name expected")
        def inner_register(primitive_cls):
            primitive_cls._name = type_name
            PrimitiveType.PRIMITIVE_TYPES_LIST.append(primitive_cls)
            PrimitiveType.PRIMITIVE_TYPE_NAME_LIST.append(primitive_cls._name)
            return primitive_cls
        return inner_register


class PrimitiveType(MetadataModel):
    __metaclass__ = PrimitiveTypeMetaclass

    EDM = "Edm"

    def __init__(self):
        self.type_category = MetadataTypeCategories.PRIMITIVE_TYPE

    @abstractmethod
    def validate(self, resource, resource_path, annotations=None):
        """
        :type annotations: dict
        :type resource: dict
        :type resource_path: str
        :rtype: str
        """

        return ValidationStatus.PASSED

    @property
    def annotations(self):
        return dict()

    @abstractmethod
    def generate_values(self, annotations):
        """
        :type annotations: dict [str, cts_core.metadata.model.annotation.Annotation]
        :rtype: list
        """
        pass

    @property
    def name(self):
        return self.__class__._name
