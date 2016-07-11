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

import random

from cts_core.discovery.api_caller import ApiCaller
from cts_core.metadata.metadata_enums import ValueTypesCategory
from cts_core.metadata_validation.get.validators.primitive_types_validator import EDM_TYPES_DICTIONARY
from cts_framework.commons.enums import HttpMethods, ReturnCodes


class MetadataPatchValidator:
    def __init__(self, metadata_container, configuration):
        self._metadata_container = metadata_container
        self._api_caller = ApiCaller(configuration)

    def validate(self, discovery_container):
        status = True
        for api_resource in discovery_container.values():
            if api_resource.body is None or api_resource.odata_id is None:
                continue

            if self._metadata_container.entities.compare_entity_types(api_resource.odata_type, "ComputerSystem.1.0.0.ComputerSystem"):
                if not self.validate_computer_system(api_resource):
                    status = False
            elif not self.validate_single_entity(api_resource):
                status = False

        return status

    def validate_single_entity(self, api_resource):
        status = True
        try:
            entity_details = self._metadata_container.entities[api_resource.odata_type]
        except KeyError:
            print "ERROR:: Unable to find definition of entity %s" % api_resource.odata_type
            return False

        for property_description in entity_details.properties + entity_details.navigation_properties:
            status = self.process_property(property_description, api_resource, list()) and status

        return status

    def process_property(self, property_description, api_resource, variable_path):
        status = True
        if property_description.is_collection:
            # setting on collections elements is not allowed
            return status

        variable_path = variable_path + [property_description.name]
        property_type_category = self._metadata_container.types.get_type_category(property_description.type)

        if property_type_category == ValueTypesCategory.COMPLEX_TYPE:
            property_type_details = self._metadata_container.types[property_description.type]

            sub_properties_list = property_type_details.properties + property_type_details.navigation_properties

            for sub_property_description in sub_properties_list:
                status = self.process_property(sub_property_description, api_resource, variable_path) and status
        else:
            if [annotation for annotation in property_description.annotations if
                    annotation.term == "OData.Permissions" and annotation.value == "OData.Permission/ReadWrite"]:
                status = self.validate_patch_request(property_description, api_resource, variable_path) and status

        return status

    def validate_patch_request(self, property_description, api_resource, variable_path):
        validation_status = True
        current_value = self.get_value_from_body_path(variable_path, api_resource.body)
        if current_value is None:
            return validation_status

        new_value = self.get_value(property_description, current_value)
        if new_value is None:
            return validation_status

        data = self.create_data(variable_path, new_value)

        status, status_code, response_body, headers = self._api_caller.\
            perform_call(api_resource.odata_id, http_method=HttpMethods.PATCH, acceptable_return_codes=[ReturnCodes.OK],
                         payload=data)
        if not status:
            print "ERROR:: Changing field %s value from %s to %s failed with wrong status code" % \
                  ("->".join(variable_path), current_value, new_value)
            validation_status = False

        status, status_code, response_body, headers = self._api_caller.get_resource(api_resource.odata_id)

        if new_value != self.get_value_from_body_path(variable_path, response_body):
            print "ERROR:: Changing field %s value failed - value not changed." % "->".join(variable_path)
            validation_status = False

        # return to previous value
        data = self.create_data(variable_path, self.get_value_from_body_path(variable_path, response_body))

        print "DEBUG::setting to previous value."
        status, status_code, response_body, headers = self._api_caller.perform_call(
            api_resource.odata_id, http_method=HttpMethods.PATCH, acceptable_return_codes=[ReturnCodes.OK], payload=data)

        if status:
            print "WARNING::Setting previous value request failed."

        return validation_status

    def get_value(self, property_description, current_value):
        if self._metadata_container.types.get_type_category(property_description.type) == ValueTypesCategory.ENUM_TYPE:
            get_value_method = self.get_enum_value
        else:
            get_value_method = self.get_primitive_type_value

        return get_value_method(property_description, current_value)

    def get_enum_value(self, property_description, current_value):
        try:
            enum_details = self._metadata_container.types[property_description.type]
        except KeyError:
            try:
                enum_details = self._metadata_container.types[property_description.name]
            except KeyError:
                return None

        return random.choice([member.name for member in enum_details.members if member.name != current_value])

    def get_primitive_type_value(self, property_description, current_value):
        base_type = self._metadata_container.types.get_base_type(property_description.type)

        try:
            type_helper_class = EDM_TYPES_DICTIONARY[base_type]
        except KeyError:
            print "WARNING:: unknown base type %s for type %s" % (base_type, property_description.type)
            return None

        return type_helper_class().generate_value(current_value)

    def create_data(self, variable_path, value):
        data = value
        for path_element in variable_path[::-1]:
            data = {path_element: data}

        return data

    def get_value_from_body_path(self, variable_path, body):
        value = body
        try:
            for path_element in variable_path:
                value = value[path_element]
        except KeyError:
            return None
        except Exception:
            return None

        return value

    def validate_computer_system(self, api_resource):
        """
        function to specially handle computer system case when patching two values is required.
        """
        current_boot_source_override_target = self.get_value_from_body_path(["Boot", "BootSourceOverrideTarget"],
                                                                            api_resource.body)
        current_boot_source_override_enabled = self.get_value_from_body_path(["Boot", "BootSourceOverrideEnabled"],
                                                                             api_resource.body)

        if not current_boot_source_override_enabled or not current_boot_source_override_target:
            return True

        try:
            boot_source_override_target_property = self._metadata_container.types["ComputerSystem.1.0.0.BootSource"]
            boot_source_override_enabled_property = self._metadata_container.types["ComputerSystem.1.0.0.BootSourceOverrideEnabled"]
        except KeyError:
            print "WARNING::Metadata does not contain required information about properties BootSource and BootSourceOverrideEnabled"
            return False

        new_boot_source_override_target = random.choice(["Hdd", "Pxe"])

        new_boot_source_override_enabled = self.get_enum_value(boot_source_override_enabled_property, current_boot_source_override_enabled)

        if not new_boot_source_override_enabled or not new_boot_source_override_target:
            return True

        payload = dict(
            Boot=dict(
                BootSourceOverrideTarget=new_boot_source_override_target,
                BootSourceOverrideEnabled=new_boot_source_override_enabled
            )
        )

        status, status_code, response_body, headers = self._api_caller.\
            perform_call(api_resource.odata_id, http_method=HttpMethods.PATCH, acceptable_return_codes=[ReturnCodes.OK],
                         payload=payload)

        if not status:
            print "ERROR::patching boot options failed with status code %s" % status_code
            return False

        return True
