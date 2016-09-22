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

import traceback

import itertools

from cts_core.discovery.api_caller import ApiCaller
from cts_core.metadata.metadata_enums import ValueTypesCategory
from cts_core.metadata_validation.get.validators.primitive_types_validator import EDM_TYPES_DICTIONARY
from cts_framework.commons.enums import HttpMethods, ReturnCodes, RequestStatus
from cts_framework.tests_helpers import ResultStatus


class MetadataPatchValidator:
    def __init__(self, metadata_container, configuration):
        self._metadata_container = metadata_container
        self._api_caller = ApiCaller(configuration)

    def validate(self, discovery_container):
        status = ResultStatus.PASSED
        for api_resource in discovery_container.values():
            if api_resource.body is None or api_resource.odata_id is None:
                continue
            try:
                if self._metadata_container.entities.compare_entity_types(api_resource.odata_type,
                                                                          "ComputerSystem.1.0.0.ComputerSystem"):
                    status = ResultStatus.join_statuses(self.validate_computer_system(api_resource), status)
                else:
                    status = ResultStatus.join_statuses(self.validate_single_entity(api_resource), status)
            except Exception as err:
                print "ERROR::unhandled exception %s while validating resource %s" % (err.message, api_resource.odata_id)
                print "DEBUG:: traceback:\n%s" % str(traceback.format_exc())
                status = ResultStatus.join_statuses(status, ResultStatus.FAILED)

        return status

    def validate_single_entity(self, api_resource):
        status = ResultStatus.PASSED
        try:
            entity_details = self._metadata_container.entities[api_resource.odata_type]
        except KeyError:
            print "ERROR:: Unable to find definition of entity %s" % api_resource.odata_type
            return ResultStatus.FAILED

        for property_description in entity_details.properties:
            status = ResultStatus.join_statuses(self.process_property(property_description, api_resource, list()), status)

        return status

    def process_property(self, property_description, api_resource, variable_path):
        status = ResultStatus.PASSED

        variable_path = variable_path + [property_description.name]

        if property_description.is_collection:
            variable_path = variable_path + [0]

        property_type_category = self._metadata_container.types.get_type_category(property_description.type)

        if property_type_category == ValueTypesCategory.COMPLEX_TYPE:
            property_type_details = self._metadata_container.types[property_description.type]

            for sub_property_description in property_type_details.properties:
                status = ResultStatus.join_statuses(self.process_property(sub_property_description, api_resource, variable_path),
                                                    status)
        elif [annotation for annotation in property_description.annotations if
              annotation.term == "OData.Permissions" and annotation.value == "OData.Permission/Read"]:
            # property is marked as not patchable
            status = ResultStatus.join_statuses(self.validate_incorrect_patch_request(property_description, api_resource,
                                                                                      variable_path),
                                                status)
        elif [annotation for annotation in property_description.annotations if
              annotation.term == "OData.Permissions" and annotation.value == "OData.Permission/ReadWrite"]:
            # property is marked as patchable
            status = ResultStatus.join_statuses(self.validate_patch_request(property_description, api_resource, variable_path),
                                                status)

        return status

    def validate_patch_request(self, property_description, api_resource, variable_path):
        try:
            current_value = self.get_value_from_body_path(variable_path, api_resource.body)
        except KeyError:
            return ResultStatus.PASSED

        new_values = self.get_value(property_description, current_value)

        if not new_values:
            return ResultStatus.PASSED

        if not isinstance(new_values, list):
            new_values = [new_values]

        overall_property_status = ResultStatus.PASSED
        for new_value in new_values:

            print "RAW::Patching %s. Trying to set %s to %s\n" % (api_resource.odata_id, "->".join(
                [str(path) for path in variable_path]), str(new_value))

            data = self.create_data(variable_path, new_value)

            status, status_code, response_body, headers = self._api_caller.perform_call(
                api_resource.odata_id, http_method=HttpMethods.PATCH, payload=data,
                acceptable_return_codes=[ReturnCodes.OK,
                                         ReturnCodes.NO_CONTENT,
                                         ReturnCodes.METHOD_NOT_ALLOWED,
                                         ReturnCodes.UNPROCESSABLE_ENTITY,
                                         ReturnCodes.NOT_IMPLEMENTED])

            if status != RequestStatus.SUCCESS:
                print "WARNING:: Changing field %s value from %s to %s failed with wrong status code %s" % \
                      ("->".join([str(path) for path in variable_path]), current_value, new_value, str(status_code))
                # change FAIL to WARNING
                overall_property_status = ResultStatus.join_statuses(overall_property_status, ResultStatus.PASSED_WITH_WARNING)

            if status_code not in [ReturnCodes.NO_CONTENT]:
                print "WARNING::entity %s did not proceed patch request - responded with status code %s" % (api_resource.odata_id,
                                                                                                            status_code)
                patch_applied = False
            else:
                patch_applied = True

            status, status_code, response_body, headers = self._api_caller.get_resource(api_resource.odata_id)

            try:
                actual_value = self.get_value_from_body_path(variable_path, response_body)
            except KeyError:
                print "WARNING::after patching value %s is not present in body\n" % "->".join(
                    [str(path) for path in variable_path])
                overall_property_status = ResultStatus.join_statuses(overall_property_status, ResultStatus.PASSED_WITH_WARNING)

            if new_value != actual_value and patch_applied:
                print "WARNING::Changing field %s value failed - value [%s] other than expected [%s]\n" % (
                    "->".join([str(path) for path in variable_path]), str(actual_value), str(new_value))
                overall_property_status = ResultStatus.join_statuses(overall_property_status, ResultStatus.PASSED_WITH_WARNING)
                continue
            elif current_value != actual_value and not patch_applied:
                print "WARNING::Patch not applied but value %s changed from [%s] to [%s]\n" % (
                    "->".join([str(path) for path in variable_path]), str(current_value), str(actual_value))
                overall_property_status = ResultStatus.join_statuses(overall_property_status, ResultStatus.PASSED_WITH_WARNING)
                continue
            elif not patch_applied:
                print "WARNING::Patch has not been applied\n"
                overall_property_status = ResultStatus.join_statuses(overall_property_status, ResultStatus.PASSED_WITH_WARNING)
                continue

            # return to previous value
            try:
                data = self.create_data(variable_path, current_value)
            except KeyError:
                continue

            print "DEBUG::setting to previous value."
            status, status_code, response_body, headers = self._api_caller.perform_call(
                api_resource.odata_id, http_method=HttpMethods.PATCH, payload=data,
                acceptable_return_codes=[ReturnCodes.OK, ReturnCodes.NO_CONTENT])

            if status != RequestStatus.SUCCESS:
                print "WARNING::Setting previous value request failed. status code %s" % status_code
                # change FAIL to WARNING
                overall_property_status = ResultStatus.join_statuses(overall_property_status, ResultStatus.PASSED_WITH_WARNING)

        return overall_property_status

    def validate_incorrect_patch_request(self, property_description, api_resource, variable_path):
        overall_property_status = ResultStatus.PASSED
        try:
            current_value = self.get_value_from_body_path(variable_path, api_resource.body)
        except KeyError:
            return ResultStatus.PASSED

        try:
            new_value = self.get_value(property_description, current_value)[0]
        except (IndexError, KeyError):
            return ResultStatus.PASSED

        if not new_value:
            return ResultStatus.PASSED

        data = self.create_data(variable_path, new_value)

        print "RAW::Patching %s. Trying to set %s to %s\n" % (api_resource.odata_id, "->".join(
            [str(path) for path in variable_path]), str(new_value))

        status, status_code, response_body, headers = self._api_caller.perform_call(
            api_resource.odata_id, http_method=HttpMethods.PATCH, payload=data,
            acceptable_return_codes=[ReturnCodes.OK,
                                     ReturnCodes.METHOD_NOT_ALLOWED,
                                     ReturnCodes.NOT_IMPLEMENTED,
                                     ReturnCodes.FORBIDDEN])

        # 403 - Forbidden was added to acceptable_return_codes to prevent VTS from stopping PATCH testing when it
        # occurs
        if status != RequestStatus.SUCCESS or ReturnCodes.FORBIDDEN == status_code:
            print "WARNING::patching property %s value from %s to %s failed with wrong status code %s" % \
                  ("->".join([str(path) for path in variable_path]), current_value, new_value, str(status_code))
            # change FAIL to WARNING
            overall_property_status = ResultStatus.join_statuses(overall_property_status, ResultStatus.PASSED_WITH_WARNING)

        status, status_code, response_body, headers = self._api_caller.get_resource(api_resource.odata_id)
        try:
            value_post = self.get_value_from_body_path(variable_path, response_body)
            if current_value != value_post:
                print "WARNING::Read-only property %s has changed from %s to %s. Not expected." % \
                      ("->".join([str(path) for path in variable_path]),
                       current_value, value_post)
                return ResultStatus.PASSED_WITH_WARNING
        except KeyError:
            print "WARNING::Property %s not present." % "->".join([str(path) for path in variable_path])
            return ResultStatus.join_statuses(ResultStatus.PASSED, overall_property_status)

        overall_property_status = ResultStatus.join_statuses(ResultStatus.PASSED, overall_property_status)
        print "RAW::\n\t[%s] - patching %s. Set %s to %s" % (
            overall_property_status, api_resource.odata_id, "->".join([str(path) for path in variable_path]), str(new_value))
        return overall_property_status

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

        if [member.name for member in enum_details.members]:
            return [member.name for member in enum_details.members]
        else:
            return None

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
            if type(path_element) == int:
                data = [data]
            else:
                data = {path_element: data}

        return data

    def get_value_from_body_path(self, variable_path, body):
        try:
            value = body
            try:
                for path_element in variable_path:
                    value = value[path_element]
            except:
                raise KeyError()

            return value
        except IndexError as err:
            raise KeyError(err.message)

    def validate_computer_system(self, api_resource):
        """
        function to specially handle computer system case when patching two values is required.
        """
        try:
            boot_source_override_enabled_property = self._metadata_container.types[
                "ComputerSystem.1.0.0.BootSourceOverrideEnabled"]
        except KeyError:
            print "WARNING::Metadata does not contain required information about properties BootSource and BootSourceOverrideEnabled"
            return ResultStatus.BLOCKED

        new_boot_source_override_targets = ["Hdd", "Pxe"]

        new_boot_source_override_enabled_list = self.get_enum_value(boot_source_override_enabled_property, None)

        overall_entity_status = ResultStatus.PASSED

        for new_boot_source_override_target, new_boot_source_override_enabled in itertools.product(
                new_boot_source_override_targets, new_boot_source_override_enabled_list):
            if not new_boot_source_override_enabled or not new_boot_source_override_target:
                continue

            payload = dict(
                Boot=dict(
                    BootSourceOverrideTarget=new_boot_source_override_target,
                    BootSourceOverrideEnabled=new_boot_source_override_enabled
                )
            )

            status, status_code, response_body, headers = self._api_caller. \
                perform_call(api_resource.odata_id, http_method=HttpMethods.PATCH, acceptable_return_codes=[
                ReturnCodes.OK, ReturnCodes.NO_CONTENT], payload=payload)

            if not status:
                print "ERROR::patching boot options failed with status code %s" % status_code
                overall_entity_status = ResultStatus.join_statuses(ResultStatus.FAILED, overall_entity_status)

        return overall_entity_status
