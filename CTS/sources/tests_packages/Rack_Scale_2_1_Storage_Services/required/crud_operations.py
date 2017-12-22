#!/usr/bin/env python
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
from copy import deepcopy
from urlparse import urlparse

from cts_core.commons.api_caller import ApiCaller
from cts_core.commons.error import cts_error, cts_warning
from cts_core.commons.json_comparator import JsonComparator
from cts_core.commons.preconditions import Preconditions, Requirement
from cts_core.commons.services import RegisterService
from cts_core.commons.services import ServiceTypes
from cts_core.discovery.api_explorer import ApiExplorer
from cts_core.discovery.discovery_container import equal, not_equal, from_collection, greater, \
    urls, order_by, odata_ids
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
        if self.metadata_container is None:
            return

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
            Requirement(MetadataConstants.LOGICAL_DRIVE_COLLECTION, min=1),
            Requirement(MetadataConstants.REMOTE_TARGET_COLLECTION, min=1),
            Requirement(MetadataConstants.LOGICAL_DRIVE, min=2)
        ]
        preconditions = Preconditions(self.metadata_container, requirements)
        status = preconditions.validate(self.discovery_container)

        if  status == ValidationStatus.FAILED:
            self.set_status_blocked()
            return

        self.api_caller = ApiCaller(self.configuration)

        status = ValidationStatus.join_statuses(status, self.crud_logical_drive())
        status = ValidationStatus.join_statuses(status, self.crud_remote_target())

        print "MESSAGE::%s overall status: %s" % (test_name, ColorPrinter.format_status(status))



    def _test_case_create_logical_drive(self, logical_drive_collection, payload=None):
        print "TEST_CASE::Create a logical drive"

        if not payload:
            payload = self.post_logical_drive_payload
        status, status_code, response_body, headers = self.api_caller.post_resource(
            logical_drive_collection, self.discovery_container, payload=payload)

        if not status:
            cts_error("Wrong status code of POST response: {status_code}; response body: {response_body:response_body}",
                      **locals())
            self.set_status_failed()
            return ValidationStatus.FAILED

        try:
            self.created_logical_drive = headers["Location"]
            self.created_logical_drive_netloc = \
                self.discovery_container.get_netloc(logical_drive_collection)
            print "MESSAGE::Newly created logical drive url %s" % self.created_logical_drive
        except KeyError:
            cts_error("In header shall be provided location to created resource")
            self.set_status_failed()
            return ValidationStatus.FAILED

        print "MESSAGE::Logical drive created"
        self.set_status_passed()
        return ValidationStatus.PASSED

    def _test_case_get_created_logical_drive(self, logical_drive):
        print "TEST_CASE::Get the created logical drive"

        link, status, status_code, response_body, _ = \
            self.api_caller.get_resource(self.created_logical_drive,
                                         self.discovery_container,
                                         api_endpoint_override=self.created_logical_drive_netloc)

        if not status:
            cts_error("Wrong status code of GET response: {status_code}; response body: {response_body:response_body}",
                      **locals())
            self.set_status_failed()
            return ValidationStatus.FAILED
        else:
            print "MESSAGE::Comparing newly created logical drive to given payload"
            if not JsonComparator.compare_json_to_golden(response_body, self.expected_created_logical_drive_body,
                                                         ignore = ["/Name"]): # Name setting not supported by PSME REST API
                cts_error("Newly created logical drive's body incorrect")
                self.set_status_failed()
                print "MESSAGE::Proceeding with deleting the resource"
                return ValidationStatus.FAILED

        print "MESSAGE::Newly created logical drive's correct"
        self.set_status_passed()
        return ValidationStatus.PASSED

    def _test_case_update_created_logical_drive(self):
        print "TEST_CASE::Update the created logical drive"

        patch_logical_drive_payload = dict(
            Bootable = not self.post_logical_drive_payload["Bootable"]
        )

        status, status_code, response_body, _ = self.api_caller.patch_resource(self.created_logical_drive,
                                                                               self.discovery_container,
                                                                               payload=patch_logical_drive_payload)

        if not status:
            cts_error("Error while executing PATCH; status code: {status_code}; response body: "
                      "{response_body:response_body}",
                      **locals())
            self.set_status_failed()
            return ValidationStatus.FAILED

        link, status, status_code, response_body, _ = \
            self.api_caller.get_resource(self.created_logical_drive,
                                         self.discovery_container,
                                         api_endpoint_override=self.created_logical_drive_netloc)

        if not status:
            cts_error("Wrong status code of GET response: {status_code}; response body: {response_body:response_body}",
                      **locals())
            self.set_status_failed()
            return ValidationStatus.FAILED
        else:
            print "MESSAGE::Verifying whether the logical drive's update has taken place"
            if not JsonComparator.compare_json_to_golden(response_body["Bootable"],
                                                         patch_logical_drive_payload["Bootable"]):
                cts_error("Newly created logical drive could not be updated")
                self.set_status_failed()
                print "MESSAGE::Proceeding with deleting the resource"
                return ValidationStatus.FAILED

        print "MESSAGE::Newly created logical drive updated correctly"
        self.set_status_passed()
        return ValidationStatus.PASSED

    def _test_case_delete_created_logical_drive(self, logical_drive):
        print "TEST_CASE::Delete the created logical drive"

        status, status_code, response_body, _ = self.api_caller.delete_resource(self.created_logical_drive,
                                                                                self.discovery_container)

        if not status:
            cts_error("Wrong status code of DELETE response: {status_code}; response body: "
                      "{response_body:response_body}", **locals())
            self.set_status_failed()
            return ValidationStatus.FAILED

        print "MESSAGE::Newly created logical drive deleted"
        self.set_status_passed()
        return ValidationStatus.PASSED

    def _test_case_get_deleted_logical_drive(self, logical_drive):
        print "TEST_CASE::Get the deleted logical drive"

        link, status, status_code, response_body, _ = \
            self.api_caller.get_resource(self.created_logical_drive,
                                         self.discovery_container,
                                         acceptable_return_codes=[ReturnCodes.NOT_FOUND],
                                         api_endpoint_override=self.created_logical_drive_netloc)

        if not status:
            cts_error("Wrong status code of GET response after deletion: {status_code}; response body: "
                      "{response_body:response_body}", **locals())
            self.set_status_failed()
            return ValidationStatus.FAILED

        print "MESSAGE::Newly created logical drive not found (as intended)"
        self.set_status_passed()
        return ValidationStatus.PASSED


    def crud_logical_drive(self, create_logical_drive_for_target_crud=None):
        """
        Test is trying to perform CRUD (create, read, update, delete) operations on a logical drive
        :type create_logical_drive_for_target_crud: Boolean
        """

        status = ValidationStatus.BLOCKED

        lvm_found = False

        chosen_logical_drive_type, chosen_logical_drive_mode = None, None
        chosen_logical_drive_collection, chosen_logical_drive, chosen_logical_drive_group = None, None, None
        inherited_bootable = None

        # to speed things up, always look for the lowest capacity logical drive
        LVM_LV_SPECIFIC_CONSTRAINTS = [equal("Mode", "LV"), greater("CapacityGiB", 0), not_equal("Snapshot", True),
                                       order_by("CapacityGiB")]
        CEPH_RBD_SPECIFIC_CONSTRAINTS = [equal("Mode", "RBD"), greater("CapacityGiB", 0), order_by("CapacityGiB")]

        logical_drive_collections = dict(self.discovery_container.get_resources(
                                         MetadataConstants.LOGICAL_DRIVE_COLLECTION,
                                         any_child_version=True))

        for collection in logical_drive_collections.keys():
            chosen_logical_drive_collection = collection
            logical_drive_groups = dict(self.discovery_container.get_resources(MetadataConstants.LOGICAL_DRIVE,
                                                                      any_child_version=True,
                                                                      constraints=[equal("Mode", "LVG"),
                                                                                   from_collection(collection)]))
            if logical_drive_groups:
                lvm_found = True
            else:
                logical_drive_groups = dict(self.discovery_container.get_resources(MetadataConstants.LOGICAL_DRIVE,
                                                                                    any_child_version=True,
                                                                                    constraints=[equal("Mode", "Pool"),
                                                                                                 from_collection(
                                                                                                     collection)]))
            if logical_drive_groups:
                specific_constraints = LVM_LV_SPECIFIC_CONSTRAINTS if lvm_found else CEPH_RBD_SPECIFIC_CONSTRAINTS

                for lvg_link, lvg in logical_drive_groups.iteritems():
                    chosen_logical_drive_group = lvg.odata_id
                    try:
                        child_lvs = [lv["@odata.id"] for lv in lvg.body["Links"]["LogicalDrives"]]
                        chosen_logical_drive, _ = self.discovery_container.get_resources(
                                                                                       MetadataConstants.LOGICAL_DRIVE,
                                                                                       any_child_version=True,
                                                                                       constraints=specific_constraints
                                                                                       + [odata_ids(child_lvs)])[0]
                        lv = self.discovery_container[chosen_logical_drive]
                        inherited_bootable = lv.body["Bootable"]
                        chosen_logical_drive_type = lv.body["Type"]
                        chosen_logical_drive_mode = lv.body["Mode"]
                    except (KeyError,IndexError) as error:
                        cts_error("Exception while parsing {id:id}: {error:exception}", id=chosen_logical_drive,
                                                                                        error=error)
                        chosen_logical_drive = None
                        pass
                    else:
                        break
            if not chosen_logical_drive_group:
                resource = "logical drive group" if lvm_found else "Pool"
                cts_warning("Could not find a {resource} resource in collection {collection}", **locals())
            if lvm_found and not chosen_logical_drive:
                cts_warning("Could not find a logical volume in collection {collection}", **locals())

        if lvm_found and not chosen_logical_drive:
            cts_error("Could not find any logical volume")
            return status

        if not chosen_logical_drive_group:
            cts_error("Could not find any logical drive group, aborting")
            return status

        self.post_logical_drive_payload = dict(
            Name="LVM Logical Drive created by CTS",
            Type=chosen_logical_drive_type,
            Mode=chosen_logical_drive_mode,
            Protected=False,
            CapacityGiB=self.discovery_container[chosen_logical_drive].body["CapacityGiB"],
            Image="CTS test image",
            Bootable=inherited_bootable if inherited_bootable else False,
            Snapshot=True,
            Links=dict(
                LogicalDrives=[{"@odata.id": chosen_logical_drive_group}],
                MasterDrive={"@odata.id": urlparse(chosen_logical_drive).path}
            )
        )

        # Creating the first RBD if no other was found
        if not chosen_logical_drive:
            if lvm_found:
                print "ERROR::Could not find a logical drive suitable for snapshot"
                return status
            cts_warning("Could not find any logical drive suitable for snapshot. Since CEPH was detected, CTS will "\
                        "try to create the first RBD")
            self.post_logical_drive_payload = dict(
                Name="CEPH Logical Drive created by CTS",
                Type="CEPH",
                Mode="RBD",
                Protected=False,
                CapacityGiB=self.discovery_container[chosen_logical_drive_group].body["CapacityGiB"]/2,
                Bootable=False,
                Snapshot=False,
                Links=dict(
                    LogicalDrives=[{"@odata.id": urlparse(chosen_logical_drive_group).path}],
                    MasterDrive=None
                )
            )

        print "MESSAGE::Logical drive CRUD test will be performed on logical drive collection %s, creating logical " \
              "drive based on %s on group %s" % (chosen_logical_drive_collection, chosen_logical_drive,
                                                 chosen_logical_drive_group)

        self.expected_created_logical_drive_body = deepcopy(self.post_logical_drive_payload)
        # The logical drive in LogicalDrives link is really the volume group and is going to be seen under
        # UsedBy on REST
        self.expected_created_logical_drive_body["Links"]["UsedBy"] = \
            self.expected_created_logical_drive_body["Links"]["LogicalDrives"]
        self.expected_created_logical_drive_body["Links"]["LogicalDrives"] = []

        status = self._test_case_create_logical_drive(chosen_logical_drive_collection)
        if status == ValidationStatus.PASSED: # only perform other tests if creation was successful
            ld = self.created_logical_drive
            status = ValidationStatus.join_statuses(status, self._test_case_get_created_logical_drive(ld))
            if create_logical_drive_for_target_crud:
                return status
            if status == ValidationStatus.PASSED:
                status = ValidationStatus.join_statuses(status, self._test_case_update_created_logical_drive())
            status = ValidationStatus.join_statuses(status, self._test_case_delete_created_logical_drive(ld))
            status = ValidationStatus.join_statuses(status, self._test_case_get_deleted_logical_drive(ld))

        return status


    def _test_case_create_remote_target(self, remote_target_collection):
        print "TEST_CASE::Create a remote target"

        status, status_code, response_body, headers = self.api_caller.post_resource(
            remote_target_collection, self.discovery_container, payload=self.post_remote_target_payload)

        if not status:
            cts_error("Wrong status code of POST response: {status_code}; response body: {response_body:response_body}",
                      **locals())
            self.set_status_failed()
            return ValidationStatus.FAILED

        try:
            self.created_remote_target = headers["Location"]
            self.created_remote_target_netloc = \
                self.discovery_container.get_netloc(remote_target_collection)
            print "MESSAGE::Newly created remote target url %s" % self.created_remote_target
        except KeyError:
            cts_error("In header shall be provided location to created resource")
            self.set_status_failed()
            return ValidationStatus.FAILED

        print "MESSAGE::Remote target created"
        self.set_status_passed()
        return ValidationStatus.PASSED

    def _test_case_get_created_remote_target(self):
        print "TEST_CASE::Get the created remote target"

        link, status, status_code, response_body, _ = \
            self.api_caller.get_resource(self.created_remote_target,
                                         self.discovery_container,
                                         api_endpoint_override=self.created_remote_target_netloc)

        if not status:
            cts_error("Wrong status code of GET response: {status_code}; response body: {response_body:response_body}",
                      **locals())
            self.set_status_failed()
            return ValidationStatus.FAILED
        else:
            print "MESSAGE::Comparing newly created remote target to given payload"
            if not JsonComparator.compare_json_to_golden(response_body, self.post_remote_target_payload):
                cts_error("Newly created remote target's body incorrect")
                self.set_status_failed()
                print "MESSAGE::Proceeding with deleting the resource"
                return ValidationStatus.FAILED

        print "MESSAGE::Newly created remote target's body correct"
        self.set_status_passed()
        return ValidationStatus.PASSED

    def _test_case_logical_drive_has_link_to_created_target(self, logical_drive):
        print "TEST_CASE::Check for a link to the created target in logical drive's body"

        link, status, status_code, response_body, _ = \
            self.api_caller.get_resource(logical_drive,
                                         self.discovery_container)

        if not status:
            cts_error("Wrong status code of GET response: {status_code}; response body: {response_body:response_body}",
                      **locals())
            self.set_status_failed()
            return ValidationStatus.FAILED
        else:
            try:
                target = urlparse(self.created_remote_target).path
                if not {"@odata.id": target} in response_body["Links"]["Targets"]\
                   and not {"@odata.id": target + "/"} in response_body["Links"]["Targets"]:
                    cts_error("No link to the created target in the logical drive's body")
                    self.set_status_failed()
                    return ValidationStatus.FAILED
            except KeyError:
                cts_error("No Links/Targets array present in the logical drive's body")
                self.set_status_failed()
                return ValidationStatus.FAILED

        print "MESSAGE::A link from the logical drive to the target is present"
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
                                                                               self.discovery_container,
                                                                               payload=patch_remote_target_payload)

        if not status:
            cts_error("Error while executing PATCH; status code: {status_code}; response body: "
                      "{response_body:response_body}",
                      **locals())
            self.set_status_failed()
            return ValidationStatus.FAILED

        link, status, status_code, response_body, _ = \
            self.api_caller.get_resource(self.created_remote_target,
                                         self.discovery_container,
                                         api_endpoint_override=self.created_remote_target_netloc)

        if not status:
            cts_error("Wrong status code of GET response: {status_code}; response body: {response_body:response_body}",
                      **locals())
            self.set_status_failed()
            return ValidationStatus.FAILED
        else:
            print "MESSAGE::Verifying whether the remote target's update has taken place"
            if not JsonComparator.compare_json_to_golden(response_body["Initiator"],
                                                         patch_remote_target_payload["Initiator"]):
                cts_error("Newly created remote target could not be updated")
                self.set_status_failed()
                print "MESSAGE::Proceeding with deleting the resource"
                return ValidationStatus.FAILED

        print "MESSAGE::Newly created remote target updated correctly"
        self.set_status_passed()
        return ValidationStatus.PASSED

    def _test_case_delete_created_remote_target(self):
        print "TEST_CASE::Delete the created remote target"

        status, status_code, response_body, _ = self.api_caller.delete_resource(self.created_remote_target,
                                                                                self.discovery_container)

        if not status:
            cts_error("Wrong status code of DELETE response: {status_code}; response body: "
                      "{response_body:response_body}",
                      **locals())
            self.set_status_failed()
            return ValidationStatus.FAILED

        print "MESSAGE::Newly created remote target deleted"
        self.set_status_passed()
        return ValidationStatus.PASSED

    def _test_case_get_deleted_remote_target(self):
        print "TEST_CASE::Get the deleted remote target"

        link, status, status_code, response_body, _ = \
            self.api_caller.get_resource(self.created_remote_target,
                                         self.discovery_container,
                                         acceptable_return_codes=[ReturnCodes.NOT_FOUND],
                                         api_endpoint_override=self.created_remote_target_netloc)

        if not status:
            cts_error("Wrong status code of GET response: {status_code}; response body: {response_body:response_body}",
                      **locals())
            self.set_status_failed()
            return ValidationStatus.FAILED

        print "MESSAGE::Newly created remote target not found (as intended)"
        self.set_status_passed()
        return ValidationStatus.PASSED

    def crud_remote_target(self):
        """
        Test is checking for rsa compute blade presence
        """

        ld_created_for_crud = False
        target_iqn = "cts.target:cts_test_target"
        target_iqns = set()

        chosen_logical_drive, chosen_remote_target_collection = None, None

        if not self.discovery_container.count_resources(MetadataConstants.LOGICAL_DRIVE, any_child_version=True):
            cts_error("Insufficient resources for API test. No logical drives found.")
            return ValidationStatus.BLOCKED

        logical_drives = dict(self.discovery_container.get_resources(MetadataConstants.LOGICAL_DRIVE,
                                                                      any_child_version=True,
                                                                      constraints=[equal("Mode", "LV"),
                                                                                   equal("Protected", False)]))
        if not logical_drives:
            logical_drives = dict(self.discovery_container.get_resources(MetadataConstants.LOGICAL_DRIVE,
                                                                      any_child_version=True,
                                                                      constraints=[equal("Mode", "RBD"),
                                                                                   equal("Protected", False)]))
        try:
            chosen_remote_target_collection = self.discovery_container.get_resources(
                                              MetadataConstants.REMOTE_TARGET_COLLECTION,
                                              any_child_version=True)[0][0]
            chosen_logical_drive = [link for link, resource in logical_drives.iteritems() if not
                                     len(resource.body["Links"]["Targets"])][0]
        except (KeyError, IndexError):
            pass

        if not chosen_remote_target_collection:
            cts_error("Insufficient resources available on API for test. Could not find any remote target collection")
            return ValidationStatus.BLOCKED

        if not chosen_logical_drive:
            cts_warning("Insufficient resources available on API for test. Could not find an unprotected logical drive "
                        "with no targets attached")
            print "MESSAGE::Trying to create a new logical volume for the target, then proceeding with remote target "\
                  "CRUD test"
            if self.crud_logical_drive(create_logical_drive_for_target_crud=True) == ValidationStatus.PASSED:
                chosen_logical_drive = self.created_logical_drive
                ld_created_for_crud = True
            else:
                cts_error("Creating a new logical volume for the target failed, skipping remote target tests")
                return ValidationStatus.BLOCKED

        targets = [link for link, resource in
                   self.discovery_container.get_resources(MetadataConstants.REMOTE_TARGET,
                                                          any_child_version=True,
                                                          constraints=[
                                                              from_collection(chosen_remote_target_collection)
                                                          ])]
        for target in targets:
            addresses = self.discovery_container[target].body["Addresses"]
            for address in addresses:
                try:
                    target_iqns.add(address["iSCSI"]["TargetIQN"])
                except KeyError:
                    pass

        if target_iqn in target_iqns:
            iqn_number = 0
            while target_iqn + str(iqn_number) in target_iqns:
                iqn_number += 1
            target_iqn = target_iqn + str(iqn_number)

        print "MESSAGE::Remote Target CRUD test will be performed on remote target collection %s, creating remote "\
              "target based on %s with first lowest consecutive \'cts.target:cts_test_target<number>\' target IQN "\
              "available: %s" % (chosen_remote_target_collection, chosen_logical_drive, target_iqn)

        self.post_remote_target_payload = dict(
            Addresses=[dict(
                iSCSI=dict(
                    TargetIQN=target_iqn,
                    TargetLUN=[dict(
                        LUN=1,
                        LogicalDrive={"@odata.id": urlparse(chosen_logical_drive).path}
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
            status = ValidationStatus.join_statuses(status, self._test_case_logical_drive_has_link_to_created_target(
                chosen_logical_drive))
            status = ValidationStatus.join_statuses(status, self._test_case_update_created_remote_target())
            status = ValidationStatus.join_statuses(status, self._test_case_delete_created_remote_target())
            status = ValidationStatus.join_statuses(status, self._test_case_get_deleted_remote_target())

        if ld_created_for_crud:
            ld = self.created_logical_drive
            del_status = self._test_case_delete_created_logical_drive(ld)
            del_status = ValidationStatus.join_statuses(del_status, self._test_case_get_deleted_logical_drive(ld))
            if status == ValidationStatus.PASSED and del_status != status:
                cts_error("Remote target CRUD test passed, but the logical drive it created failed to delete properly")
                status = ValidationStatus.PASSED_WITH_WARNINGS

        return status

CRUDOperations.service_cli()
