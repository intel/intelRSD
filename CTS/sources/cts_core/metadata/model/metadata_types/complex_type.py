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

from copy import deepcopy

from cts_core.commons.error import cts_error
from cts_core.metadata.comparator import Comparator
from cts_core.metadata.model.type_compare_mixin import TypeCompareMixin

from cts_core.commons.text_helpers import try_suggest_other_property_names
from cts_core.metadata.model.metadata_types.dynamic_properties import match_dynamic_property
from cts_core.metadata.model.metadata_types.metadata_type import MetadataType
from cts_core.metadata.model.metadata_types.metadata_type_categories import MetadataTypeCategories
from cts_core.metadata.model.navigation_property import NavigationProperty
from cts_core.metadata.model.property import Property
from cts_core.validation.validation_status import ValidationStatus
from cts_core.commons.json_helpers import get_odata_type

PROPERTY = "property"
NAVIGATION_PROPERTY = "navigationproperty"
BASE_TYPE = "basetype"


class ComplexType(MetadataType, TypeCompareMixin):
    def __init__(self, metadata_container, namespace_name, type_soup, qualifiers=None):
        """
        :type metadata_container: cts_core.metadata.metadata_container.MetadataContainer
        :type namespace_name: str
        :type type_soup: bs4.element.Tag
        """
        MetadataType.__init__(self, metadata_container, namespace_name, type_soup, qualifiers)

        self.type_category = MetadataTypeCategories.COMPLEX_TYPE
        self.base_type = type_soup.get(BASE_TYPE, None)

        properties = [Property(self.metadata_container, namespace_name, property_soup, qualifiers) for property_soup in
                      type_soup.find_all(PROPERTY, recursive=False)]
        self._properties = {prop.name: prop for prop in properties}

        navigation_properties = [NavigationProperty(self.metadata_container, namespace_name, navigation_property_soup, qualifiers) for
                                 navigation_property_soup in type_soup.find_all(NAVIGATION_PROPERTY, recursive=False)]

        self._navigation_properties = {navigation_property.name: navigation_property for navigation_property in
                                       navigation_properties}

    @property
    def navigation_properties(self):
        """
        :rtype: dict[str, cts_core.metadata.model.navigation_property.NavigationProperty]
        """
        try:
            navigation_properties = self.metadata_container.types[self.base_type].navigation_properties.copy()
            navigation_properties.update(self._navigation_properties)
        except (KeyError, AttributeError):
            navigation_properties = self._navigation_properties

        return navigation_properties

    @property
    def properties(self):
        """
        :rtype: dict[str, cts_core.metadata.model.property.Property]
        """
        try:
            properties = self.metadata_container.types[self.base_type].properties.copy()
            properties.update(self._properties)
        except KeyError:
            properties = self._properties

        return properties

    @property
    def all_properties(self):
        return self.properties.values() + self.navigation_properties.values()

    @property
    def allow_additional_properties(self):
        """
        :rtype: bool
        """
        try:
            return self.annotations["OData.AdditionalProperties"].value.lower() == "true"
        except KeyError:
            return True

    @property
    def dynamic_property_patterns(self):
        try:
            return self.annotations["Redfish.DynamicPropertyPatterns"].dynamic_property_patterns
        except KeyError:
            return []

    def validate(self, resource, resource_path, annotations=None):
        """
        :type annotations: dict
        :type resource: dict
        :type resource_path: str
        :rtype: str
        """
        try:
            odata_type = self.metadata_container.map_type(get_odata_type(resource))
            mapped_odata_type = self.metadata_container.map_type(odata_type)
            if self.metadata_container.to_be_ignored(odata_type, mapped_odata_type):
                return ValidationStatus.PASSED
        except KeyError:
            pass #this is legal that complex type does not have odata.type specified

        types_consistency_validation_status, complex_type_name = self.validate_types_consistency(resource, resource_path)
        if self.metadata_container.to_be_ignored(complex_type_name):
            return ValidationStatus.PASSED

        if complex_type_name == self.name:
            resource_validation_status = self.validate_properties(resource, resource_path)
        else:
            try:
                resource_validation_status = self.metadata_container.types[complex_type_name].validate(resource, resource_path)
            except KeyError:
                resource_validation_status = ValidationStatus.FAILED
            except Exception as err:
                import traceback
                cts_error("{path} - exception {err:exceptions}: {stack:stacktrace}",
                          path=resource_path, err=err, stack=traceback.format_exc())
                resource_validation_status = ValidationStatus.FAILED

        return ValidationStatus.join_statuses(types_consistency_validation_status, resource_validation_status)

    def validate_properties(self, container, path):
        """
        :type container: dict
        :type path: str
        :rtype: str
        """
        additional_items_validation = self._validate_additional_properties(container, path)

        fields_validation = list()

        for resource_property in self.properties.itervalues():
            fields_validation.append(resource_property.validate(container, path))

        return ValidationStatus.join_statuses(additional_items_validation, *fields_validation)

    def _validate_additional_properties(self, container, path):
        """
        :type container: dict
        :type path: str
        :rtype: str
        """
        status = ValidationStatus.PASSED
        if not isinstance(container, dict):
            return status

        expected_properties = self.properties.keys() + self.navigation_properties.keys()
        present_properties = container.keys()
        unused_properties = set(expected_properties) - set(present_properties)

        for property_name in container.keys():
            if property_name not in expected_properties and "#" not in property_name and "@" not in property_name:
                status = ValidationStatus.join_statuses(status,
                                                        self._validate_additional_property(
                                                            property_name,
                                                            container[property_name],
                                                            path,
                                                            unused_properties))
        return status

    def _validate_additional_property(self, property_name, property_value, resource_path, unused_properties):
        if self.allow_additional_properties:
            print "DEBUG::Additional property {resource_path}->{property_name}".format(
                resource_path=resource_path, property_name=property_name)
            try_suggest_other_property_names(name=property_name, candidates=unused_properties)

            odata_type = match_dynamic_property(property_name, self)
            if odata_type is None:
                try:
                    odata_type = get_odata_type(property_value)
                except KeyError:
                    if isinstance(property_value, dict):
                        cts_error("Property {path}->{property_name} is of complex type without @odata_type",
                                  path=resource_path, property_name=property_name)
                        return ValidationStatus.FAILED
                    else:
                        return ValidationStatus.PASSED

            mapped_odata_type = self.metadata_container.map_type(odata_type)

            # assuming all aditional properties are nullable
            if property_value is None:
                return ValidationStatus.PASSED

            if not self.metadata_container.to_be_ignored(odata_type, mapped_odata_type):
                return self.metadata_container.types[odata_type].validate(
                    property_value, resource_path + '->' + property_name)
            else:
                return ValidationStatus.PASSED
        else:
            cts_error("Property {resource_path}->{property_name} is present, but not defined in metadata. " \
                      "Type {type_name} does not allow additional properties",
                      resource_path=resource_path, property_name=property_name, type_name=self.name)
            try_suggest_other_property_names(name=property_name, candidates=unused_properties)
            return ValidationStatus.FAILED

    def validate_types_consistency(self, container, path):
        """
        :type container: dict
        :type path: str
        :return: validation_status, type that will be validated
        :rtype: str, str
        """
        try:
            odata_type = get_odata_type(container)
        except KeyError:
            odata_type = self.name
        reported_odata_type = self.metadata_container.map_type(odata_type)
        if self.metadata_container.to_be_ignored(odata_type, reported_odata_type):
            return ValidationStatus.PASSED, reported_odata_type

        if reported_odata_type == self.name:
            return ValidationStatus.PASSED, self.name

        try:
            if self.is_compatible(reported_odata_type):
                msg = ("DEBUG::Resource {resource_path} reported type {reported_odata_type}, " +
                       "expected type {expected_type}, types are consistent").format(
                    resource_path=path, reported_odata_type=reported_odata_type, expected_type=self.name)
                print msg

                return ValidationStatus.PASSED, reported_odata_type
            else:
                cts_error("Resource {resource_path} reported type {reported_odata_type}, " +
                          "expected type {expected_type}, types are not consistent",
                          resource_path=path, reported_odata_type=reported_odata_type, expected_type=self.name)

                print "MESSAGE::Setting results to ERROR, but will process validation with reported complex type " + \
                      "{reported_odata_type}".format(reported_odata_type=reported_odata_type)

                return ValidationStatus.FAILED, reported_odata_type
        except KeyError:
            cts_error("Resource {resource_path} reported unknown type {reported_odata_type}," +
                      "will process validation with expected entity type {expected_type}",
                      resource_path=path, reported_odata_type=reported_odata_type, expected_type=self.name)

            return ValidationStatus.FAILED, self.name
        except AttributeError:
            cts_error("Resource {resource_path} reported itself as not complex type {reported_type}",
                      resource_path=path, reported_type=reported_odata_type)

            return ValidationStatus.FAILED, self.name

    def is_compatible(self, other):
        """
        Returns True if 'other' is this entity itself or its child type

        :type other: str
        :rtype: bool
        """
        if other is None:
            return False

        if self.name == other:
            return True

        try:
            return self.is_compatible(self.metadata_container.types[other].base_type)
        except KeyError:
            cts_error("Reference to unknown type {type}", type=other)
            return False
        except AttributeError:
            return False

    def compare(self, other):
        """
        :type other: cts_core.metadata.model.metadata_types.complex_type.ComplexType
        :rtype: int
        """
        cmp = Comparator()
        cmp.message_both("Type: {type}".format(type=self.name))
        cmp.merge_result(
            self.compare_base_type(other, level=1),
            self.compare_properties(other, level=1),
            self.compare_annotations(other, level=1),
            self.compare_dynamic_property_patterns(other, level=1))
        cmp.separator()
        return cmp


    def compare_dynamic_property_patterns(self, other, level):
        """
        :type other: cts_core.metadata.model.metadata_types.complex_type.ComplexType
        :type cmp: cts_core.metadata.comparator.Comparator
        :rtype: int
        """
        return Comparator(level)

