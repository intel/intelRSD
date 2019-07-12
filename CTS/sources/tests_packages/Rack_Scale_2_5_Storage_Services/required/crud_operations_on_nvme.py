#!/usr/bin/env python
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
from collections import OrderedDict
from uuid import uuid4

from cts_core.commons.api_caller import ApiCaller
from cts_core.commons.error import cts_error, cts_message
from cts_core.commons.json_comparator import JsonComparator
from cts_core.commons.preconditions import Preconditions, Requirement
from cts_core.commons.services import RegisterService
from cts_core.commons.services import ServiceTypes
from cts_core.discovery.api_explorer import ApiExplorer
from cts_core.metadata.metadata_constants import MetadataConstants2_5 as MetadataConstants
from cts_core.validation.validation_status import ValidationStatus
from cts_framework.commons.color_printer import ColorPrinter
from cts_framework.commons.enums import ReturnCodes
from cts_framework.helpers.python.cts_test_script import CtsTestScript
from cts_framework.helpers.python.test_script import TestScript


RANDOM_RANGE = 1000000000


@RegisterService(ServiceTypes.SS_2_5)
class CRUDOperations(CtsTestScript):
    TIMEOUT = 600
    DESCRIPTION = """test script validating CRUD (create, read, update and delete) functionalities"""

    CONFIGURATION_PARAMETERS = [
        TestScript.ConfigurationParameter(parameter_name="UniqueInitiatorName",
                                          parameter_description="NVMe Qualified Name (NQN) for Initiator",
                                          parameter_type=str, is_required=False, parameter_default_value="generate"),
        TestScript.ConfigurationParameter(parameter_name="UniqueTargetName",
                                          parameter_description="NVMe Qualified Name (NQN) for Target",
                                          parameter_type=str, is_required=False, parameter_default_value="generate")
    ]

    def run(self):
        if self.metadata_container is None:
            return

        self.chosen_endpoint, self.chosen_zone, self.chosen_volume, self.chosen_volume_collection = (None,)*4
        self.initiator_endpoint, self.target_endpoint, self.zone_endpoint, self.created_volume = (None,)*4

        self.initiator_unique_name = self.generate_unique_names() if self.configuration.UniqueInitiatorName \
            != "generate" else self.configuration.UniqueInitiatorName
        self.target_unique_name = self.generate_unique_names() if self.configuration.UniqueTargetName \
            != "generate" else self.configuration.UniqueTargetName

        test_name = "Storage Services CRUD test with NVM Express (NVMe) Support"
        print "MESSAGE::%s starting" % test_name

        print "TEST_CASE::API crawling"
        api_explorer = ApiExplorer(self.metadata_container, self.configuration)
        self.discovery_container, status = api_explorer.discover(MetadataConstants.SERVICE_ROOT_URI,
                                                                 MetadataConstants.SERVICE_ROOT)
        print "STATUS::{status}".format(status=status)

        requirements = [
            Requirement(MetadataConstants.SERVICE_ROOT, min=1, max=1),
            Requirement(MetadataConstants.STORAGE_SERVICE, min=1),
            Requirement(MetadataConstants.VOLUME, min=1),
            Requirement(MetadataConstants.VOLUME_COLLECTION, min=1)
        ]
        preconditions = Preconditions(self.metadata_container, requirements)
        status = preconditions.validate(self.discovery_container)

        if status == ValidationStatus.FAILED:
            self.set_status_failed()
            return

        self.api_caller = ApiCaller(self.configuration)

        status = ValidationStatus.join_statuses(status, self.crud_nvme())

        print "MESSAGE::%s overall status: %s" % (test_name, ColorPrinter.format_status(status))

    def crud_nvme(self):
        status = ValidationStatus.PASSED

        cts_message("CRUD NVME")

        # test pre check
        self.__find_volumes_without_any_links(optional_pre_check=True)

        try:
            status = ValidationStatus.join_statuses(status, self._test_create_volume())
        except:
            self._test_delete_all_created_endpoints_and_zones()
            status = ValidationStatus.FAILED

        self.__find_and_choose_endpoint_collection()
        self.__find_and_choose_volume()

        if not (self.chosen_volume and self.chosen_endpoint and self.chosen_zone):
            self.set_status_blocked()
            return ValidationStatus.BLOCKED

        cts_message("Chosen Volume for test: {volume_url}".format(volume_url=self.chosen_volume))

        try:
            status = ValidationStatus.join_statuses(status, self._test_create_initiator_endpoint())
            status = ValidationStatus.join_statuses(status, self._test_create_target_endpoint())
            status = ValidationStatus.join_statuses(status, self._test_create_zone_with_only_one_endpoint())
            status = ValidationStatus.join_statuses(status, self._test_patch_zone_with_target_endpoint())
        except:
            status = ValidationStatus.FAILED
        finally:
            status = ValidationStatus.join_statuses(status, self._test_delete_all_created_endpoints_and_zones())

        return status

    @staticmethod
    def generate_unique_names():
        return "nqn.2014-08.com.intel:uuid:" + str(uuid4())

    # Test case 1:
    def _test_create_volume(self):
        print "TEST_CASE::Create volume"

        post_volume_payload = dict(
            CapacityBytes=10000000
        )

        status, status_code, response_body, headers = self.api_caller.post_resource(
            self.chosen_volume_collection,
            self.discovery_container,
            payload=post_volume_payload,
            acceptable_return_codes=[ReturnCodes.OK, ReturnCodes.ACCEPTED, ReturnCodes.CREATED]
        )

        if not status:
            cts_error("CTS cannot create volume")
            self.set_status_failed()
            return ValidationStatus.FAILED

        self.created_volume = headers['location']
        cts_message("Volume created at {location}".format(location=self.created_volume))

        if not self._verify_size_of_created_volume(self.created_volume, post_volume_payload):
            cts_error("Service create volume with wrong capacity")
            self.set_status_failed()
            return ValidationStatus.FAILED

        self.set_status_passed()
        return ValidationStatus.PASSED

    # Test case 2:
    def _test_create_initiator_endpoint(self):
        print "TEST_CASE::Create initiator endpoint"

        post_initiator_endpoint_payload = dict(
            Identifiers=[dict(
                DurableName=self.initiator_unique_name,
                DurableNameFormat="NQN"
              )],
            IPTransportDetails=[],
            ConnectedEntities=[dict(
                EntityRole="Initiator",
                Identifiers=[],
            )],
            Links=dict(
                Oem=dict(
                    Intel_RackScale=dict())))

        status, status_code, response_body, headers = self.api_caller.post_resource(
            self.chosen_endpoint,
            self.discovery_container,
            payload=post_initiator_endpoint_payload,
            acceptable_return_codes=[ReturnCodes.OK, ReturnCodes.ACCEPTED, ReturnCodes.CREATED]
        )

        if not status:
            cts_error("CTS cannot create Initiator Endpoint")
            self.set_status_failed()
            return ValidationStatus.FAILED

        self.initiator_endpoint = headers['location']
        cts_message("Initiator Endpoint created at {location}".format(location=self.initiator_endpoint))

        try:
            verify = self.__verify_created_initiator_endpoint(self.initiator_endpoint, post_initiator_endpoint_payload)
        except:
            verify = False

        if not verify:
            cts_error("Service create initiator endpoint with wrong value")
            self.set_status_failed()
            return ValidationStatus.FAILED

        self.set_status_passed()
        return ValidationStatus.PASSED

    # Test case 3:
    def _test_create_target_endpoint(self):
        print "TEST_CASE::Create target endpoint"

        selected_volume = self.discovery_container.get(self.chosen_volume)
        post_target_endpoint_payload = dict(
            Identifiers=[dict(
                DurableName=self.target_unique_name,
                DurableNameFormat="NQN"
            )],
            IPTransportDetails=[],
            ConnectedEntities=[dict(
                EntityRole="Target",
                Identifiers=[],
                EntityLink={"@odata.id": selected_volume.odata_id},
            )],
            Links=dict(
                Oem=dict(
                    Intel_RackScale=dict())))

        status, status_code, response, headers = self.api_caller.post_resource(
                                                                          self.chosen_endpoint,
                                                                          self.discovery_container,
                                                                          payload=post_target_endpoint_payload,
                                                                          acceptable_return_codes=[ReturnCodes.OK,
                                                                                                   ReturnCodes.ACCEPTED,
                                                                                                   ReturnCodes.CREATED])

        if not status:
            cts_error("CTS can not create Target Endpoint")
            self.set_status_failed()
            return ValidationStatus.FAILED

        self.target_endpoint = headers['location']
        cts_message("Target Endpoint created at {location}".format(location=self.target_endpoint))

        try:
            verify = self.__verify_created_target_endpoint(self.target_endpoint, post_target_endpoint_payload)
        except:
            verify = False

        if not verify:
            cts_error("Service create target endpoint with wrong value")
            self.set_status_failed()
            return ValidationStatus.FAILED

        self.set_status_passed()
        return ValidationStatus.PASSED

    # Test case 4:
    def _test_create_zone_with_only_one_endpoint(self):
        print "TEST_CASE::Create zone with only one endpoint"

        try:
            initiator_endpoint_link = self.__get_odata_id_links(self.initiator_endpoint)
        except:
            cts_error("No valid initiator endpoint for zone creation")
            self.set_status_blocked()
            return ValidationStatus.BLOCKED

        post_create_zone_payload = dict(
            Links=dict(
                Endpoints=[{"@odata.id": initiator_endpoint_link}]
            )
        )

        status, status_code, _, headers = self.api_caller.post_resource(self.chosen_zone,
                                                                        self.discovery_container,
                                                                        payload=post_create_zone_payload,
                                                                        acceptable_return_codes=[ReturnCodes.ACCEPTED,
                                                                                                 ReturnCodes.CREATED,
                                                                                                 ReturnCodes.OK])

        if not status:
            cts_error("CTS can not create Zone")
            self.set_status_failed()
            return ValidationStatus.FAILED

        self.zone_endpoint = headers['location']
        cts_message("Zone created at {location}".format(location=self.zone_endpoint))

        if not self.__verify_created_zone(self.zone_endpoint, post_create_zone_payload):
            cts_error("Service zone created with wrong value")
            self.set_status_failed()
            return ValidationStatus.FAILED

        self.set_status_passed()
        return ValidationStatus.PASSED

    # Test case 5:
    def _test_patch_zone_with_target_endpoint(self):
        print "TEST_CASE::Patch zone with target endpoint"

        initiator_or_target_missing = False

        try:
            initiator_endpoint_link = self.__get_odata_id_links(self.initiator_endpoint)
        except:
            cts_error("No valid initiator endpoint for zone patching")
            initiator_or_target_missing = True

        try:
            target_endpoint_link = self.__get_odata_id_links(self.target_endpoint)
        except:
            cts_error("No valid target endpoint for zone patching")
            initiator_or_target_missing = True

        if initiator_or_target_missing:
            cts_error("Missing resources for zone patching")
            self.set_status_blocked()

            return ValidationStatus.BLOCKED

        patch_create_zone_payload = dict(
            Links=dict(
                Endpoints=[
                    {"@odata.id": initiator_endpoint_link},
                    {"@odata.id": target_endpoint_link}]
        ))

        status, status_code, _, headers = self.api_caller.patch_resource(self.zone_endpoint,
                                                                         self.discovery_container,
                                                                         payload=patch_create_zone_payload,
                                                                         acceptable_return_codes=[ReturnCodes.ACCEPTED,
                                                                                                  ReturnCodes.OK])
        if not status:
            cts_error("CTS can not PATCH Zone")
            self.set_status_failed()
            return ValidationStatus.FAILED

        cts_message("Zone PATCHED at {location}".format(location=self.zone_endpoint))

        if not self.__verify_patched_zone(self.zone_endpoint, patch_create_zone_payload):
            cts_error("Service zone patched with wrong value")
            self.set_status_failed()
            return ValidationStatus.FAILED

        self.set_status_passed()
        return ValidationStatus.PASSED

    # Test case 6:
    def _test_delete_all_created_endpoints_and_zones(self):
        status = ValidationStatus.PASSED
        for resource, link in OrderedDict([("volume", self.created_volume),
                                           ("zone", self.zone_endpoint),
                                           ("initiator endpoint", self.initiator_endpoint),
                                           ("target endpoint", self.target_endpoint)]).iteritems():
            print "TEST_CASE::Delete {resource}".format(resource=resource)
            if link:
                if not self.__delete_location(link):
                    self.set_status_failed()
                    status = ValidationStatus.join_statuses(status, ValidationStatus.FAILED)
                else:
                    self.set_status_passed()
                    status = ValidationStatus.join_statuses(status, ValidationStatus.PASSED)
            else:
                self.set_status_blocked()
                status = ValidationStatus.join_statuses(status, ValidationStatus.BLOCKED)
        return status

    def __check_resources_have_same_parent_url(self, res1, res2):
        return (self.discovery_container.get(res1)).parent_url == (self.discovery_container.get(res2)).parent_url

    def __choose_endpoint_and_zone_from_fabric_collection(self, endpoint_collection, zone_collection):
        for ec in endpoint_collection:
            for zc in zone_collection:
                if self.__check_resources_have_same_parent_url(ec, zc):
                    return ec, zc
        return None, None

    def __find_and_choose_volume(self):
        for volume in self.__find_volumes_without_any_links():
            self.chosen_volume = volume
            break

    def __delete_location(self, location_url):
        status, _, _, _ = self.api_caller.delete_resource(location_url,
                                                          self.discovery_container,
                                                          acceptable_return_codes=[ReturnCodes.NO_CONTENT])
        if not status:
            cts_error("Resource at %s was not properly deleted" % location_url)
            return False
        return self.__verify_resource_non_exist(location_url)

    def __verify_resource_non_exist(self, location_url_verification):
        status, _, _, _, _ = self.api_caller.get_resource(location_url_verification,
                                                          self.discovery_container,
                                                          acceptable_return_codes=[ReturnCodes.BAD_REQUEST,
                                                                                   ReturnCodes.NOT_FOUND])
        if not status:
            cts_error("Resource at %s was not properly deleted" % location_url_verification)
            return False
        cts_message("Resource at %s was properly deleted" % location_url_verification)
        return True

    def __find_and_choose_endpoint_collection(self):
        endpoint_collection = dict(self.discovery_container.get_resources(
                                    MetadataConstants.ENDPOINT_COLLECTION))

        zones_collection = dict(self.discovery_container.get_resources(
                                    MetadataConstants.ZONE_COLLECTION))

        if not endpoint_collection:
            cts_error("No Endpoint Collection")
            self.set_status_blocked()

        if not zones_collection:
            cts_error("No Zone Collection")
            self.set_status_blocked()

        self.chosen_endpoint, self.chosen_zone = \
            self.__choose_endpoint_and_zone_from_fabric_collection(
                    endpoint_collection,
                    zones_collection)

    def __find_volumes_without_any_links(self, optional_pre_check=False):
        reusable_volumes = []
        logical_drive_collections = dict(self.discovery_container.get_resources(
                                         MetadataConstants.VOLUME_COLLECTION,
                                         any_child_version=True))

        # Allocated Volumes are connected with this same type as Volume
        volumes_collection_wo_allocated_volumes = [x for x in logical_drive_collections if not "AllocatedVolumes" in x]

        for collection in volumes_collection_wo_allocated_volumes:
            if not self.chosen_volume_collection:
                self.chosen_volume_collection = collection
            logical_drive_groups = dict(self.discovery_container.get_resources(MetadataConstants.VOLUME,
                                                                               any_child_version=True))
            volumes_collection = []

            for c in logical_drive_groups.keys():
                try:
                    # this Volume we can use to our tests
                    if len(self.discovery_container.get(c).body["Links"]["Oem"]["Intel_RackScale"]["Endpoints"]) == 0:
                        volumes_collection.append(c)
                except KeyError:
                    pass

            # if there are any free Volume, suspend test
            if len(volumes_collection) == 0:
                if optional_pre_check:
                    return False
                cts_error("No reusable Volume. Create a Volume without linked Endpoints")
                self.set_status_blocked()
            else:
                reusable_volumes.extend(volumes_collection)
        return reusable_volumes

    def __get_odata_id_links(self, link):
        return (self.discovery_container.get(link)).odata_id

    # verification code
    def __verify_patched_zone(self, resource_location, resource_payload):
        print "MESSAGE::Comparing newly patched zone to given payload"
        return self.__verify_zone(resource_location, resource_payload)

    def __verify_created_zone(self, resource_location, resource_payload):
        print "MESSAGE::Comparing newly created zone to given payload"
        return self.__verify_zone(resource_location, resource_payload)

    def __verify_zone(self, resource_location, resource_payload):
        location_payload_to_compare = JsonComparator.odict_to_dict(
                                            self.discovery_container.get(resource_location).body["Links"]["Endpoints"])
        payload_to_compare = resource_payload["Links"]["Endpoints"]

        return JsonComparator.compare_lists(location_payload_to_compare, payload_to_compare)

    def __verify_created_target_endpoint(self, resource_location, resource_payload):
        print "MESSAGE::Comparing newly created target endpoint to given payload"

        location_payload_to_compare = JsonComparator.odict_to_dict(
                                            self.discovery_container.get(resource_location).body)
        payload_to_compare = resource_payload

        verification_dict = {'ConnectedEntities': ['EntityLink',
                                                   'EntityRole'],
                             'Identifiers': ['DurableNameFormat',
                                             'DurableName']}

        return self.__verify_body(location_payload_to_compare, payload_to_compare, verification_dict)

    def __verify_created_initiator_endpoint(self, resource_location, resource_payload):
        print "MESSAGE::Comparing newly created initiator endpoint to given payload"
        location_payload_to_compare = JsonComparator.odict_to_dict(
                                            self.discovery_container.get(resource_location).body)
        payload_to_compare = resource_payload

        verification_dict = {'ConnectedEntities': ['EntityRole'],
                             'Identifiers': ['DurableNameFormat',
                                             'DurableName']}

        return self.__verify_body(location_payload_to_compare, payload_to_compare, verification_dict)

    def _verify_size_of_created_volume(self, resource_location, resource_payload):
        print "MESSAGE::Comparing newly created volume to given payload"

        discovery_service_capacity = self.discovery_container.get(resource_location).body["CapacityBytes"]
        resource_payload_capacity = resource_payload["CapacityBytes"]

        return self.__verify_bytes(resource_payload_capacity, discovery_service_capacity)

    @staticmethod
    def __verify_body(resource_1, resource_2, verification_dict):
        try:
            for verification_key in verification_dict.keys():
                for verification_elem in verification_dict[verification_key]:
                    if resource_1[verification_key][0][verification_elem] != \
                            resource_2[verification_key][0][verification_elem]:
                        return False
        except KeyError:
            return False
        return True

    @staticmethod
    def __verify_bytes(resource_on_payload, resource_discovered, min_bytes=None, max_bytes=None):
        if not min_bytes:
            # default 4 mb
            min_bytes = 4000000
        if not max_bytes:
            # default 4 mb
            max_bytes = 4000000

        if (resource_on_payload - min_bytes) <= resource_discovered <= (resource_on_payload + max_bytes):
            return True
        return False

    @staticmethod
    def __verify_strings(resource_on_payload, resource_discovered):
        resource_on_payload = str(resource_on_payload).lower()
        resource_discovered = str(resource_discovered).lower()

        return resource_on_payload == resource_discovered


CRUDOperations.service_cli()
