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

from random import choice
from urlparse import urlparse

from cts_core.commons.api_caller import ApiCaller
from cts_core.commons.error import cts_warning, cts_error
from cts_core.commons.json_comparator import JsonComparator
from cts_core.commons.preconditions import Preconditions, Requirement
from cts_core.commons.services import RegisterService
from cts_core.commons.services import ServiceTypes
from cts_core.discovery.api_explorer import ApiExplorer
from cts_core.discovery.discovery_container import from_collection
from cts_core.metadata.metadata_constants import MetadataConstants2_1 as MetadataConstants
from cts_core.validation.validation_status import ValidationStatus
from cts_framework.commons.enums import ReturnCodes
from cts_framework.helpers.python.cts_test_script import CtsTestScript
from cts_framework.helpers.python.result_status import ResultStatus

MIN_VLAN_ID = 1
MAX_VLAN_ID = 4095


@RegisterService(ServiceTypes.PSME_2_1)
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
            Requirement(MetadataConstants.ETHERNET_SWITCH, min=1),
            Requirement(MetadataConstants.ETHERNET_SWITCH_PORT, min=1)
        ]
        preconditions = Preconditions(self.metadata_container, requirements)
        status = preconditions.validate(self.discovery_container)

        self.api_caller = ApiCaller(self.configuration)

        self.crud_acl_rule()
        self.crud_vlan()
        self.crud_static_mac()


    def _test_case_create_acl(self, acl_collection):
        print "TEST_CASE::Create ACL"

        self.created_acl_netloc = self.discovery_container.get_netloc(acl_collection)
        status, status_code, response_body, headers = self.api_caller.post_resource(
            acl_collection,
            self.discovery_container,
            payload=self.post_acl_payload,
            api_endpoint_override=self.created_acl_netloc)

        if not status:
            cts_error("Wrong status code of POST response: {status_code}; response body: {response_body:response_body}", **locals())
            self.set_status_failed()
            return False

        try:
            self.created_acl = headers["Location"]
            print "MESSAGE::Newly created ACL URI: %s" % self.created_acl
        except KeyError:
            cts_error("Incorrect response - header shall contain the location of the created resource")
            self.set_status_failed()
            return False

        if self.created_acl.endswith("/"):
            self.created_acl += self.created_acl[:-1]

        print "MESSAGE::ACL created"
        self.set_status_passed()
        return True


    def _test_case_get_created_acl(self):
        print "TEST_CASE::Get created ACL"

        link, status, status_code, response_body, _ = \
            self.api_caller.get_resource(self.created_acl,
                                         self.discovery_container,
                                         api_endpoint_override=self.created_acl_netloc)

        if not status:
            cts_error("Wrong status code of GET response: {status_code}; response body: {response_body:response_body}", **locals())
            self.set_status_failed()
            return False
        else:
            print "MESSAGE::Comparing newly created ACL to given payload"
            if not JsonComparator.compare_json_to_golden(response_body, self.post_acl_payload):
                cts_error("Newly created ACL's body incorrect")
                self.set_status_failed()
                return False

        print "MESSAGE::Newly created ACL correct"
        self.set_status_passed()
        return True

    def _test_case_delete_created_acl(self):
        print "TEST_CASE::Delete created ACL"

        status, status_code, response_body, _ = self.api_caller.delete_resource(self.created_acl,
                                                                                self.discovery_container)

        if not status:
            cts_error("Wrong status code of DELETE response: {status_code}; response body: {response_body:response_body}", **locals())
            self.set_status_failed()
            return False

        print "MESSAGE::Newly created ACL deleted"
        self.set_status_passed()
        return True


    def _test_case_get_deleted_acl(self):
        print "TEST_CASE::Get deleted ACL"

        link, status, status_code, response_body, _ = \
            self.api_caller.get_resource(self.created_acl, self.discovery_container,
                                         acceptable_return_codes=[
                                             ReturnCodes.NOT_FOUND],
                                         api_endpoint_override=self.created_acl_netloc)

        if not status:
            cts_error("Wrong status code of GET response after deletion: {status_code}; response body: {response_body:response_body}",
                      **locals())
            self.set_status_failed()
            return False

        print "MESSAGE::Newly created ACL not found (as intended)"
        self.set_status_passed()
        return True

    def _test_case_bind_acl_to_port(self, port):
        print "TEST_CASE::Bind ACL to a port"

        port_id = dict()
        port_id["@odata.id"] = port
        payload = dict(
            Port=port_id
        )
        status, status_code, response_body, _ = \
            self.api_caller.post_resource(self.created_acl +
                                          "/Actions/EthernetSwitchACL.Bind",
                                          self.discovery_container,
                                          payload=payload,
                                          acceptable_return_codes=
                                          [ReturnCodes.NO_CONTENT],
                                          expect_location=False,
                                          api_endpoint_override=self.created_acl_netloc)
        if not status:
            cts_error("Wrong status code of POST response: {status_code}; response body: {response_body:response_body}", **locals())
            self.set_status_failed()
            return False

        print "MESSAGE::ACL successfully bound to port"
        self.set_status_passed()
        return True

    def _test_case_unbind_acl_from_port(self, port):
        print "TEST_CASE::Unbind ACL from a port"

        port_id = dict()
        port_id["@odata.id"] = port
        payload = dict(
            Port=port_id
        )
        status, status_code, response_body, _ = \
            self.api_caller.post_resource(self.created_acl + "/Actions/EthernetSwitchACL.Unbind",
                                          self.discovery_container,
                                          payload=payload,
                                          acceptable_return_codes=[ReturnCodes.NO_CONTENT],
                                          expect_location=False,
                                          api_endpoint_override=self.created_acl_netloc)
        if not status:
            cts_error("Wrong status code of POST response: {status_code}; response body: {response_body:response_body}", **locals())
            self.set_status_failed()
            return False

        print "MESSAGE::ACL successfully unbound from port"
        self.set_status_passed()
        return True


    def _test_case_bound_acl_and_port_are_linked(self, port, should_be_linked):
        print "TEST_CASE::Check if the ACL and port are linked  " + \
              ("(should be)" if should_be_linked else "(shouldn't be)")
        port = urlparse(port).path
        acl = urlparse(self.created_acl).path
        acl_linked, port_linked = None, None

        try:
            link, status, status_code, acl_body, _ = \
                self.api_caller.get_resource(self.created_acl,
                                             self.discovery_container,
                                             api_endpoint_override=self.created_acl_netloc)
            if not status:
                cts_error("Wrong status code of GET response: {status_code}; response body: {response_body:response_body}", **locals())
                self.set_status_failed()
                return False

            if not {"@odata.id": port} in acl_body["Links"]["BoundPorts"] \
                    and not {"@odata.id": port + "/"} in acl_body["Links"]["BoundPorts"]:
                acl_linked = False
                if should_be_linked:
                    cts_error("No link to the bound port in the ACL's body")
                else:
                    print "MESSAGE::No link to the bound port in the ACL's body (as expected)"
            else:
                acl_linked = True
                if should_be_linked:
                    print "MESSAGE::ACL has a link to the bound Port"
                else:
                    cts_error("ACL still has a link to the unbound Port")

        except KeyError:
            cts_error("No Links/BoundPorts array present in the ACL's body")

        try:
            link, status, status_code, port_body, _ = \
                self.api_caller.get_resource(port,
                                             self.discovery_container,
                                             api_endpoint_override=self.discovery_container.get_netloc(
                                                 port))
            if not status:
                cts_error("Wrong status code of GET response: {status_code}; response body: {response_body:response_body}", **locals())
                self.set_status_failed()
                return False

            if not {"@odata.id": acl} in port_body["Links"]["ActiveACLs"] \
                    and not {"@odata.id": acl + "/"} in port_body["Links"]["ActiveACLs"]:
                port_linked = False
                if should_be_linked:
                    cts_error("No link to the bound ACL in the port's body")
                else:
                    print "MESSAGE::No link to the bound ACL in the port's body (as expected)"
            else:
                port_linked = True
                if should_be_linked:
                    print "MESSAGE::Port has a link to the bound ACL"
                else:
                    cts_error("Port still has a link to the unbound ACL")

        except KeyError:
            cts_error("No Links/ActiveACLs array present in the port's body")

        if acl_linked is not None and port_linked is not None:
            if acl_linked == port_linked == should_be_linked:
                self.set_status_passed()
                return True

        self.set_status_failed()
        return False

    def _test_case_add_rules_to_acl(self):
        print "TEST_CASE::Add rules to the ACL"
        self.created_rules = dict()
        optional_rules_triggered_an_error = False

        rule_collection = self.created_acl + "/Rules"
        for rule_type, rule_payload in self.rules_payloads.iteritems():
            print "MESSAGE::Adding a {type} rule".format(type=rule_type)


            status, status_code, response_body, headers = \
                self.api_caller.post_resource(
                rule_collection, self.discovery_container, payload=rule_payload,
                    api_endpoint_override=self.created_acl_netloc)

            if not status:
                if rule_type not in self.mandatory_rules:
                    optional_rules_triggered_an_error = True
                    cts_warning("Wrong status code of POST response: {status_code}; response body: {response_body:response_body}",
                                **locals())
                    cts_warning("Rule {rule_type} was defined as optional, continuing", **locals())
                    continue
                cts_error("Wrong status code of POST response: {status_code}; response body: {response_body:response_body}",
                          **locals())
                self.set_status_failed()
                return False

            try:
                self.created_rules[rule_type] = headers["Location"]
                print "MESSAGE::Newly created ACL Rule URI: %s" % self.created_acl
            except KeyError:
                cts_error("Incorrect response - header shall contain the location of the created resource")
                self.set_status_failed()
                return False

            print "MESSAGE::Getting the newly created ACL Rule URI: %s" % self.created_acl
            link, status, status_code, response_body, _ = \
                self.api_caller.get_resource(self.created_rules[rule_type],
                                             self.discovery_container,
                                             api_endpoint_override=self.created_acl_netloc)

            if not status:
                cts_error("Wrong status code of GET response: {status_code}; response body: {response_body:response_body}",
                          **locals())
                self.set_status_failed()
                return False
            else:
                print "MESSAGE::Comparing newly created ACL Rule to given payload"
                if not JsonComparator.compare_json_to_golden(response_body, rule_payload):
                    cts_error("Newly created Rule's body incorrect")
                    self.set_status_failed()
                    return False
                else:
                    print "MESSAGE::Newly created Rule's body correct"

            print "MESSAGE::Rule added"

        print "MESSAGE::ACL Rules created succesfully"

        if optional_rules_triggered_an_error:
            self.set_status_passed_with_warnings()
        else:
            self.set_status_passed()

        return True

    def _test_case_delete_rules_from_acl(self):
        print "TEST_CASE::Delete the created rules from the ACL"

        for rule_type, rule_uri in self.created_rules.iteritems():
            print "MESSAGE::Deleting {type}".format(type=rule_type)

            status, status_code, response_body, _ = self.api_caller.delete_resource(rule_uri, self.discovery_container)

            if not status:
                cts_error("Wrong status code of DELETE response: {status_code}; response body: {response_body:response_body}",
                          **locals())
                self.set_status_failed()
                return False

            print "MESSAGE::Checking if the rule was actually deleted"
            link, status, status_code, response_body, _ = \
                self.api_caller.get_resource(rule_uri,
                                             self.discovery_container,
                                             acceptable_return_codes=[
                                                 ReturnCodes.NOT_FOUND],
                                             api_endpoint_override=self.created_acl_netloc)
            if not status:
                cts_error(
                    "Wrong status code of GET response after deletion: {status_code}; response body: {response_body:response_body}",
                    **locals())
                self.set_status_failed()
                return False

            print "MESSAGE::The rule was not found (as intended)"

        print "MESSAGE::ACL Rules deleted succesfully"
        self.set_status_passed()
        return True

    def crud_acl_rule(self):
        """
        Test is trying to perform CRUD (create, read, update, delete) as well as binding operations on an ACL and ACL
        Rule resource
        """
        switches = \
            dict(self.discovery_container.get_resources(MetadataConstants.ETHERNET_SWITCH, any_child_version=True))

        all_ports = []

        if not switches:
            cts_error("Could not find any Ethernet Switch to perform the test on, aborting")
            return ValidationStatus.BLOCKED

        chosen_switch, chosen_acl_collection, chosen_port, mirrored_port = None, None, None, None

        for switch in switches.keys():
            acl_collections = dict(self.discovery_container.get_resources(MetadataConstants.ETHERNET_SWITCH_ACL_COLLECTION,
                                                                          any_child_version=True,
                                                                          constraints=[from_collection(switch)]))
            if acl_collections:
                chosen_switch = switch
                chosen_acl_collection = choice(acl_collections.keys())

                all_ports = [port[1].odata_id for port in
                             self.discovery_container.get_resources(
                                 MetadataConstants.ETHERNET_SWITCH_PORT,
                                 any_child_version=True,
                                 constraints=[
                                     from_collection(chosen_switch)])]


                try:
                    chosen_port = choice(all_ports)
                    all_ports.remove(chosen_port)
                    mirrored_port = choice(all_ports)
                except IndexError:
                    if not chosen_port:
                        print "MESSAGE::Could not find a proper port for ACL binding on the switch"
                        continue
                    else:
                        print "WARNING::Only one proper port found, unable to create a Mirror rule"
                        break


        if not chosen_acl_collection:
            cts_error("Could not find any ACL collection on any of the present Ethernet Switches, aborting")
            return ValidationStatus.BLOCKED

        if not chosen_port:
            cts_error("Could not find any proper port for ACL binding on any of the present Ethernet Switches, aborting")
            return ValidationStatus.BLOCKED


        print "MESSAGE:CRUD test will be performed on ACL collection: {collection}, the test will attempt to bind and "\
              "unbind the ACL from the port: {port}".format(collection=chosen_acl_collection, port=chosen_port)

        self.post_acl_payload = dict()
        self.rules_payloads = dict()

        self.rules_payloads["Deny"] = dict(
            Action="Deny",
            Condition=dict(
                MACSource=dict(
                    MACAddress="AA:BB:CC:DD:EE:FF",
                    Mask="FF:FF:FF:00:00:00"
                )
            )
        )

        self.rules_payloads["Permit"] = dict(
            Action="Permit",
            Condition=dict(
                IPSource=dict(
                    IPv4Address="192.168.0.1",
                    Mask="255.255.255.0"
                )
            )
        )

        self.rules_payloads["Forward"] = dict(
            Action="Forward",
            ForwardMirrorInterface={"@odata.id": chosen_port},
            Condition=dict(
                IPDestination=dict(
                    IPv4Address="1.1.1.1"
                )
            )
        )

        if mirrored_port:
            self.rules_payloads["Mirror"] = dict(
                RuleId=1111,
                Action="Mirror",
                ForwardMirrorInterface={"@odata.id": chosen_port},
                MirrorPortRegion=[{"@odata.id": mirrored_port}],
                MirrorType="Egress",
                Condition=dict(
                    VLANId=dict(
                        Id=1,
                        Mask=None
                    )
                )
            )

        # basic rules (Deny and Permit actions) should be supported by any PSME service; therefore failure in adding
        # them triggers FAILED status for the test. If the mandatory rules are added successfully, but there is an
        # error while adding the optional (Forward and Mirror actions) rules, the test's result is PASSED WITH WARNINGS
        self.mandatory_rules = ["Deny", "Permit"]

        if self._test_case_create_acl(chosen_acl_collection):
            self._test_case_get_created_acl()
            if self._test_case_add_rules_to_acl(): # Rules need to be added before binding
                if self._test_case_bind_acl_to_port(chosen_port):
                    if self._test_case_bound_acl_and_port_are_linked(chosen_port, True):
                        if self._test_case_unbind_acl_from_port(chosen_port):
                            self._test_case_bound_acl_and_port_are_linked(chosen_port, False)
                self._test_case_delete_rules_from_acl()
            self._test_case_delete_created_acl()
            self._test_case_get_deleted_acl()

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
            cts_error("Wrong status code of DELETE response: {status_code}; response body: {response_body:response_body}",
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
            cts_error("Wrong status code of GET response after deletion: {status_code}; response body: {response_body:response_body}",
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
            Requirement(MetadataConstants.ETHERNET_SWITCH, min=1),
            Requirement(MetadataConstants.ETHERNET_SWITCH_PORT, min=1),
            Requirement(MetadataConstants.VLAN_NETWORK_INTERFACE_COLLECTION, min=1),
        ]
        preconditions = Preconditions(self.metadata_container, requirements)

        if preconditions.validate(self.discovery_container) == ValidationStatus.FAILED:
            return ValidationStatus.BLOCKED

        vlan_network_interface_collections = \
            dict(self.discovery_container.get_resources(MetadataConstants.VLAN_NETWORK_INTERFACE_COLLECTION,
                 any_child_version=True))

        chosen_vlan_network_interface_collection = None
        new_vlan_id = 0

        for vlan_network_interface_collection in vlan_network_interface_collections:
            present_vlan_ids = []
            vlan_network_interfaces = dict(self.discovery_container.
                                           get_resources(MetadataConstants.VLAN_NETWORK_INTERFACE,
                                                        constraints=[from_collection(vlan_network_interface_collection)],
                                                        any_child_version=True))

            if len(vlan_network_interfaces) > MAX_VLAN_ID - MIN_VLAN_ID:
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
            cts_warning('No free VLAN id available')
            return ValidationStatus.BLOCKED

        self.post_vlan_payload = dict(
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


    def _test_case_create_static_mac(self, static_mac_collection):
        print "TEST_CASE::Create Static MAC"

        status, status_code, response_body, headers = self.api_caller.post_resource(
            static_mac_collection, self.discovery_container, payload=self.post_static_mac_payload)

        if not status:
            cts_error("Wrong status code of POST response: {status_code}; response body: {response_body:response_body}",
                      **locals())
            self.set_status_failed()
            return False

        try:
            self.created_static_mac = headers["Location"]
            self.created_static_mac_netloc = self.discovery_container.get_netloc(static_mac_collection)
            print "MESSAGE::Newly created Static MAC URI: %s" % self.created_static_mac
        except KeyError:
            cts_error("Incorrect response - header shall contain the location of the created resource")
            self.set_status_failed()
            return False

        print "MESSAGE::Static MAC created"
        self.set_status_passed()
        return True


    def _test_case_get_created_static_mac(self):
        print "TEST_CASE::Get created Static MAC"

        link, status, status_code, response_body, _ = \
            self.api_caller.get_resource(self.created_static_mac,
                                         self.discovery_container,
                                         api_endpoint_override=self.created_static_mac_netloc)

        if not status:
            cts_error("Wrong status code of GET response: {status_code}; response body: {response_body:response_body}",
                      **locals())
            self.set_status_failed()
            return False
        else:
            print "MESSAGE::Comparing newly created Static MAC to given payload"
            if not JsonComparator.compare_json_to_golden(response_body, self.post_static_mac_payload):
                cts_error("Newly created Static MAC's body incorrect")
                self.set_status_failed()
                return False

        print "MESSAGE::Newly created Static MAC correct"
        self.set_status_passed()
        return True

    def _test_case_delete_created_static_mac(self):
        print "TEST_CASE::Delete created Static MAC"

        status, status_code, response_body, _ = self.api_caller.delete_resource(self.created_static_mac,
                                                                                self.discovery_container)

        if not status:
            cts_error("Wrong status code of DELETE response: {status_code}; response body: {response_body:response_body}", **locals())
            self.set_status_failed()
            return False


        print "MESSAGE::Newly created Static MAC deleted"
        self.set_status_passed()
        return True

    def _test_case_get_deleted_static_mac(self):
        print "TEST_CASE::Get deleted Static MAC"

        link, status, status_code, response_body, _ = \
            self.api_caller.get_resource(self.created_static_mac,
                                         self.discovery_container,
                                         acceptable_return_codes=[ReturnCodes.NOT_FOUND],
                                         api_endpoint_override=self.created_static_mac_netloc)

        if not status:
            cts_error("Wrong status code of GET response after deletion: {status_code}; response body: {response_body:response_body}",
                      **locals())
            self.set_status_failed()
            return False

        print "MESSAGE::Newly created Static MAC not found (as intended)"
        self.set_status_passed()
        return True


    def crud_static_mac(self):
            """
            Test is trying to perform CRUD (create, read, update, delete) operations on a Static MAC resource
            """

            def _find_unique_mac_address_vlan_id_pair(present_pairs):
                MAX_ADDRESS = 281474976710656 - 1 # 16^12 - 1
                def address_to_string(address):
                    parts = []
                    for _ in range(6):
                        parts.append(address % 256)
                        address = address / 256
                    return ":".join("{0:02x}".format(part) for part in parts).upper()
                address = 0
                vlan_id = MIN_VLAN_ID
                while (address_to_string(address), vlan_id) in present_pairs:
                    address = address + 1
                    if address > MAX_ADDRESS:
                        address = 0
                        vlan_id = vlan_id + 1
                        if vlan_id > MAX_VLAN_ID:
                            return None
                return (address_to_string(address), vlan_id)

            requirements = [
                Requirement(MetadataConstants.SERVICE_ROOT, min=1, max=1),
                Requirement(MetadataConstants.ETHERNET_SWITCH, min=1),
                Requirement(MetadataConstants.ETHERNET_SWITCH_PORT, min=1),
                Requirement(MetadataConstants.STATIC_MAC_COLLECTION, min=1),
            ]
            preconditions = Preconditions(self.metadata_container, requirements)

            if preconditions.validate(self.discovery_container) == ValidationStatus.FAILED:
                return ValidationStatus.BLOCKED

            static_mac_collections = \
                dict(self.discovery_container.get_resources(MetadataConstants.STATIC_MAC_COLLECTION,
                     any_child_version=True))

            chosen_static_mac_collection = static_mac_collections.keys()[0]

            print "MESSAGE::\nMESSAGE::Static MAC CRUD test will be performed on collection: {}"\
                .format(chosen_static_mac_collection)

            present_vlan_id_mac_address_pairs = []

            for resource_link, resource in self.discovery_container.get_resources(
                    MetadataConstants.STATIC_MAC,
                    any_child_version=True,
                    constraints=[from_collection(chosen_static_mac_collection)]):
                try:
                    present_vlan_id_mac_address_pairs.append((resource.body["MACAddress"], resource.body["VLANId"]))
                except Exception as err:
                    cts_warning("{link:id} Incorrect resource structure; err={err:exception}",
                                link=resource_link, err=err)

            initial_values = _find_unique_mac_address_vlan_id_pair(present_vlan_id_mac_address_pairs)

            if not initial_values:
                cts_error("Cannot create a Static MAC on the chosen collection - "
                          "all MAC addresses and VLAN ids already occupied")
                return ValidationStatus.BLOCKED

            self.post_static_mac_payload = dict(
                MACAddress=initial_values[0],
                VLANId=initial_values[1]
            )

            # PSME does not support patching Static MAC
            if self._test_case_create_static_mac(chosen_static_mac_collection):
                self._test_case_get_created_static_mac()
                self._test_case_delete_created_static_mac()
                self._test_case_get_deleted_static_mac()

CRUDOperations.service_cli()
