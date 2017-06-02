#!/usr/bin/env python
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
from cts_core.commons.api_caller import ApiCaller
from cts_core.commons.json_comparator import JsonComparator
from cts_core.commons.preconditions import Preconditions, Requirement
from cts_core.commons.services import RegisterService
from cts_core.commons.services import ServiceTypes
from cts_core.discovery.api_explorer import ApiExplorer
from cts_core.metadata.metadata_constants import MetadataConstants2_1 as MetadataConstants
from cts_core.validation.validation_status import ValidationStatus
from cts_framework.commons.enums import ReturnCodes
from cts_framework.helpers.python.cts_test_script import CtsTestScript
from cts_framework.helpers.python.result_status import ResultStatus


@RegisterService(ServiceTypes.PSME_2_1)
class CRUDOperations(CtsTestScript):
    TIMEOUT = 600
    DESCRIPTION = """test script validating CRUD (create, read, update and delete) functionalities"""

    def run(self):
        print "TEST_CASE::API crawling"
        api_explorer = ApiExplorer(self.metadata_container, self.configuration)

        discovery_container, status = api_explorer.discover(MetadataConstants.SERVICE_ROOT_URI,
                                                            MetadataConstants.SERVICE_ROOT)
        print "STATUS::{status}".format(status=status)

        requirements = [
            Requirement(MetadataConstants.SERVICE_ROOT, min=1, max=1),
            Requirement(MetadataConstants.ETHERNET_SWITCH, min=1),
            Requirement(MetadataConstants.ETHERNET_SWITCH_PORT, min=1)
        ]
        preconditions = Preconditions(self.metadata_container, requirements)
        status = preconditions.validate(discovery_container)

        self.api_caller = ApiCaller(self.configuration)

        self.crud_vlan(discovery_container)


    def _test_case_create_vlan(self, vlan_network_interface_collection):
        print "TEST_CASE::Create VLAN"

        status, status_code, response_body, headers = self.api_caller.post_resource(
            vlan_network_interface_collection, payload=self.post_vlan_payload)

        if not status:
            print "ERROR::Wrong status code of POST response: %s" % status_code
            print "ERROR::Response body:\n%s" % response_body
            self.set_status_failed()
            return False

        try:
            self.created_vlan_network_interface = headers["Location"]
            print "MESSAGE::newly created vlan network interface url %s" % self.created_vlan_network_interface
        except KeyError:
            print "ERROR::in header shall be provided location to created resource"
            self.set_status_failed()
            return False

        print "MESSAGE::vlan network interface created"
        self.set_status_passed()
        return True


    def _test_case_get_created_vlan(self):
        print "TEST_CASE::Get created VLAN"

        status, status_code, response_body, _ = self.api_caller.get_resource(self.created_vlan_network_interface)

        if not status:
            print "ERROR::Wrong status code of GET response: %s" % status_code
            print "ERROR::Response body:\n%s" % response_body
            self.set_status_failed()
            return False
        else:
            print "MESSAGE::Comparing newly created vlan network interface to given payload"
            #  PSME is unable to set "Name" and "VLANEnable" fields properly
            if not JsonComparator.compare_json_to_golden(response_body, self.post_vlan_payload,
                                                         ignore = ["/Name", "/VLANEnable"]):
                print "ERROR::newly created vlan network interface's body incorrect"
                self.set_status_failed()
                print "MESSAGE::proceeding with deleting the resource"
                return False

        print "MESSAGE::newly created vlan network interface correct"
        self.set_status_passed()
        return True


    def _test_case_delete_created_vlan(self):
        print "TEST_CASE::Delete created VLAN"

        status, status_code, response_body, _ = self.api_caller.delete_resource(self.created_vlan_network_interface)

        if not status:
            print "ERROR::Wrong status code of DELETE reponse %s" % status_code
            print "ERROR::Response body:\n%s" % response_body
            self.set_status_failed()
            return False

        print "MESSAGE::newly created vlan network interface deleted"
        self.set_status_passed()
        return True


    def _test_case_get_deleted_vlan(self):
        print "TEST_CASE::Get deleted VLAN"

        status, status_code, response_body, _ = self.api_caller.get_resource(self.created_vlan_network_interface,
            acceptable_return_codes = [ReturnCodes.NOT_FOUND])

        if not status:
            print "ERROR::Wrong status code of GET response after deletion: %s" % status_code
            print "ERROR::Response body:\n%s" % response_body
            self.set_status_failed()
            return False

        print "MESSAGE::newly created vlan network interface not found (as intended)"
        self.set_status_passed()
        return True


    def crud_vlan(self, discovery_container):
        """
        Test is trying to perform CRUD (create, read, update, delete) operations on a VLAN Network Interface resource
        :type discovery_container: cts_core.discovery.discovery_container.DiscoveryContainer
        """

        vlan_network_interface_collections = dict()

        for resource_link, resource in discovery_container.iteritems():
            if self.metadata_container.entities[MetadataConstants.VLAN_NETWORK_INTERFACE_COLLECTION].compare_entities(resource.odata_type):
                print "DEBUG::found entity of type %s in %s" % (MetadataConstants.VLAN_NETWORK_INTERFACE_COLLECTION,
                                                                resource_link)
                vlan_network_interface_collections[resource_link] = resource

        if not vlan_network_interface_collections:
            print "ERROR::Insufficient resources available on API for test. Found %s network interface collections" % \
                  len(vlan_network_interface_collections)

            return ValidationStatus.BLOCKED

        chosen_vlan_network_interface_collection = vlan_network_interface_collections.keys()[0]

        print "MESSAGE::test will be performed on vlan network interface collection %s" % chosen_vlan_network_interface_collection

        present_vlan_ids = []

        for resource_link, resource in discovery_container.iteritems():
            if self.metadata_container.entities[MetadataConstants.VLAN_NETWORK_INTERFACE].compare_entities(resource.odata_type) \
            and chosen_vlan_network_interface_collection in resource_link:
                print "DEBUG::found entity of type %s in %s" % (MetadataConstants.VLAN_NETWORK_INTERFACE, resource_link)
                try:
                    present_vlan_ids.append(resource.body["VLANId"])
                except:
                    print "WARNING::Incorrect resource %s structure" % resource_link

        new_vlan_id = 0

        for possible_id in range(1, 4096): # VLANId is a value of 1 - 4095
            if possible_id not in present_vlan_ids:
                new_vlan_id = possible_id
                break

        self.post_vlan_payload = dict(
            Name=unicode("CTS_CRUD_TEST_VLAN"),
            VLANId=new_vlan_id,
            VLANEnable=True,
            Oem=dict(
                Intel_RackScale=dict(
                    Tagged=True
                )
            )
        )

        if self._test_case_create_vlan(chosen_vlan_network_interface_collection):
            self._test_case_get_created_vlan()
            self._test_case_delete_created_vlan()
            self._test_case_get_deleted_vlan()

CRUDOperations.service_cli()
