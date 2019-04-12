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

from cts_core.commons.error import cts_warning
from cts_core.metadata.comparator import Comparator
from cts_core.metadata.model.type_compare_mixin import TypeCompareMixin

from cts_core.commons.error import cts_error
from cts_core.commons.json_helpers import sanitize_odata_type
from cts_core.commons.json_helpers import get_odata_type, is_special_property
from cts_core.commons.text_helpers import try_suggest_other_property_names
from cts_core.metadata.commons import Commons
from cts_core.metadata.model.metadata_model import MetadataModel
from cts_core.metadata.model.metadata_types.dynamic_properties import match_dynamic_property
from cts_core.metadata.model.navigation_property import NavigationProperty
from cts_core.metadata.model.property import Property
from cts_core.validation.validation_status import ValidationStatus

PROPERTY = "property"
NAVIGATION_PROPERTY = "navigationproperty"
BASE_TYPE = "basetype"
NAME = "name"
ABSTRACT = "abstract"


class Entity(MetadataModel, TypeCompareMixin):
    def __init__(self, metadata_container, namespace_name, entity_soup, qualifiers=None):
        """
        :type metadata_container: cts_core.metadata.metadata_container.MetadataContainer
        :type namespace_name: str
        :type entity_soup: bs4.element.Tag
        """
        MetadataModel.__init__(self, metadata_container, entity_soup, qualifiers)

        self.name = Commons.parse_type_name(entity_soup[NAME], namespace_name)
        try:
            self.base_type = Commons.parse_type_name(entity_soup[BASE_TYPE], namespace_name)
        except KeyError:
            # does not have base type
            self.base_type = None
        self.abstract = entity_soup.get(ABSTRACT, False)

        properties = [Property(self.metadata_container, namespace_name, property_soup, qualifiers) for property_soup in
                      entity_soup.find_all(PROPERTY, recursive=False)]
        self._properties = {prop.name: prop for prop in properties}

        navigation_properties = [NavigationProperty(self.metadata_container, namespace_name, navigation_property_soup, qualifiers) for
                                 navigation_property_soup in entity_soup.find_all(NAVIGATION_PROPERTY, recursive=False)]
        self._navigation_properties = {navigation_property.name: navigation_property for navigation_property in
                                       navigation_properties}

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

    @property
    def properties(self):
        """
        :rtype: dict[str, cts_core.metadata.model.property.Property]
        """
        try:
            properties = self.metadata_container.entities[self.base_type].properties.copy()
            properties.update(self._properties)
        except KeyError:
            properties = self._properties

        return properties

    @property
    def navigation_properties(self):
        """
        :rtype: dict[str, cts_core.metadata.model.navigation_property.NavigationProperty]
        """
        try:
            navigation_properties = self.metadata_container.entities[self.base_type].navigation_properties.copy()
            navigation_properties.update(self._navigation_properties)
        except KeyError:
            navigation_properties = self._navigation_properties

        return navigation_properties

    @property
    def all_properties(self):
        return self.properties.values() + self.navigation_properties.values()

    @property
    def annotations(self):
        """
        :rtype: dict[str, cts_core.metadata.model.annotation.Annotation]
        """
        try:
            annotations = self.metadata_container.entities[self.base_type].annotations.copy()
            annotations.update(self._annotations)
        except KeyError:
            annotations = self._annotations

        return annotations

    def is_compatible(self, other):
        """
        Returns True if 'other' is this entity itself or its child type

        :type other: str
        :rtype: bool
        """
        if other is None:
            return False

        other = sanitize_odata_type(other)

        if self.name == other:
            return True

        try:
            return self.is_compatible(self.metadata_container.entities[other].base_type)
        except (KeyError, AttributeError):
            return False

    def validate(self, resource, resource_path, annotations=None, odata_type=None):
        """
        :type annotations: dict
        :type resource: dict
        :type resource_path: str
        :type odata_type: str
        :rtype: str
        """
        if not odata_type:
            try:
                odata_type = get_odata_type(resource)
            except KeyError:
                odata_type = None

        if not odata_type:
            cts_warning("Resource {odata_id} did not report odata.type property", odata_id=resource_path)
            return ValidationStatus.PASSED_WITH_WARNINGS
        else:
            mapped_odata_type = self.metadata_container.map_type(odata_type)
            if self.metadata_container.to_be_ignored(odata_type, mapped_odata_type):
                return ValidationStatus.PASSED

        types_consistency_validation_status, entity_type = self._validate_types_consistency(resource,
                                                                                            resource_path,
                                                                                            odata_type)

        if entity_type == self.name:
            resource_validation_status = self._validate_container(resource, resource_path)
        else:
            try:
                resource_validation_status = self.metadata_container.entities[entity_type]._validate_container(resource,
                                                                                                               resource_path)
            except KeyError:
                resource_validation_status = ValidationStatus.FAILED
            except Exception as error:
                cts_error("{id:id} Unexpected error: {error:exception}", id==resource_path, error=error)
                resource_validation_status = ValidationStatus.FAILED

        return ValidationStatus.join_statuses(types_consistency_validation_status, resource_validation_status)

    def _validate_container(self, container, path):
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
        expected_properties = self.properties.keys() + self.navigation_properties.keys()
        present_properties = container.keys()
        unused_properties = set(expected_properties) - set(present_properties)

        for property_name in container.keys():
            if property_name not in expected_properties and not is_special_property(property_name):
                status = ValidationStatus.join_statuses(status,
                                                        self._validate_additional_property(property_name,
                                                                                           container[property_name],
                                                                                           path,
                                                                                           unused_properties))
        return status

    def _validate_additional_property(self, property_name, property_value, resource_path, unused_properties):
        if self.allow_additional_properties:
            print "DEBUG::Additional property {odata_id}->{property_name}" \
                .format(odata_id=resource_path, property_name=property_name)
            try_suggest_other_property_names(name=property_name, candidates=unused_properties)

            odata_type = match_dynamic_property(property_name, self)
            if odata_type is None:
                try:
                    odata_type = get_odata_type(property_value)
                except KeyError:
                    if isinstance(property_value, dict):
                        cts_error("Property {odata_id:id}->{key} is of complex type without @odata_type",
                                  odata_id=resource_path, key=property_name)
                        return ValidationStatus.FAILED
                    else:
                        return ValidationStatus.PASSED
            mapped_odata_type = self.metadata_container.map_type(odata_type)
            if not self.metadata_container.to_be_ignored(odata_type, mapped_odata_type):
                return self.metadata_container.types[mapped_odata_type].validate(
                    property_value,
                    resource_path + '->' + property_name)
        else:
            cts_error("Property {odata_id:id}->{property_name} is present, but not defined in metadata. "
                      "Entity {entity_name} does not allow additional properties",
                      odata_id=resource_path, property_name=property_name, entity_name=self.name)
            try_suggest_other_property_names(name=property_name, candidates=unused_properties)

            status = ValidationStatus.FAILED

    def _validate_types_consistency(self, resource, resource_path, odata_type=None):
        """
        :type resource: dict
        :type resource_path: str
        :type odata_type: str
        :return: validation_status, type that will be validated
        :rtype: str, str
        """
        if not odata_type:
            try:
                odata_type = get_odata_type(resource)
            except KeyError:
                odata_type = None

        if not odata_type:
            cts_warning("Missing odata.type property in {odata_id:id}", odata_id=resource_path)
            return ValidationStatus.PASSED_WITH_WARNINGS, self.name
        else:
            reported_odata_type = self.metadata_container.map_type(odata_type)
            if self.metadata_container.to_be_ignored(odata_type, reported_odata_type):
                return ValidationStatus.PASSED, reported_odata_type

        try:
            if self.is_compatible(reported_odata_type):
                msg = ("DEBUG::Resource {odata_id} reported entity type {reported_odata_type}, " +
                       "expected type {expected_entity_type}, types are consistent").format(
                    odata_id=resource_path, reported_odata_type=reported_odata_type, expected_entity_type=self.name)
                print msg

                return ValidationStatus.PASSED, reported_odata_type
            else:
                cts_error("Resource {odata_id:id} reported entity type {reported_odata_type}, " +
                          "expected type {expected_entity_type}, types are not consistent",
                          odata_id=resource_path,
                          reported_odata_type=reported_odata_type,
                          expected_entity_type=self.name)

                return ValidationStatus.FAILED, reported_odata_type
        except KeyError:
            cts_error("Resource {odata_id:id} reported unknown entity type {reported_odata_type}," +
                      "will process validation with expected entity type {expected_entity_type}",
                      odata_id=resource_path,
                      reported_odata_type=reported_odata_type,
                      expected_entity_type=self.name)

            return ValidationStatus.FAILED, reported_odata_type

    def compare(self, other):
        """
        :type other: cts_core.metadata.model.entity.Entity
        :rtype: int
        """
        cmp = Comparator(level=0)
        cmp.message_both("Entity: {entity}".format(entity=self.name))
        cmp.merge_result(
            self.compare_base_type(other, level=1),
            self.compare_properties(other, level=1),
            self.compare_annotations(other, level=1),
            self.compare_dynamic_property_patterns(other, level=1))
        cmp.separator()
        return cmp

    def compare_dynamic_property_patterns(self, other, level):
        """
        :type other: cts_core.metadata.model.entity.Entity
        :type cmp: cts_core.metadata.comparator.Comparator
        :rtype: int
        """
        return Comparator(level)

