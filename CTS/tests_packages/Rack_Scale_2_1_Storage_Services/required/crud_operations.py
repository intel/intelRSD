#!/usr/bin/env python
"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2016-2017 Intel Corporation
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
from urlparse import urlparse

from cts_core.commons.api_caller import ApiCaller
from cts_core.commons.json_comparator import JsonComparator
from cts_core.commons.preconditions import Preconditions, Requirement
from cts_core.commons.services import RegisterService
from cts_core.commons.services import ServiceTypes
from cts_core.discovery.api_explorer import ApiExplorer
from cts_core.discovery.api_resource import ApiResource
from cts_core.metadata.metadata_constants import MetadataConstants2_1 as MetadataConstants
from cts_core.validation.validation_status import ValidationStatus
from cts_framework.commons.color_printer import ColorPrinter
from cts_framework.commons.enums import ReturnCodes
from cts_framework.helpers.python.cts_test_script import CtsTestScript


@RegisterService(ServiceTypes.SS_2_1)
class CRUDOperations(CtsTestScript):
    TIMEOUT = 600
    DESCRIPTION = """test script validating CRUD (create, read, update and delete) functionalities"""

    def run(self):
        test_name = "Storage Services CRUD test"
        print "MESSAGE::%s starting" % test_name

        print "TEST_CASE::API crawling"
        api_explorer = ApiExplorer(self.metadata_container, self.configuration)
        self.discovery_container, status = api_explorer.discover(MetadataConstants.SERVICE_ROOT_URI,
                                                            MetadataConstants.SERVICE_ROOT)
        print "STATUS::{status}".format(status=status)

        requirements = [
            Requirement(MetadataConstants.SERVICE_ROOT, min=1, max=1),
            Requirement(MetadataConstants.STORAGE_SERVICE, min=1),
            Requirement(MetadataConstants.LOGICAL_DRIVE, min=2)
        ]
        preconditions = Preconditions(self.metadata_container, requirements)
        status = preconditions.validate(self.discovery_container)

        self.api_caller = ApiCaller(self.configuration)

        status = ValidationStatus.join_statuses(status, self.crud_logical_drive())
        status = ValidationStatus.join_statuses(status, self.crud_remote_target())

        print "MESSAGE::%s overall status: %s" % (test_name, ColorPrinter.format_status(status))


    def _test_case_create_logical_drive(self, logical_drive_collection, payload=None):
        print "TEST_CASE::Create a logical drive"

        if not payload:
            payload = self.post_logical_drive_payload
        status, status_code, response_body, headers = self.api_caller.post_resource(
            logical_drive_collection, payload=payload)

        if not status:
            print "ERROR::Wrong status code of POST response: %d" % status_code
            self.set_status_failed()
            return ValidationStatus.FAILED

        try:
            self.created_logical_drive = headers["Location"]
            print "MESSAGE::Newly created logical drive url %s" % self.created_logical_drive
        except KeyError:
            print "ERROR::In header shall be provided location to created resource"
            self.set_status_failed()
            return ValidationStatus.FAILED

        print "MESSAGE::Logical drive created"
        self.set_status_passed()
        return ValidationStatus.PASSED

    def _test_case_get_created_logical_drive(self, logical_drive=None):
        print "TEST_CASE::Get the created logical drive"

        if not logical_drive:
            logical_drive = self.created_logical_drive
        status, status_code, response_body, _ = self.api_caller.get_resource(logical_drive)

        if not status:
            print "ERROR::Wrong status code of GET response: %s" % status_code
            print "ERROR::Response body:\n%s" % response_body
            self.set_status_failed()
            return ValidationStatus.FAILED
        else:
            api_resource = ApiResource(response_body, response_body["@odata.type"])
            self.discovery_container.add_resource(logical_drive, api_resource)
            print "MESSAGE::Comparing newly created logical drive to given payload"
            ignored_fields = ["/Name", "/Image"]  # Name setting not supported by PSME REST API, Image is optional

            print "MESSAGE::Comparing newly created logical drive to given payload"
            if not JsonComparator.compare_json_to_golden(response_body, self.expected_created_logical_drive_body,
                                                         ignore = ignored_fields):
                print "ERROR::Newly created logical drive's body incorrect"
                self.set_status_failed()
                print "MESSAGE::Proceeding with deleting the resource"
                return ValidationStatus.FAILED

        print "MESSAGE::Newly created logical drive's body correct"
        self.set_status_passed()
        return ValidationStatus.PASSED

    def _test_case_update_created_logical_drive(self):
        print "TEST_CASE::Update the created logical drive"

        patch_logical_drive_payload = dict(
            Bootable = not self.post_logical_drive_payload["Bootable"]
        )

        status, status_code, response_body, _ = self.api_caller.patch_resource(self.created_logical_drive,
                                                                               payload=patch_logical_drive_payload)

        if not status:
            print "ERROR::Wrong status code of PATCH reponse %s" % status_code
            print "ERROR::Response body:\n%s" % response_body
            self.set_status_failed()
            return ValidationStatus.FAILED

        status, status_code, response_body, _ = self.api_caller.get_resource(self.created_logical_drive)

        if not status:
            print "ERROR::Wrong status code of GET reponse %s" % status_code
            print "ERROR::Response body:\n%s" % response_body
            self.set_status_failed()
            return ValidationStatus.FAILED
        else:
            print "MESSAGE::Verifying whether the logical drive's update has taken place"
            if not JsonComparator.compare_json_to_golden(response_body, patch_logical_drive_payload):
                print "ERROR::Newly created logical drive could not be updated"
                self.set_status_failed()
                print "MESSAGE::Proceeding with deleting the resource"
                return ValidationStatus.FAILED

        print "MESSAGE::Newly created logical drive updated correctly"
        self.set_status_passed()
        return ValidationStatus.PASSED

    def _test_case_delete_created_logical_drive(self, logical_drive=None):
        print "TEST_CASE::Delete the created logical drive"

        if not logical_drive:
            logical_drive = self.created_logical_drive

        status, status_code, response_body, _ = self._delete_resource(logical_drive)

        if not status:
            self.set_status_failed()
            return ValidationStatus.FAILED

        self.discovery_container.remove_resource(logical_drive)
        print "MESSAGE::Newly created logical drive deleted"
        self.set_status_passed()
        return ValidationStatus.PASSED

    def _test_case_get_deleted_logical_drive(self, logical_drive=None):
        print "TEST_CASE::Get the deleted logical drive"

        if not logical_drive:
            logical_drive = self.created_logical_drive
        status, status_code, response_body, _ = self.api_caller.get_resource(logical_drive,
            acceptable_return_codes = [ReturnCodes.NOT_FOUND])

        if not status:
            print "ERROR::Wrong status code of GET response after deletion: %s" % status_code
            print "ERROR::Response body:\n%s" % response_body
            self.set_status_failed()
            return ValidationStatus.FAILED

        print "MESSAGE::Newly created logical drive not found (as intended)"
        self.set_status_passed()
        return ValidationStatus.PASSED


    def crud_logical_drive(self, called_from_crud_remote_target=None):
        """
        Test is trying to perform CRUD (create, read, update, delete) operations on a logical volume
        :type discovery_container: cts_core.discovery.discovery_container.DiscoveryContainer
        :type self.metadata_container: cts_core.metadata.self.metadata_container.MetadataContainer
        """

        status = ValidationStatus.BLOCKED

        lvm_found = False
        logical_volumes = dict()
        logical_volume_groups = dict()
        logical_drive_collections = dict()

        for resource_link, resource in self.discovery_container.iteritems():
            ok = True
            if self.metadata_container.entities[MetadataConstants.LOGICAL_DRIVE].compare_entities(resource.odata_type):
                print "DEBUG::found entity of type %s in %s" % (MetadataConstants.LOGICAL_DRIVE, resource_link)
                try:
                    if resource.body["Type"] == "LVM":
                        lvm_found = True
                        if resource.body["Mode"] == "LVG":
                            logical_volume_groups[resource_link] = resource
                        elif resource.body["Mode"] == "LV":
                            if "CapacityGiB" not in resource.body.keys():
                                print "DEBUG::%s drive does not have specified capacity" % resource_link
                                ok = False
                            if "Snapshot" in resource.body.keys() and resource.body["Snapshot"] == True:
                                print "DEBUG::%s cannot be used - drive is a snapshot volume" % resource_link
                                ok = False
                            if ok:
                                logical_volumes[resource_link] = resource
                    elif resource.body["Type"] == "CEPH":
                        if resource.body["Mode"] == "Pool":
                            logical_volume_groups[resource_link] = resource
                        elif resource.body["Mode"] == "RBD":
                            if "CapacityGiB" not in resource.body.keys():
                                print "DEBUG::%s drive does not have specified capacity" % resource_link
                                ok = False
                            if ok:
                                logical_volumes[resource_link] = resource
                except:
                    print "WARNING::Incorrect resource %s structure" % resource_link
            elif self.metadata_container.entities[MetadataConstants.LOGICAL_DRIVE_COLLECTION]\
                 .compare_entities(resource.odata_type):
                logical_drive_collections[resource_link] = resource

        if lvm_found and (not logical_volume_groups or not logical_volumes):
            print "ERROR::Insufficient resources available on API for test. Found %s logical volumes and %s logical " \
                  " volume groups" % (len(logical_volumes), len(logical_volume_groups))
            return ValidationStatus.BLOCKED

        # to speed things up, always look for the lowest capacity logical volume
        lowest_capacity = float('inf')
        chosen_logical_drive_collection, chosen_logical_volume, chosen_logical_volume_group = None, None, None
        inherited_bootable = None

        if not logical_volume_groups:
            if lvm_found:
                print "ERROR::No LVM volume group found, aborting"
            else:
                print "ERROR::No CEPH Pool device found, aborting"
            return ValidationStatus.BLOCKED

        for lvg_link, lvg in logical_volume_groups.iteritems():
            # finding a collection matching this volume group
            possible_collections = [collection for collection in logical_drive_collections.keys() if collection in lvg_link]
            if not possible_collections:
                continue
            try:
                for lv in lvg.body["Links"]["LogicalDrives"]: # iterating LVs that have this LVG as a parent
                    try:
                        lv_link = lv["@odata.id"]
                        lv_type = logical_volumes[lv_link].body["Type"]
                        lv_mode = logical_volumes[lv_link].body["Mode"]
                        lv_capacity = logical_volumes[lv_link].body["CapacityGiB"]
                        lv_bootable = logical_volumes[lv_link].body["Bootable"]
                        if lv_capacity < lowest_capacity:
                            lowest_capacity = lv_capacity
                            inherited_bootable = lv_bootable
                            chosen_logical_volume = lv_link
                            chosen_logical_volume_type = lv_type
                            chosen_logical_volume_mode = lv_mode
                            chosen_logical_volume_group = lvg_link
                            chosen_logical_drive_collection = possible_collections[0]
                    except KeyError:
                        pass
            except KeyError:
                pass

        self.post_logical_drive_payload = dict(
            Name="LVM Logical Drive created by CTS",
            Type=chosen_logical_volume_type,
            Mode=chosen_logical_volume_mode,
            Protected=False,
            CapacityGiB=self.discovery_container[chosen_logical_volume].body["CapacityGiB"],
            Bootable=inherited_bootable if inherited_bootable else False,
            Snapshot=True,
            Links=dict(
                LogicalDrives=[{"@odata.id": chosen_logical_volume_group}],
                MasterDrive={"@odata.id": chosen_logical_volume}
            )
        )

        # Creating the first RBD if no other was found
        if not chosen_logical_volume:
            if lvm_found:
                print "ERROR::Could not find a (logical volume, logical volume group) pair suitable for the test"
                return status
            else:
                self.post_logical_drive_payload = dict(
                    Name="CEPH Logical Drive created by CTS",
                    Type="CEPH",
                    Mode="RBD",
                    Protected=False,
                    CapacityGiB=self.discovery_container[chosen_logical_volume_group].body["CapacityGiB"]/2,
                    Bootable=False,
                    Snapshot=False,
                    Links=dict(
                        LogicalDrives=[{"@odata.id": chosen_logical_volume_group}],
                        MasterDrive=None
                    )
                )


        print "MESSAGE::Logical drive CRUD test will be performed on logical drive collection %s, creating logical volume " \
              "based on %s on group %s" % (chosen_logical_drive_collection, chosen_logical_volume, chosen_logical_volume_group)

        self.expected_created_logical_drive_body = deepcopy(self.post_logical_drive_payload)
        # The logical drive in LogicalDrives link is really the volume group and is going to be seen under
        # UsedBy on REST
        self.expected_created_logical_drive_body["Links"]["UsedBy"] = \
            self.expected_created_logical_drive_body["Links"]["LogicalDrives"]
        self.expected_created_logical_drive_body["Links"]["LogicalDrives"] = []

        status = self._test_case_create_logical_drive(chosen_logical_drive_collection)
        if status == ValidationStatus.PASSED: # only perform other tests if creation was successful
            status = ValidationStatus.join_statuses(status, self._test_case_get_created_logical_drive())
            if called_from_crud_remote_target:
                status = ValidationStatus.join_statuses(status,
                         self.crud_remote_target(called_from_crud_logical_drive=True))
            else:
                status = ValidationStatus.join_statuses(status, self._test_case_update_created_logical_drive())
            status = ValidationStatus.join_statuses(status, self._test_case_delete_created_logical_drive())
            status = ValidationStatus.join_statuses(status, self._test_case_get_deleted_logical_drive())

        return status


    def _test_case_create_remote_target(self, remote_target_collection):
        print "TEST_CASE::Create a remote target"

        status, status_code, response_body, headers = self.api_caller.post_resource(
            remote_target_collection, payload=self.post_remote_target_payload)

        if not status:
            print "ERROR::Wrong status code of POST response: %d" % status_code
            self.set_status_failed()
            return ValidationStatus.FAILED

        try:
            self.created_remote_target = headers["Location"]
            print "MESSAGE::Newly created remote target url %s" % self.created_remote_target
        except KeyError:
            print "ERROR::In header shall be provided location to created resource"
            self.set_status_failed()
            return ValidationStatus.FAILED

        print "MESSAGE::Remote target created"
        self.set_status_passed()
        return ValidationStatus.PASSED

    def _test_case_get_created_remote_target(self):
        print "TEST_CASE::Get the created remote target"

        status, status_code, response_body, _ = self.api_caller.get_resource(self.created_remote_target)

        if not status:
            print "ERROR::Wrong status code of GET response: %s" % status_code
            print "ERROR::Response body:\n%s" % response_body
            self.set_status_failed()
            return ValidationStatus.FAILED
        else:
            print "MESSAGE::Comparing newly created remote target to given payload"
            if not JsonComparator.compare_json_to_golden(response_body, self.post_remote_target_payload):
                print "ERROR::Newly created remote target's body incorrect"
                self.set_status_failed()
                print "MESSAGE::Proceeding with deleting the resource"
                return ValidationStatus.FAILED

        print "MESSAGE::Newly created remote target's correct"
        self.set_status_passed()
        return ValidationStatus.PASSED

    def _test_case_logical_volume_has_link_to_created_target(self, logical_volume):
        print "TEST_CASE::Check for a link to the created target in logical volume's body"

        status, status_code, response_body, _ = self.api_caller.get_resource(logical_volume)

        if not status:
            print "ERROR::Wrong status code of GET response: %s" % status_code
            print "ERROR::Response body:\n%s" % response_body
            self.set_status_failed()
            return ValidationStatus.FAILED
        else:
            try:
                target = urlparse(self.created_remote_target).path
                if not {"@odata.id": target} in response_body["Links"]["Targets"]\
                   and not {"@odata.id": target + "/"} in response_body["Links"]["Targets"]:
                    print "ERROR::No link to the created target in the logical volume's body"
                    self.set_status_failed()
                    return ValidationStatus.FAILED
            except KeyError:
                    print "ERROR::No Links/Targets array present in the logical volume's body"
                    self.set_status_failed()
                    return ValidationStatus.FAILED

        print "MESSAGE::A link from the logical volume to the target is present"
        self.set_status_passed()
        return ValidationStatus.PASSED

    def _test_case_update_created_remote_target(self):
        print "TEST_CASE::Update the created remote target"

        patch_remote_target_payload = dict(
            Initiator=[dict(
                iSCSI=dict(
                    InitiatorIQN="cts.initiator:patched_cts_initiator"
                )
            )]
        )

        status, status_code, response_body, _ = self.api_caller.patch_resource(self.created_remote_target,
                                                                               payload=patch_remote_target_payload)

        if not status:
            print "ERROR::Wrong status code of PATCH reponse %s" % status_code
            print "ERROR::Response body:\n%s" % response_body
            self.set_status_failed()
            return ValidationStatus.FAILED

        status, status_code, response_body, _ = self.api_caller.get_resource(self.created_remote_target)

        if not status:
            print "ERROR::Wrong status code of GET reponse %s" % status_code
            print "ERROR::Response body:\n%s" % response_body
            self.set_status_failed()
            return ValidationStatus.FAILED
        else:
            print "MESSAGE::Verifying whether the remote target's update has taken place"
            if not JsonComparator.compare_json_to_golden(response_body, patch_remote_target_payload):
                print "ERROR::Newly created remote target could not be updated"
                self.set_status_failed()
                print "MESSAGE::Proceeding with deleting the resource"
                return ValidationStatus.FAILED

        print "MESSAGE::Newly created remote target updated correctly"
        self.set_status_passed()
        return ValidationStatus.PASSED

    def _test_case_delete_created_remote_target(self):
        print "TEST_CASE::Delete the created remote target"

        status, status_code, response_body, _ = self._delete_resource(self.created_remote_target)

        if not status:
            self.set_status_failed()
            return ValidationStatus.FAILED

        print "MESSAGE::Newly created remote target deleted"
        self.set_status_passed()
        return ValidationStatus.PASSED

    def _test_case_get_deleted_remote_target(self):
        print "TEST_CASE::Get the deleted remote target"

        status, status_code, response_body, _ = self.api_caller.get_resource(self.created_remote_target,
            acceptable_return_codes = [ReturnCodes.NOT_FOUND])

        if not status:
            print "ERROR::Wrong status code of GET response after deletion: %s" % status_code
            self.set_status_failed()
            return ValidationStatus.FAILED

        print "MESSAGE::Newly created remote target not found (as intended)"
        self.set_status_passed()
        return ValidationStatus.PASSED

    def _delete_resource(self, resource_link):
        status, status_code, response_body, headers = self.api_caller.delete_resource(resource_link)

        if not status:
            print "ERROR::Wrong status code of DELETE reponse: %s" % status_code
            print "ERROR::Response body:\n%s" % response_body

        return status, status_code, response_body, headers

    def crud_remote_target(self, called_from_crud_logical_drive=None):
        """
        Test is checking for rsa compute blade presence
        :type discovery_container: cts_core.discovery.discovery_container.DiscoveryContainer
        :type self.metadata_container: cts_core.metadata.self.metadata_container.MetadataContainer
        """
        target_iqn = "cts.target:cts_test_target"
        target_iqns = set()

        logical_volumes = dict()
        remote_target_collections = dict()

        for resource_link, resource in self.discovery_container.iteritems():
            if self.metadata_container.entities[MetadataConstants.LOGICAL_DRIVE].compare_entities(resource.odata_type):
                print "DEBUG::found entity of type %s in %s" % (MetadataConstants.LOGICAL_DRIVE, resource_link)
                try:
                    # looking for unprotected LVs with no targets associated with it
                    if ((resource.body["Mode"] == "LV" or resource.body["Mode"] == "RBD")
                        and not len(resource.body["Links"]["Targets"]) and not resource.body["Protected"]):
                        logical_volumes[resource_link] = resource
                except:
                    print "WARNING::Incorrect resource %s structure" % resource_link
            elif self.metadata_container.entities[MetadataConstants.REMOTE_TARGET_COLLECTION].compare_entities(resource.odata_type):
                remote_target_collections[resource_link] = resource
            elif self.metadata_container.entities[MetadataConstants.REMOTE_TARGET].compare_entities(
                    resource.odata_type):
                try:
                    for address in resource.body["Addresses"]:
                        target_iqns.add(address["iSCSI"]["TargetIQN"])
                except KeyError:
                    pass

        if target_iqn in target_iqns:
            iqn_number = 0
            while target_iqn + str(iqn_number) in target_iqns:
                iqn_number += 1
            target_iqn = target_iqn + str(iqn_number)

        if not logical_volumes:
            print "WARNING::Insufficient resources available on API for test. Could not find an unprotected logical volume with"\
                  " no targets attached"
            if called_from_crud_logical_drive:
                print "ERROR::Creating a new logical volume for the target failed, skipping remote target tests"
                return ValidationStatus.BLOCKED
            else:
                print "MESSAGE::Trying to create a new logical volume for the target, then proceeding with remote target CRUD test"
                return self.crud_logical_drive(called_from_crud_remote_target=True)

        chosen_logical_volume = logical_volumes.keys()[0]
        chosen_remote_target_collection = remote_target_collections.keys()[0]

        print "MESSAGE::Remote Target CRUD test will be performed on remote target collection %s, creating remote target " \
              "based on %s" % (chosen_remote_target_collection, chosen_logical_volume)

        self.post_remote_target_payload = dict(
            Addresses=[dict(
                iSCSI=dict(
                    TargetIQN=target_iqn,
                    TargetLUN=[dict(
                        LUN=1,
                        LogicalDrive={"@odata.id": chosen_logical_volume}
                    )],
                )
            )],
            Initiator=[dict(
                iSCSI=dict(
                    InitiatorIQN="cts.initiator:initiator_cts_test"
                )
            )]
        )

        status = self._test_case_create_remote_target(chosen_remote_target_collection)
        if status == ValidationStatus.PASSED: # only perform other tests if creation was successful
            status = ValidationStatus.join_statuses(status, self._test_case_get_created_remote_target())
            status = ValidationStatus.join_statuses(status, self._test_case_logical_volume_has_link_to_created_target(
                chosen_logical_volume))
            status = ValidationStatus.join_statuses(status, self._test_case_update_created_remote_target())
            status = ValidationStatus.join_statuses(status, self._test_case_delete_created_remote_target())
            status = ValidationStatus.join_statuses(status, self._test_case_get_deleted_remote_target())

        return status

CRUDOperations.service_cli()
