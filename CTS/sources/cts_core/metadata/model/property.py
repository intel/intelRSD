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
from cts_framework.helpers.vars.env_consts import Constants

from copy import deepcopy, copy

from cts_core.commons.error import cts_error, cts_warning
from cts_core.metadata.comparator import Comparator
from cts_core.metadata.model.type_compare_mixin import TypeCompareMixin
from cts_core.metadata.commons import Commons
from cts_core.metadata.model.metadata_model import MetadataModel
from cts_core.validation.validation_status import ValidationStatus

NAME = "name"
TYPE = "type"
NULLABLE = "nullable"
COLLECTION = "Collection("
ALLOWABLE = "@Redfish.AllowableValues"


class PatchStatus:
    NONTRIVIAL = "[Nontrivial]"
    NOT_DEFINED = "[Undefined]"
    NOT_PATCHABLE = "[ReadOnly]"
    PATCHABLE = "[ReadWrite]"
    WRITE_ONLY = "[Write]"


class Property(MetadataModel, TypeCompareMixin):
    def __init__(self, metadata_container, namespace_name, property_soup, qualifiers=None):
        """
        :type metadata_container: cts_core.metadata.metadata_container.MetadataContainer
        :type namespace_name: str
        :type property_soup: bs4.element.Tag
        """
        MetadataModel.__init__(self, metadata_container, property_soup, qualifiers)

        self.name = property_soup[NAME]
        property_type = property_soup[TYPE]
        self.is_collection = property_type.startswith(COLLECTION) and property_type.endswith(")")
        property_type = property_type[len(COLLECTION):-1] if self.is_collection else property_type
        self.type = Commons.parse_type_name(property_type, namespace_name)
        self.nullable = property_soup.get(NULLABLE, "true").lower() == "true"

    def __str__(self):
        return self.name

    @property
    def annotations(self):
        """
        :rtype: dict[str, cts_core.metadata.model.annotation.Annotation]
        """
        try:
            annotations = self.metadata_container.get_type_definition(self.type).annotations.copy()
            annotations.update(self._annotations)
        except KeyError:
            annotations = self._annotations

        return annotations

    @property
    def is_required(self):
        """
        :rtype: bool
        """
        return "Redfish.Required" in self.annotations

    @property
    def patch_status(self):
        """
        :rtype: bool
        """
        try:
            if any(c in self.annotations for c in [Constants.INTEL_RACKSCALE_NONTRIVIAL,
                                                   Constants.INTEL_RACKSCALE_EXTENSIONS_NONTRIVIAL]):
                return PatchStatus.NONTRIVIAL
            elif self.annotations["OData.Permissions"].value == "OData.Permission/ReadWrite":
                return PatchStatus.PATCHABLE
            elif self.annotations["OData.Permissions"].value == "OData.Permission/Read":
                return PatchStatus.NOT_PATCHABLE
            elif self.annotations["OData.Permissions"].value == "OData.Permission/Write":
                return PatchStatus.WRITE_ONLY
            else:
                cts_error("Metadata issue: Incorrect enum value `{value}` for OData.Permissions; Property `{name}`",
                          name=self.name, value=self.annotations["OData.Permissions"].value)
                return PatchStatus.NOT_DEFINED

        except KeyError:
            return PatchStatus.NOT_DEFINED

    def get_allowables(self, resource):
        try:
            allowable_values_property = self.name + ALLOWABLE
            allowable_values = resource[allowable_values_property]
            return allowable_values
        except KeyError:
            return []

    def validate(self, resource, path, annotations=None):
        """
        :type annotations: dict
        :type resource: dict
        :type path: str
        :rtype: str
        """
        if self.metadata_container.to_be_ignored(self.type):
            return ValidationStatus.PASSED

        resource_path = "->".join([path, self.name])
        try:
            property_value = resource[self.name]
        except KeyError:
            if self.is_required:
                cts_error("Required property {value_path} not present in the resource", value_path=resource_path)
                return ValidationStatus.FAILED
            else:
                print "DEBUG::Optional property {value_path} is not present in the resource".format(
                    value_path=resource_path)
                return ValidationStatus.PASSED
        except TypeError:
            cts_error("Property {path} is expected to be json object", path=path)
            return ValidationStatus.FAILED

        if property_value is None:
            if self.nullable:
                return ValidationStatus.PASSED
            else:
                cts_error("Property {path} cannot be null",  path=resource_path)
                return ValidationStatus.FAILED

        try:
            if self.is_collection:
                status = self._validate_collection(property_value, resource_path)
            else:
                status = self.metadata_container.types[self.type].validate(property_value, resource_path,
                                                                           self.annotations)

                allowable_values = self.get_allowables(resource)

                # is property value on "allowable values" list?
                if allowable_values and (not property_value in allowable_values):
                    cts_warning("Property {resource_path} has value '{property_value}' " +
                                "that is not on the list of allowable values: {allowable_values_property}",
                                property_value=property_value,
                                allowable_values_property=allowable_values,
                                resource_path=resource_path)
                    status = ValidationStatus.join_statuses(status, ValidationStatus.PASSED_WITH_WARNINGS)

                # allowable list should contain only legal values
                for value in allowable_values:
                    allowable_value_status = \
                        self.metadata_container.types[self.type].validate(value, resource_path+ALLOWABLE,
                                                                          self.annotations)
                    status = ValidationStatus.join_statuses(status, allowable_value_status)


            return status
        except KeyError as err:
            cts_error("Property {value_path} is defined to reference unknown type '{property_type}'. Error: {key}",
                      value_path=resource_path, property_type=self.type, key=err)
            return ValidationStatus.FAILED

    def generate_values(self, property_annotations=None):
        """
        :type annotations: list [cts_core.metadata.model.annotation.Annotation]
        :rtype: list
        """
        # merge annotations from type definition with annotations from property definition
        annotations = self.annotations.copy()
        annotations.update(property_annotations)
        return self.metadata_container.get_type_definition(self.type).generate_values(annotations)

    def _validate_collection(self, property_value, resource_path):
        value_is_collection = True
        status = ValidationStatus.PASSED
        if isinstance(property_value, list):
            try:
                for path_element, resource_element in enumerate(property_value):
                    status = ValidationStatus.join_statuses(self.metadata_container.types[self.type].validate(
                        resource_element, "%s->%s" % (resource_path, path_element), self.annotations), status)
            except TypeError:
                value_is_collection = False
        else:
            value_is_collection = False

        if not value_is_collection:
            cts_error("Property {resource_path} is expected to be collection, but it is not",
                      resource_path=resource_path)
            status = ValidationStatus.FAILED

        return status

    def compare_type(self, other, level):
        """
        :type other: cts_core.metadata.model.property.Property
        :rtype: int
        """
        cmp = Comparator(level)
        if self.type != other.type or self.is_collection != other.is_collection:
            fmt = "Type: {type}"
            fmt_collection = "Type: Collection({type})"
            cmp.message_left((fmt_collection if self.is_collection else fmt).format(type=self.type))
            cmp.message_right((fmt_collection if other.is_collection else fmt).format(type=other.type))
            cmp.set_not_equal()

        return cmp

    def compare_nullable(self, other, level):
        """
        :type other: cts_core.metadata.model.property.Property
        :rtype: int
        """
        cmp = Comparator(level)
        if self.nullable != other.nullable:
            cmp.message_left("Nullable" if self.nullable else "Not Nullable")
            cmp.message_right("Nullable" if other.nullable else "Not Nullable")
            cmp.set_not_equal()

        return cmp

    def compare_is_required(self, other, level):
        """
        :type other: cts_core.metadata.model.property.Property
        :rtype: int
        """

        cmp = Comparator(level)
        if self.is_required != other.is_required:
            cmp.message_left("Required" if self.is_required else "Optional")
            cmp.message_right("Required" if other.is_required else "Optional")
            cmp.set_not_equal()

        return cmp

    def compare_patch_status(self, other, level):
        """
        :type other: cts_core.metadata.model.property.Property
        :rtype: int
        """
        cmp=Comparator(level)
        if self.patch_status != other.patch_status:
            cmp.message_left(self.patch_status)
            cmp.message_right(other.patch_status)
            cmp.set_not_equal()

        return cmp

    def compare(self, other, level):
        """
        :type other: cts_core.metadata.model.property.Property
        :rtype: int
        """
        cmp = Comparator(level)
        cmp.message_both("Property: {name}".format(name=self.name))
        return cmp.merge_result(
            self.compare_type(other, level+1),
            self.compare_nullable(other, level+1),
            self.compare_is_required(other, level+1),
            self.compare_patch_status(other, level+1),
            self.compare_annotations(other, level+1)
        )
