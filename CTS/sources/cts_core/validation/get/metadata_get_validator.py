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
import collections
import re

from cts_core.commons.error import cts_error, cts_warning, cts_message
from cts_core.commons.preconditions import Preconditions
from cts_core.metadata.model.annotation import Annotation
from cts_core.validation.validation_status import ValidationStatus


class MetadataGetValidator:
    def __init__(self, metadata_container, requirements=None, ignore=None, skip_check_predicate=None):
        """
        :type metadata_container: cts_core.metadata.metadata_container.MetadataContainer
        :type requirements: list(Requirement)
        """
        self._metadata_container = metadata_container
        self._preconditions = Preconditions(metadata_container, requirements, skip_check_predicate=skip_check_predicate)
        self._ignore = ignore if ignore is not None else set()

    def validate(self, discovery_container):
        """
        :type discovery_container: cts_core.discovery.discovery_container.DiscoveryContainer
        :rtype: cts_core.validation.validation_status.ValidationStatus
        """
        print "TEST_CASE::Checking for mandatory entities"
        status = self._preconditions.validate(discovery_container)
        print "STATUS::%s" % status

        resources = discovery_container.itervalues()
        count = len(discovery_container)
        for idx, api_resource in enumerate(resources):
            print "TEST_CASE::%s" % api_resource.odata_id
            print "MESSAGE::[%5d/%5d] Checking %s : %s" % (
                idx + 1, count, api_resource.odata_id, api_resource.odata_type)

            api_resource_status = self.validate_single_entity(api_resource)
            print "STATUS::%s" % api_resource_status

            redfish_uri_compliance = discovery_container.validate_redfish_uris_consistency(api_resource, self._metadata_container)
            status = ValidationStatus.join_statuses(status, api_resource_status, redfish_uri_compliance)

        print "SCREEN::Overall status: %s" % status
        return status

    def get_validated_information(self, discovery_container):
        resources = discovery_container.itervalues()

        ValidatedEntity = collections.namedtuple('ValidatedEntity', 'test_case count checking message status')
        list_of_validated_elements = []

        for idx, api_resource in enumerate(resources):
            api_resource_status = self.validate_single_entity(api_resource)

            list_of_validated_elements.append(ValidatedEntity(test_case=api_resource.odata_id,
                                                              count=idx + 1,
                                                              checking=api_resource.odata_id,
                                                              message=api_resource.odata_type,
                                                              status=api_resource_status))

        return list_of_validated_elements

    def validate_single_entity(self, api_resource):
        """
        :type api_resource: cts_core.discovery.api_resource.ApiResourceProxy
        :rtype: str
        """
        if not api_resource.body:
            cts_warning("{odata_id:id} can't be analyzed - body is empty", api_resource.odata_id)
            return ValidationStatus.FAILED

        try:
            entity_details = self._metadata_container.entities[api_resource.odata_type]
        except KeyError:
            cts_error("Unable to find definition of entity {expected_odata_type} for resource {odata_id:id}",
                      expected_odata_type=api_resource.odata_type, odata_id=api_resource.odata_id)
            return ValidationStatus.FAILED

        return entity_details.validate(resource=api_resource.body,
                                       resource_path=api_resource.odata_id,
                                       odata_type=api_resource.odata_type)
