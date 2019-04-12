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
from abc import abstractmethod, abstractproperty

from cts_core.commons.error import cts_error
from cts_core.commons.json_helpers import update_existing_keys, create_data
from cts_core.validation.validation_status import ValidationStatus
from cts_framework.commons.enums import RequestStatus


class ApplicabilityTestResult:
    NOT_MATCHED = "NOT_MATCHED"
    MATCHED = "MATCHED"
    PATCH_AS_TRIVIAL = "PATCH_AS_TRIVIAL"
    SKIP_PROPERTY = "SKIP_PROPERTY"


class PatchNonTrivialPropertyBase(object):
    def __init__(self, metadata_container, discovery_container, api_caller, patching_strategy,
                 api_resource, variable_path, property_description):
        self.metadata_container = metadata_container
        self.discovery_container = discovery_container
        self._api_caller = api_caller
        self.patching_strategy = patching_strategy
        self.url = api_resource.url
        self.variable_path = variable_path
        self.property_description = property_description

    def payload(self):
        raise NotImplementedError()

    def verify(self, resource_after_patch, expected_value):
        raise NotImplementedError()

    @property
    def api_resource(self):
        return self.discovery_container[self.url]

    def handle_nontrivial_property(self):
        try:
            value_pre = self.get_property_value()
        except KeyError:
            return ValidationStatus.FAILED

        for payload in self.payload():
            property_str = "->".join(map(str, self.variable_path + [self.property_description.name]))
            print "TEST_CASE:: Patch %s[%s]" % (self.api_resource.odata_id, property_str)
            patch_status = self.do_patch(payload)
            print "STATUS::%s" % patch_status

            if patch_status == ValidationStatus.FAILED:
                return ValidationStatus.FAILED

            print "TEST_CASE:: Verify %s[%s]" % (self.api_resource.odata_id, property_str)
            verify_status = self.verify(self.api_resource, payload)
            print "STATUS::%s" % verify_status

        print "TEST_CASE:: Restore %s[%s]" % (self.api_resource.odata_id, property_str)
        restore_status = self.do_restore(value_pre)
        print "STATUS::%s" % restore_status

        return ValidationStatus.join_statuses(patch_status, verify_status, restore_status)

    def get_property_value(self):
        try:
            value = self.api_resource.get_value_from_path(self.property_full_path)
        except KeyError as key:
            cts_error("Unable to access {odataid:id}->{path}. Key={key}",
                      odataid=self.api_resource.odata_id, path="->".join(self.variable_path), key=key)
            raise
        return value

    @property
    def property_full_path(self):
        return self.variable_path + [self.__class__.PROPERTY_NAME]

    @staticmethod
    def test_applicability(metadata_container, property_type):
        return ApplicabilityTestResult.NOT_MATCHED


    def do_patch(self, payload):
        payload_shifted = create_data(self.property_full_path, payload)
        status, status_code, response_body, headers = self._api_caller.patch_resource(
            self.api_resource.url,
            self.discovery_container,
            payload=payload_shifted,
            acceptable_return_codes=self.patching_strategy.allowable_return_codes)

        patch_applied = self.patching_strategy.was_patch_applied(status_code)

        if status != RequestStatus.SUCCESS:
            cts_error("{odata_id:id} Patching {property} failed (unexpected status code: {code})",
                      odata_id=self.api_resource.odata_id,
                      property=self.property_description.name,
                      code=status_code)
            validation_status = ValidationStatus.FAILED
        elif not patch_applied:
            cts_error("{odata_id:id} Patching {property} failed (patch not applied); status code: {code}",
                      odata_id=self.api_resource.odata_id,
                      property=self.property_description.name,
                      code=status_code)
            validation_status = ValidationStatus.FAILED
        else:
            validation_status = ValidationStatus.PASSED

        return validation_status

    def build_payload_for_restore(self, value):
        payload = self.payload().next()
        value = create_data(self.property_full_path, value)
        update_existing_keys(payload, value)
        return payload

    def do_restore(self, value_pre):
        payload = self.build_payload_for_restore(value_pre)
        return self.do_patch(payload)


