"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2017 Intel Corporation
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
from json import dumps
from os import getenv
from traceback import format_exc

from bs4 import BeautifulSoup

from cts_core.commons.api_caller import ApiCaller
from cts_core.commons.error import cts_error, cts_warning
from cts_core.commons.json_helpers import get_odata_type, create_data
from cts_core.commons.preconditions import Preconditions
from cts_core.metadata.model.metadata_types.metadata_type_categories import MetadataTypeCategories
from cts_core.metadata.model.property import PatchStatus, Property
from cts_core.validation.patch.nontrivial_property.patch_nontrivial_property_mixin import PatchNontrivialPropertyMixin
from cts_core.validation.patch.nontrivial_property.patch_nontrivial_property_base import ApplicabilityTestResult
from cts_core.validation.patch.skip_list_mixin import SkipListMixin
from cts_core.validation.validation_status import ValidationStatus
from cts_framework.commons.enums import RequestStatus
from cts_framework.configuration.configuration import ValueNotFound


class Context:
    def __init__(self, api_resource):
        """
        :type api_resource: cts_core.discovery.api_resource.ApiResourceProxy
        """

        self.api_resource = api_resource
        self.attempted_patches = []

    def register_patch_attempt(self, property_name, value_requested=None):
        if value_requested is None:
            value_requested = '[...]'
        self.attempted_patches.append("%s=%s" % (property_name, str(value_requested)))


class MetadataPatchValidator(SkipListMixin, PatchNontrivialPropertyMixin):
    def __init__(self, metadata_container, configuration, strategy, requirements=None):
        """
        :type metadata_container: cts_core.metadata.metadata_container.MetadataContainer
        :type configuration: cts_framework.configuration.configuration.Configurations
        :type strategy: cts_core.validation.patch.patching_strategy.PatchingStrategy
        :type requirements: list
        """
        self._metadata_container = metadata_container
        self._api_caller = ApiCaller(configuration)
        self._strategy = strategy
        self._preconditions = Preconditions(metadata_container, requirements)
        self._fast_mode = getenv('CTS_PATCH_ONE_PER_TYPE')
        self._types_patched = set()
        if self._fast_mode:
            cts_warning("Test results may be UNRELIABLE - PATCHING ONLY ONE RESOURCE OF EACH TYPE!")

    def validate(self, discovery_container):
        self.discovery_container = discovery_container
        status = self._check_mandatory_entities()

        for api_resource in self._enumerate_resources(discovery_container):
            context = Context(api_resource)
            resource_status = self._validate_resource(context)
            self._print_resource_footer(api_resource, context, resource_status)
            status = ValidationStatus.join_statuses(status, resource_status)

        print "SCREEN::Overall status: %s" % status
        return status

    def _enumerate_resources(self, discovery_container):
        count = len(discovery_container)
        resource_urls = sorted(self.discovery_container.keys())
        for idx, url in enumerate(resource_urls):
            api_resource = discovery_container[url]
            self._print_progress(api_resource, count, idx)
            if self.skip_resource(api_resource):
                continue
            if self._fast_mode:
                generalized_id = discovery_container._generalize_url(api_resource.url)
                if generalized_id in self._types_patched:
                    print "MESSAGE::skipping... other {} has already been tested". \
                        format(generalized_id)
                    continue
                else:
                    self._types_patched.add(generalized_id)

            yield api_resource

    def _print_progress(self, api_resource, count, idx):
        print "SCREEN::" + "-" * 120
        progress = "[%5d/%5d]" % (idx + 1, count)
        print "MESSAGE::%s - %s : %s" % \
              (progress, api_resource.odata_id, api_resource.odata_type)

    def _check_mandatory_entities(self):
        print "TEST_CASE::Checking for mandatory entities"
        status = self._preconditions.validate(self.discovery_container)
        print "STATUS::%s" % status
        return status

    def _validate_resource(self, context):
        """
        :type context: Context
        :rtype: str
        """
        api_resource = context.api_resource

        if self._metadata_container.to_be_ignored(api_resource.odata_type):
            return ValidationStatus.PASSED

        try:
            properties_list = self._metadata_container.entities[api_resource.odata_type].properties.values()
            try:
                return self._validate_property_list(context,
                                                    list(),
                                                    properties_list)
            except SystemExit:
                raise
            except:
                cts_error("Unhandled exception {exception:stacktrace} " \
                          "while handling resource {odata_id:id}",
                          exception=format_exc(),
                          odata_id=api_resource.odata_id)
                return ValidationStatus.FAILED
        except KeyError:
            cts_error("Unable to find definition of entity type {odata_type}",
                      odata_type=api_resource.odata_type)
            return ValidationStatus.FAILED

    def _validate_property_list(self, context, variable_path, property_list):
        """
        :type context: Context
        :type variable_path: list [str or int]
        :type property_list: list[cts_core.metadata.model.property.Property]
        :rtype: str
        """
        api_resource = context.api_resource

        status = ValidationStatus.PASSED
        # use local copy - api resource will be modified while test is executed
        local_property_list = list(property_list)
        for property_description in local_property_list:
            status = ValidationStatus.join_statuses(self._validate_property(context,
                                                                            variable_path,
                                                                            property_description),
                                                    status)

        # validate additional properties
        try:
            body = api_resource.get_value_from_path(variable_path)
        except KeyError as key:
            cts_warning("Unable to access {odataid:id}->{path}. Key={key}",
                        odataid=api_resource.odata_id, path="->".join(variable_path), key=key)
            body = None

        if body is not None:
            all_properties = set(body.keys())
            known_properties = set([property.name for property in local_property_list])
            additional_properties = all_properties - known_properties
            for additional_property_name in additional_properties:
                property_value = body[additional_property_name]
                if isinstance(property_value, dict) and "@odata.type" in property_value:
                    raw_soup = """
                    <Property Name="{name}" Type="{type}">
                        <Annotation Term="OData.Permissions"
                                    EnumMember="OData.Permission/ReadWrite"/>
                    </Property>
                    """
                    soup = BeautifulSoup(raw_soup.format(name=additional_property_name,
                                                         type=get_odata_type(property_value)),
                                         "lxml").find_all("property")[0]

                    adhoc_description = Property(self._metadata_container, None, soup)
                    status = ValidationStatus.join_statuses(
                        self._validate_property(context,
                                                variable_path,
                                                adhoc_description),
                        status)

        return status

    def _validate_property(self, context, variable_path, property_description,
                           skip_collection=False):
        """
        :type context: Context
        :type variable_path: list [str or int]
        :type property_description: cts_core.metadata.model.property.Property
        :type skip_collection: bool
        :rtype: str
        """
        applicability = None
        if property_description.patch_status == PatchStatus.NONTRIVIAL:
            applicability, validation_result = self.handle_nontrivial(context, variable_path, property_description)
            if applicability == ApplicabilityTestResult.MATCHED:
                return validation_result
            if applicability in [ApplicabilityTestResult.NOT_MATCHED, ApplicabilityTestResult.SKIP_PROPERTY]:
                return validation_result

        api_resource = context.api_resource

        if not skip_collection:
            variable_path = variable_path + [property_description.name]

        try:
            property_body = api_resource.get_value_from_path(variable_path)
        except KeyError as error:
            if property_description.is_required:
                path_s = "->".join([str(segment) for segment in variable_path])
                print "TEST_CASE::Patching %s->%s" % (api_resource.odata_id, path_s)
                cts_error("Unable to patch {error}", error=error)
                status = ValidationStatus.FAILED
                print "STATUS::%s" % status
            else:
                status = ValidationStatus.PASSED

            return status

        if property_description.is_collection and not skip_collection:
            status = ValidationStatus.PASSED

            for path_element, _ in enumerate(property_body):
                status = \
                    ValidationStatus.join_statuses(status,
                                                   self._validate_property(context,
                                                                           variable_path + [path_element],
                                                                           property_description,
                                                                           skip_collection=True))
            return status
        else:
            try:
                if self._metadata_container.types[property_description.type].type_category \
                        == MetadataTypeCategories.COMPLEX_TYPE:
                    return self._validate_property_list(context,
                                                        variable_path,
                                                        self._metadata_container.types[property_description.type].
                                                        properties.itervalues())
                else:
                    return self._validate_leaf_property(context,
                                                        variable_path,
                                                        property_description)
            except KeyError as key:
                cts_error("Unable to find definition of type {type} referenced by {odata_id:id}",
                          type=key,
                          odata_id=api_resource.odata_id)
                return ValidationStatus.FAILED

    def _validate_leaf_property(self, context, variable_path, property_description):
        """
        :type context: Context
        :type variable_path: list [str or int]
        :type property_description: cts_core.metadata.model.property.Property
        :rtype: str
        """
        if property_description.patch_status in (PatchStatus.NOT_PATCHABLE, PatchStatus.NOT_DEFINED):
            return ValidationStatus.PASSED

        if property_description.patch_status == PatchStatus.NONTRIVIAL:
            applicability, validation_result = self.handle_nontrivial(context, variable_path, property_description)
            if applicability == ApplicabilityTestResult.MATCHED:
                return validation_result
            if applicability in [ApplicabilityTestResult.NOT_MATCHED, ApplicabilityTestResult.SKIP_PROPERTY]:
                return validation_result

        return self._validate_patchable_property(context, variable_path, property_description)

    def _validate_patchable_property(self, context, variable_path, property_description):
        """
        :type context: Context
        :type variable_path: list [str or int]
        :type property_description: cts_core.metadata.model.property.Property
        :rtype: str
        """
        api_resource = context.api_resource
        verify_only_response = property_description.patch_status == PatchStatus.WRITE_ONLY

        new_values_list = property_description.generate_values(property_description.annotations)
        allowed_values = api_resource.get_allowable_from_path(variable_path)
        if allowed_values:
            new_values_list = [value for value in new_values_list if value in allowed_values]

        collection = None
        if property_description.is_collection and len(variable_path) > 1:
            collection = api_resource.get_value_from_path(variable_path[:-1])

        try:
            value_pre = current_value = api_resource.get_value_from_path(variable_path)
        except KeyError:
            return ValidationStatus.PASSED

        total_status = ValidationStatus.PASSED

        for new_value in new_values_list:
            if new_value != current_value:
                status, current_value = \
                    self._patch_and_verify_property(context,
                                                    variable_path,
                                                    current_value,
                                                    new_value,
                                                    verify_only_response,
                                                    collection=collection,
                                                    property_description_type=property_description.name)

                total_status = ValidationStatus.join_statuses(total_status, status)

                if current_value is None:
                    try:
                        value_pre = current_value = api_resource.get_value_from_path(variable_path)
                    except KeyError:
                        return total_status

        if not allowed_values or value_pre in allowed_values:
            total_status = ValidationStatus.join_statuses(total_status,
                                                          self._restore_property(context,
                                                                                 value_pre,
                                                                                 variable_path,
                                                                                 collection=collection))
        else:
            print "MESSAGE::Original value '{value_pre}' is illegal. Will not _restore_property" \
                .format(value_pre=dumps(value_pre))

        return total_status

    def _patch_and_verify_property(self, context, variable_path, current_value, value_requested,
                                   verify_only_response=False, collection=None, property_description_type=None):
        """
        :type context: Context
        :type variable_path: list [str or int]
        :type current_value: *
        :type value_requested: *
        :type verify_only_response: bool
        """

        property = "%s[%s]" \
                   % (context.api_resource.odata_id, "->".join([str(path) for path in
                                                                variable_path]))

        patch_applied, status, validation_status = self._patch_property(context,
                                                                        property,
                                                                        current_value,
                                                                        value_requested,
                                                                        variable_path,
                                                                        collection=collection)
        # only _verify_property if the PATCH request succeeded and is not only writeable
        if status == RequestStatus.SUCCESS and patch_applied:
            if not verify_only_response:
                verify_status, current_value = self._verify_property(context,
                                                                     patch_applied,
                                                                     property,
                                                                     current_value,
                                                                     value_requested,
                                                                     variable_path,
                                                                     property_description_type=property_description_type)
                validation_status = ValidationStatus.join_statuses(validation_status, verify_status)
            else:
                validation_status = ValidationStatus.join_statuses(validation_status, ValidationStatus.PASSED)

        return validation_status, current_value

    def _patch_property(self, context, property_name, value_pre, value_requested, variable_path, collection=None):
        """
        Issues Patch request to update property to new requested value. Tries to determine
        if patch has been applied and sets validation status accordingly.

        :type context: Context
        :type property_name: str
        :type value_pre: *
        :type value_requested: *
        :type variable_path: list [str or int]
        :rtype: (bool, bool, str)
        """

        api_resource = context.api_resource

        print "TEST_CASE:: Patch %s := %s" % (property_name, str(value_requested))
        context.register_patch_attempt(property_name, str(value_requested))

        data = create_data(variable_path, value_requested, collection)

        status, status_code, response_body, headers = self._api_caller.patch_resource(
            api_resource.url,
            self.discovery_container,
            payload=data,
            acceptable_return_codes=self._strategy.allowable_return_codes)

        patch_applied = self._strategy.was_patch_applied(status_code)

        if status != RequestStatus.SUCCESS or not patch_applied:
            validation_status = ValidationStatus.FAILED
        else:
            validation_status = ValidationStatus.PASSED
        print "STATUS::%s" % validation_status

        return patch_applied, status, validation_status

    def _verify_property(self, context, patch_applied, property_name, value_pre,
                         value_requested, variable_path, property_description_type):
        """
        Verifies if property value after patch is as expected.
        Should be equal to original value if patch has not been applied.
        Should be equal to requested value if patch has been applied.

        :type context: Context
        :type patch_applied: bool
        :type property_name: str
        :type value_pre: *
        :type value_requested: *
        :type variable_path: list[str or int]
        :rtype: (str, *)
        """

        api_resource = self.discovery_container[context.api_resource.url] # refresh reference to the resource

        validation_status = ValidationStatus.PASSED
        print "TEST_CASE::Verify %s (expected: %s)" % (property_name, str(value_requested))

        try:
            value_post = api_resource.get_value_from_path(variable_path)

            if self._validate_corner_cases(property_description_type, value_post, value_requested) and patch_applied:
                print "STATUS::%s" % validation_status
                return validation_status, value_post

            if value_requested != value_post and patch_applied:
                cts_error("{odata_id:id} Unexpected value after patching {property} - " +
                          "IS : {post:ignore}, EXPECTED : {requested:ignore}",
                          odata_id=api_resource.odata_id,
                          property=property_name,
                          post=value_post,
                          requested=value_requested)
                validation_status = ValidationStatus.FAILED
            elif value_pre != value_post and not patch_applied:
                cts_error("{odata_id:id} Service reported that the patch has not been applied, "
                          "but '{property}' has changed unexpectedly "
                          "from {pre:ignore} to {post:ignore}",
                          odata_id=api_resource.odata_id,
                          property=property_name,
                          pre=value_pre,
                          post=value_post)
                validation_status = ValidationStatus.FAILED
        except ValueNotFound:
            validation_status = ValidationStatus.FAILED

        print "STATUS::%s" % validation_status
        return validation_status, value_post

    def _restore_property(self, context, value_pre, variable_path, collection=None):
        """
        Final patching that restores original value of the property.

        :type context: Context
        :type value_pre: *
        :type variable_path: list [int or str]
        :return: str
        """
        api_resource = context.api_resource

        validation_status = ValidationStatus.PASSED

        property = "%s" % ("->".join([str(path) for path in variable_path]))
        print "TEST_CASE::Restore %s := %s" % (property, dumps(value_pre))

        data = create_data(variable_path, value_pre, collection)
        status, status_code, response_body, headers = self._api_caller.patch_resource(
            api_resource.url,
            self.discovery_container,
            payload=data,
            acceptable_return_codes=self._strategy.allowable_return_codes)
        if status != RequestStatus.SUCCESS:
            cts_error("{odata_id:id} Restoring {property} failed. status code {code}",
                      odata_id=api_resource.odata_id,
                      property=property,
                      code=status_code)
            validation_status = ValidationStatus.FAILED
        print "STATUS::%s" % validation_status
        return validation_status

    def _print_resource_footer(self, api_resource, context, resource_status):
        if not context.attempted_patches:
            if resource_status not in [ValidationStatus.PASSED,
                                       ValidationStatus.PASSED_WITH_WARNINGS]:
                # 0 attempted patches means 0 test cases.
                # FAILED means error (visible in the log) happened during validation,
                # but not associated with any TEST_CASE
                # For the purpose of showing failing test case, we produce dummy TEST_CASE here
                print "TEST_CASE::Patching %s" % api_resource.odata_id
                print "STATUS::%s" % resource_status
            else:
                print "MESSAGE::[{resource} - 0 patchable properties found]". \
                    format(resource=api_resource.odata_id)

    def _validate_corner_cases(self, property_description_type, value_post, value_requested):
        if property_description_type == "MACAddress":
            return self._validate_canonical_form_of_mac_address(value_post, value_requested)
        return False

    def _validate_canonical_form_of_mac_address(self, value_post, value_requested):
        post = self._mac_address_standarizer(value_post)
        req = self._mac_address_standarizer(value_requested)

        return int(post, 16) == int(req, 16)

    def _mac_address_standarizer(self, mac_address):
        replace_dict = {":": "", "-": ""}
        for i, j in replace_dict.iteritems():
            mac_address = mac_address.replace(i, j)

        return mac_address