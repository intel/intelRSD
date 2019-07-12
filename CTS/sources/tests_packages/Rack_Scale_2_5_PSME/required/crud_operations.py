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

from random import choice
from urlparse import urlparse

from cts_core.commons.api_caller import ApiCaller
from cts_core.commons.error import cts_warning, cts_error
from cts_core.commons.json_comparator import JsonComparator
from cts_core.commons.preconditions import Preconditions, Requirement
from cts_core.commons.services import RegisterService
from cts_core.commons.services import ServiceTypes
from cts_core.commons.text_helpers import get_parent_odata_id
from cts_core.discovery.api_explorer import ApiExplorer
from cts_core.discovery.discovery_container import from_collection
from cts_core.metadata.metadata_constants import MetadataConstants2_5 as MetadataConstants
from cts_core.validation.validation_status import ValidationStatus
from cts_framework.commons.enums import ReturnCodes
from cts_framework.helpers.python.cts_test_script import CtsTestScript
from cts_framework.helpers.python.result_status import ResultStatus

MIN_VLAN_ID = 1
MAX_VLAN_ID = 4095


@RegisterService(ServiceTypes.PSME_2_5)
class CRUDOperations(CtsTestScript):
    TIMEOUT = 600
    DESCRIPTION = """test script validating CRUD (create, read, update and delete) functionalities"""

    def run(self):
        if self.metadata_container is None:
            return

        print "TEST_CASE::API crawling"

        api_explorer = ApiExplorer(self.metadata_container, self.configuration)

        self.discovery_container, status = api_explorer.discover(MetadataConstants.SERVICE_ROOT_URI,
                                                                 MetadataConstants.SERVICE_ROOT)
        print "STATUS::{status}".format(status=status)

        requirements = [
            Requirement(MetadataConstants.SERVICE_ROOT, min=1, max=1),
        ]
        preconditions = Preconditions(self.metadata_container, requirements)
        status = preconditions.validate(self.discovery_container)

        self.api_caller = ApiCaller(self.configuration)
        self.crud_vlan()

    def _test_case_create_vlan(self, vlan_network_interface_collection):
        print "TEST_CASE::Create VLAN"

        status, status_code, response_body, headers = \
            self.api_caller.post_resource(
            vlan_network_interface_collection, self.discovery_container, payload=self.post_vlan_payload)

        if not status:
            cts_error("Wrong status code of POST response: {status_code}; response body: {response_body:response_body}",
                      **locals())
            self.set_status_failed()
            return False

        try:
            self.created_vlan_network_interface = headers["Location"]
            self.created_vlan_network_interface_netlock = self.discovery_container.get_netloc(
                vlan_network_interface_collection)
            print "MESSAGE::Newly created VLAN Network Interface URI: %s" % self.created_vlan_network_interface
        except KeyError:
            cts_error("Incorrect response - header shall contain the location of the created resource")
            self.set_status_failed()
            return False

        print "MESSAGE::VLAN Network Interface created"
        self.set_status_passed()
        return True

    def _test_case_get_created_vlan(self):
        print "TEST_CASE::Get created VLAN"

        link, status, status_code, response_body, _ = \
            self.api_caller.get_resource(self.created_vlan_network_interface,
                                         self.discovery_container,
                                         api_endpoint_override=self.created_vlan_network_interface_netlock)

        if not status:
            cts_error("Wrong status code of GET response: {status_code}; response body: {response_body:response_body}",
                      **locals())
            self.set_status_failed()
            return False
        else:
            print "MESSAGE::Comparing newly created VLAN Network Interface to given payload"
            #  PSME is unable to set "Name" and "VLANEnable" fields properly but only "VLANEnable" is mandatory
            if not JsonComparator.compare_json_to_golden(response_body, self.post_vlan_payload,
                                                         ignore=["/VLANEnable"]):
                cts_error("Newly created VLAN Network Interface's body incorrect")
                self.set_status_failed()
                return False

        print "MESSAGE::Newly created VLAN Network Interface correct"
        self.set_status_passed()
        return True

    def _test_case_delete_created_vlan(self):
        print "TEST_CASE::Delete created VLAN"

        status, status_code, response_body, _ = self.api_caller.delete_resource(self.created_vlan_network_interface,
                                                                                self.discovery_container)

        if not status:
            cts_error("Wrong status code of DELETE response: {status_code};"
                      " response body: {response_body:response_body}",
                      **locals())
            self.set_status_failed()
            return False

        print "MESSAGE::Newly created VLAN Network Interface deleted"
        self.set_status_passed()
        return True

    def _test_case_get_deleted_vlan(self):
        print "TEST_CASE::Get deleted VLAN"

        link, status, status_code, response_body, _ = \
            self.api_caller.get_resource(self.created_vlan_network_interface,
                                         self.discovery_container,
                                         acceptable_return_codes=[ReturnCodes.NOT_FOUND],
                                         api_endpoint_override=self.created_vlan_network_interface_netlock)

        if not status:
            cts_error("Wrong status code of GET response after deletion: {status_code};"
                      " response body: {response_body:response_body}",
                      **locals())
            self.set_status_failed()
            return False

        print "MESSAGE::Newly created VLAN Network Interface not found (as intended)"
        self.set_status_passed()
        return True

    def crud_vlan(self):
        """
        Test is trying to perform CRUD (create, read, update, delete) operations on a VLAN Network Interface resource
        """
        requirements = [
            Requirement(MetadataConstants.SERVICE_ROOT, min=1, max=1),
            Requirement(MetadataConstants.VLAN_NETWORK_INTERFACE_COLLECTION, min=1),
        ]
        preconditions = Preconditions(self.metadata_container, requirements)
        status = preconditions.validate(self.discovery_container)

        if status is ValidationStatus.PASSED_WITH_WARNINGS:
            cts_warning("Without some components it is not possible to perform tests.")
            print "STATUS::{status}".format(status=status)
            return ValidationStatus.PASSED_WITH_WARNINGS

        if status is ValidationStatus.FAILED:
            return ValidationStatus.BLOCKED

        vlan_network_interface_collections = \
            dict(self.discovery_container.get_resources(MetadataConstants.VLAN_NETWORK_INTERFACE_COLLECTION,
                 any_child_version=True))

        chosen_vlan_network_interface_collection = None
        new_vlan_id = 0

        for vlan_network_interface_collection in vlan_network_interface_collections:
            try:
                parent_port = get_parent_odata_id(vlan_network_interface_collection)
                parent_port_id = self.discovery_container[parent_port].body["PortId"]
            except:
                continue

            # For arista setup, only ever try adding VLANs on the main /1 port
            if parent_port_id.endswith("/2") or parent_port_id.endswith("/3") or parent_port_id.endswith("/4"):
                continue

            present_vlan_ids = []
            vlan_network_interfaces = \
                dict(self.discovery_container.
                     get_resources(MetadataConstants.VLAN_NETWORK_INTERFACE,
                                   constraints=[from_collection(vlan_network_interface_collection)],
                                   any_child_version=True))

            if len(vlan_network_interfaces) > MAX_VLAN_ID - MIN_VLAN_ID:
                print "MESSAGE::No free VLAN id available on collection {}".format(vlan_network_interface_collection)
                continue

            for resource_link, resource in vlan_network_interfaces.iteritems():
                    try:
                        present_vlan_ids.append(resource.body["VLANId"])
                    except:
                        cts_error("Inorrect resource {resource_link:id} structure", **locals())

            for possible_id in range(MIN_VLAN_ID, MAX_VLAN_ID + 1):
                if possible_id not in present_vlan_ids:
                    chosen_vlan_network_interface_collection = vlan_network_interface_collection
                    new_vlan_id = possible_id
                    break

            if chosen_vlan_network_interface_collection:
                break

        if chosen_vlan_network_interface_collection:
            print "MESSAGE::CRUD test will be performed on vlan network interface collection %s" % \
                  chosen_vlan_network_interface_collection
        else:
            cts_warning('No free VLAN id available on any of the suitable ports (perhaps there are no suitable ports)')
            return ValidationStatus.BLOCKED

        self.post_vlan_payload = dict(VLANId=new_vlan_id,
                                      VLANEnable=True,
                                      Oem=dict(
                                        Intel_RackScale=dict(
                                            Tagged=True
                                        )))

        if self._test_case_create_vlan(chosen_vlan_network_interface_collection):
            self._test_case_get_created_vlan()
            self._test_case_delete_created_vlan()
            self._test_case_get_deleted_vlan()


CRUDOperations.service_cli()
