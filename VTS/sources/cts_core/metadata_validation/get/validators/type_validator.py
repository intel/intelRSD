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
from cts_core.metadata_validation.get.validators.enum_type_validator import EnumTypeValidator
from cts_core.metadata_validation.get.validators.primitive_types_validator import PrimitiveTypeValidator
from cts_core.metadata_validation.get.validators.validator import BaseTypeValidator, PropertyValidator


class TypeValidator(PropertyValidator):
    def __init__(self, property_description, metadata_container, variable_path=None):
        PropertyValidator.__init__(self, property_description, metadata_container)

        if not variable_path:
            variable_path = []

        self.variable_path = "::".join([i for i in variable_path + [property_description.name] if i])

    def verify_property(self, property_value):
        if self._property_description.is_collection:
            return self.process_list_attribute(property_value)
        else:
            return self.verify_single_property_value(property_value)

    def verify_single_property_value(self, property_value):
        if property_value is None:
            return self.check_nullable_property()
        return TypesDispatcher.dispatch_type_validation(self._metadata_container, self._property_description.type,
                                                        property_value, self._property_description.name,
                                                        variable_path=[self.variable_path])

    def process_list_attribute(self, property_value):
        if type(property_value) == list:
            status = True
            for split_property_value in property_value:
                split_property_status = TypesDispatcher.dispatch_type_validation(self._metadata_container,
                                                                                 self._property_description.type,
                                                                                 split_property_value,
                                                                                 self._property_description.name,
                                                                                 variable_path=[self.variable_path])

                status = status and split_property_status

            return status
        else:
            print "ERROR:: Wrong property %s type" % self._property_description.name
            return False

    def check_nullable_property(self):
        if self._property_description.nullable:
            return True
        else:
            print "ERROR:: Value of field %s is mandatory, cannot be null." % self._property_description.name
            return False


class TypesDispatcher:
    @staticmethod
    def dispatch_type_validation(metadata_container, property_type, property_value, property_name, variable_path=None):
        if property_type in metadata_container.entities.keys():
            # entities are validated as separated resource
            return True

        property_type_category = metadata_container.types.get_type_category(property_type)
        try:
            validation_class = {ValueTypesCategory.COMPLEX_TYPE: ComplexTypeValidator,
                                ValueTypesCategory.ENUM_TYPE: EnumTypeValidator,
                                ValueTypesCategory.TYPE_DEFINITION: TypesDefinitionValidator,
                                ValueTypesCategory.PRIMITIVE_TYPE: PrimitiveTypeValidator}[property_type_category]

            return validation_class(variable_path, metadata_container).validate(property_type, property_value)
        except KeyError:
            print "ERROR:: Unknown type %s for field %s" % (property_type, property_name)
            return False


class TypesDefinitionValidator(BaseTypeValidator):
    def validate(self, property_type, property_value):
        """
        :type property_type: str
        :param property_type: property type name
        :param property_value: property value
        :return: validation status
        :rtype: bool
        """
        try:
            type_definition_description = self._metadata_container.types[property_type]
        except KeyError:
            print "ERROR:: Unknown type %s " % property_type
            return False

        if type_definition_description.base_type:
            return TypesDispatcher.dispatch_type_validation(self._metadata_container,
                                                            type_definition_description.base_type, property_value,
                                                            self.variable_path, variable_path=[self.variable_path])
        else:
            print "ERROR:: Unable to find base type definition of type %s" % property_type
            return False


class ComplexTypeValidator(BaseTypeValidator):
    def validate(self, property_type, property_value):
        try:
            complex_type_description = self._metadata_container.types[property_type]
        except KeyError:
            print "ERROR:: Unknown type " + str(property_type)
            return False

        status = True

        for property_description in complex_type_description.properties:
            status = status and self.validate_property(property_description, property_value)

        return status

    def validate_property(self, property_description, complex_type_body):
        status = True

        validator = TypeValidator(property_description, self._metadata_container, variable_path=[self.variable_path])

        try:
            status = validator.verify_property(complex_type_body[property_description.name])
        except KeyError:
            if property_description.required:
                status = False
                print "ERROR:: Field %s is required." % property_description.name
        except Exception:
            # TODO: add better logging
            import traceback
            print "DEBUG:: %s " % traceback.format_exc()
            status = False
            print "ERROR:: During field " + str(property_description.name) + " validation unkown error occured."

        return status
