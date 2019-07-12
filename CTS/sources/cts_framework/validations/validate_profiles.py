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
import json

import objectpath

import cts_framework.validations.profile_database as profile_database
from cts_core.commons.error import cts_color_message, cts_message, get_fail, \
    get_ok, cts_warning, cts_error, get_info, get_warning
from cts_core.discovery.api_resource import ApiResourceProxy
from cts_core.validation.validation_status import ValidationStatus


class ValidateProfiles(object):
    def __init__(self, discovery_container):
        self.profile_container = {}
        self.discovery_container = discovery_container
        self.missing_main_elements = []

    def detect(self, profile_name, purpose, profile=None):
        cts_color_message("Testing a profile: {name}".format(name=profile_name),
                          "HEADER")
        cts_color_message("Purpose: {purpose}".format(purpose=purpose))

        status = ValidationStatus.PASSED

        self.main_resource_validation(profile)
        self.show_results(profile_name)

        if self.missing_main_elements:
            status = ValidationStatus.join_statuses(ValidationStatus.BLOCKED)
            cts_warning("Some elements are missing for all tests")
            print("STATUS::{status}".format(status=status))
        else:
            status = ValidationStatus.join_statuses(ValidationStatus.PASSED)
            cts_message("All main elements are available")
            print("STATUS::{status}".format(status=status))

        self.run(self.profile_container)

        cts_color_message("Deeper verification", font_color="HEADER")
        self.validate_profile_container()
        return status

    def validate(self):
        for x in self._load_available_profiles():
            self.__clean_missing_elements()
            self.detect(profile_name=x['ProfileName'], purpose=x['Purpose'], profile=x)

    def main_resource_validation(self, profile):
        for resource_key, resource_value in profile["Resources"].iteritems():
            is_resource = list(self.discovery_container.odata_type_lookup(resource_key))

            status_of_verification, verification_resource = self._resource_verification(is_resource,
                                                                                        resource_key)
            if not status_of_verification:
                self.missing_main_elements.append(resource_key)
                continue

            api_resource_for_deeper_verification = self.__get_api_element_by_the_odata_type(verification_resource)
            self.profile_container[resource_key] = {
                "api": api_resource_for_deeper_verification,
                "property_req": resource_value,
                "all_leafs": [],
                "last_property": [],
                "read_requirement": [],
                "write_requirement": [],
                "purpose": [],
                "min_count": [],
                "conditional_requirements": [],
                "min_support_values": [],
                "property_requirements": [],
                "status_of_validation": -1
            }

    def validate_profile_container(self):
        for key_element, value_element in self.profile_container.iteritems():
            print('TEST_CASE::Verification {element}'.format(element=key_element))
            for api in value_element['api']:
                self.test_roulette(api, value_element)

    def test_roulette(self, api, value_element):
        test_phases = {
            self.tree_deep_search: ["last_property",
                                    "Test phase: Verify that the \"PropertyRequirements\" fields exists"],
            self.min_count_search: ["min_count",
                                    "Test phase: Verify the number of elements meets the minimum criterion "
                                    "(\"MinCount\")"],
            self.read_requirement: ["read_requirement",
                                    "Test phase: Verify that the \"ReadRequirement\" fields exist and are readable"],
            self.write_requirement: ["write_requirement",
                                     "Test phase: ONLY Verify that the \"WriteRequirement\" elements are exist"]
        }
        test_case_status = ValidationStatus.PASSED
        for test_to_do, test_description in test_phases.iteritems():
            phase_status = ValidationStatus.PASSED
            each_value_element = value_element[test_description[0]]
            try:
                if len(each_value_element) > 0:
                    cts_message(test_description[1])
                    each_value_element = set(value_element[test_description[0]])
                else:
                    continue
            except TypeError:
                pass

            for each in each_value_element:
                phase_status = ValidationStatus.join_statuses(
                    test_to_do(api.body, each))
            print("STATUS::{status}".format(status=phase_status))
            test_case_status = ValidationStatus.join_statuses(phase_status)
        cts_color_message("Tested element {odata_type} ({odata_id})".format(
            odata_type=api.odata_type,
            odata_id=api.odata_id
        ), font_color="LIGHT_BLUE")
        print("STATUS::{status}".format(status=test_case_status))

    @staticmethod
    def read_requirement(api, key):
        """
        Verify that the resource exists in the list and that it can be read with GET
        :param api:
        :param key:
        :return:
        """
        resource_tree = objectpath.Tree(json.loads(json.dumps(api)))

        phase_status = ValidationStatus.PASSED

        for element_key, element_property in key.iteritems():
            result_tuple = tuple(resource_tree.execute('$..{key}'.format(key=element_key)))

            if not result_tuple:
                if element_property == "Mandatory":
                    cts_error("\t{status} Element {element_key} is {element_property}"
                              .format(status=get_fail(),
                                      element_key=element_key,
                                      element_property=element_property))
                    phase_status = ValidationStatus.join_statuses(ValidationStatus.FAILED)
                elif element_property == "IfImplemented":
                    continue
                else:
                    cts_color_message("\t{status} Element {element_key} is {element_property}"
                                      .format(status=get_info(),
                                              element_key=element_key,
                                              element_property=element_property))
                    phase_status = ValidationStatus.join_statuses(ValidationStatus.PASSED_WITH_WARNINGS)
            else:
                cts_message("\t{status} Element {element_key} is {element_property}"
                            .format(status=get_ok(),
                                    element_key=element_key,
                                    element_property=element_property))
                phase_status = ValidationStatus.join_statuses(ValidationStatus.PASSED)
        return phase_status

    def write_requirement(self, api, key):
        """
        Verify that the resource exists in the list and that it can be PATCH-ed using metadata information
        :param api:
        :param key:
        :return:
        """
        # print("INFO::The tool only checks if the resource is available\n"
        #       "(currently we do not check if it is possible to write this item)")
        return self.read_requirement(api, key)

    @staticmethod
    def tree_deep_search(body, key):
        """
        Use this method for looking a last property on the tree
        :return ValidationStatus
        """
        resource_tree = objectpath.Tree(json.loads(json.dumps(body)))

        result_tuple = tuple(resource_tree.execute('$..{}'.format(key)))
        if not result_tuple:
            cts_warning("\t{status} Key {key} was not found"
                        .format(status=get_warning(),
                                key=key))
            return ValidationStatus.PASSED_WITH_WARNINGS
        cts_message("\t{status} Key {key} was found"
                    .format(status=get_ok(),
                            key=key))
        return ValidationStatus.PASSED

    @staticmethod
    def min_count_search(api, key):
        """
        Verify the minimal presents of the properties
        :param api:
        :param key:
        :return:
        """
        resource_tree = objectpath.Tree(json.loads(json.dumps(api)))

        phase_status = ValidationStatus.PASSED

        for element_key, element_count in key.iteritems():
            result_tuple = tuple(resource_tree.execute('$..{key}'.format(
                key=element_key
            )))

            if len(result_tuple) < 1:
                cts_color_message(
                    "\t{status} Minimal count of {element_key} not achieved. Minimum values <= {count}".format(
                        status=get_fail(),
                        element_key=element_key,
                        count=element_count))
                phase_status = ValidationStatus.join_statuses(ValidationStatus.FAILED)
            else:
                cts_color_message("\t{status} Minimal count of {element_key} was achieved".format(
                    status=get_ok(),
                    element_key=element_key))
                phase_status = ValidationStatus.join_statuses(ValidationStatus.PASSED)
        return phase_status

    def run(self, profile_container):
        for pk, pv in profile_container.iteritems():
            for s in self.__dict_generator(pv['property_req'], external_dict_key=pk):
                self.profile_container[pk]['all_leafs'].append(s)

    def show_results(self, profile_name):
        cts_color_message("Main resource verification", font_color="HEADER")
        print("TEST_CASE::{profile_name}: High-level Resource Verification".format(
            profile_name=profile_name
        ))
        for profile_element, _ in self.profile_container.iteritems():
            cts_color_message("{status} {element_was_found} was detected".format(
                status=get_ok(),
                element_was_found=str(profile_element)))
        for missing_element in self.missing_main_elements:
            cts_color_message("{status} {missing_element} is missing".format(
                status=get_fail(),
                missing_element=missing_element))

    def additional_params_verification(self, dict_key, key, value):
        elements_for_check = {
            "ReadRequirement": "read_requirement",
            "WriteRequirement": "write_requirement",
            "MinCount": "min_count",
            "Purpose": "purpose",
            "MinSupportValues": "min_support_values",
            "ConditionalRequirements": "conditional_requirements",
            "PropertyRequirements": "property_requirements"
        }
        for key_for_check, value_for_save in elements_for_check.iteritems():
            read_status, read_value, condition = self._check_status_of(value, key_for_check)
            if read_status:
                self.profile_container[dict_key][value_for_save].append({key: read_value})

    def _load_available_profiles(self):
        available_profiles_data = (profile_database.OCP_PROFILE_1_0_0,
                                   profile_database.Intel_RSD_Compute_Base_1_0_0,
                                   profile_database.Intel_RSD_Baseline_Compute_0_0_1)
        available_profiles_json = [json.loads(x) for x in available_profiles_data]

        cts_color_message("CTS detects a {len_of_profiles} {const_profile}".
                          format(len_of_profiles=len(available_profiles_json),
                                 const_profile="profile" if len(available_profiles_json) <= 1 else "profiles"))
        self._print_available_profiles_names(available_profiles_json)
        return available_profiles_json

    @staticmethod
    def _check_status_of(dict_element, condition):
        for potential_read_requirement_key, read_value in dict_element.iteritems():
            if potential_read_requirement_key == condition:
                return True, read_value, condition
        return False, None, condition

    @staticmethod
    def _print_available_profiles_names(profiles_in_json):
        for xid, profile, in enumerate(profiles_in_json, start=1):
            cts_message("{id}: {profile_name}".format(id=xid, profile_name=profile['ProfileName']))

    @staticmethod
    def _resource_verification(is_resource, wanted_resource, resource_version="v1_0_0"):
        if not (is_resource and wanted_resource and is_resource):
            return False, []

        for resource in is_resource:
            resource_version_type = resource.split(".")
            if len(resource_version_type) < 2:
                print("Unable to verify")
                return False, []
            if wanted_resource.lower() == resource_version_type[0].lower():
                return True, resource
        return False, []

    def __get_api_element_by_the_odata_type(self, resource):
        api_resource_proxy_elements = [x for x in self.discovery_container.get_resources(resource)[0] \
                                       if isinstance(x, ApiResourceProxy)]
        return [self.discovery_container.get(y.url) for y in api_resource_proxy_elements]

    def __clean_missing_elements(self):
        self.profile_container = {}
        self.missing_main_elements = []

    def __dict_generator(self, dictionary, previous_element=None, external_dict_key=None):
        previous_element = previous_element[:] if previous_element else []
        if isinstance(dictionary, dict):
            for key, value in dictionary.items():
                if isinstance(value, dict):
                    if len(value) == 0:
                        # if the next element is empty dict
                        if external_dict_key:
                            self.profile_container[external_dict_key]['last_property'].append(key)
                        yield [key].append(previous_element)
                    else:
                        # verify keys regarding for this used by
                        if external_dict_key:
                            self.additional_params_verification(external_dict_key, key, value)
                        for dict_element in self.__dict_generator(value,
                                                                  [key].append(previous_element),
                                                                  external_dict_key=external_dict_key):
                            yield dict_element
                elif isinstance(value, list) or isinstance(value, tuple):
                    for v in value:
                        for dict_element in self.__dict_generator(v,
                                                                  [key].append(previous_element),
                                                                  external_dict_key=external_dict_key):
                            yield dict_element
                else:
                    yield previous_element + [key, value]
        else:
            yield dictionary
