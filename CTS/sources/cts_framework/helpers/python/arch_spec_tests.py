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
import itertools
import re

from cts_core.commons.error import cts_message
from cts_core.validation.validation_status import ValidationStatus
from cts_framework.commons.color_printer import ColorPrinter


class ArchSpecTests:

    def __init__(self):
        self.failure = False
        self.resources = {}

    def get_members(self, discovery_container, entity):
        """gets members of entity through Discovery Container and removes unnecessary members"""
        members = dict(discovery_container.get_resources(entity, True))
        revised_members = []
        for member in members:
            if "#" not in member:
                revised_members.append(member)
        return revised_members

    def construct(self, discovery_container, resource, property):
        try:
            exec "construct = discovery_container.get(resource).body" + property
            return construct
        except KeyError:
            return None

    def format_body(self, discovery_container, resource, properties):
        """if asterisk was used it finds existing properties"""
        return_list = []
        for prop in properties:
            body = ""
            if "/" in prop:
                for element in prop.split("/"):
                    if "[" in element:
                        body = body + "[\"" + element.split("[")[0] + "\"][" + element.split("[")[1]
                    else:
                        body = body + "[\"" + element + "\"]"
            else:
                if "[" in prop:
                    body = "[\"" + prop.split("[")[0] + "\"][" + prop.split("[")[1]
                else:
                    body = "[\"" + prop + "\"]"
            if "[*]" in body:
                for i in itertools.count():
                    try:
                        construct = self.construct(discovery_container, resource, body.replace("*", str(i)))
                        return_list.append(body.replace("*", str(i)))
                    except IndexError:
                        break
                    except KeyError:
                        break
            else:
                return_list.append(body)
        return return_list

    def createlist(self, discovery_container, resource_collection, collection_name, property):
        return_list = []
        if self.collection_is_not_empty(resource_collection, collection_name) is True:
            for resource in resource_collection:
                for prop in self.format_body(discovery_container, resource, [property]):
                    return_list.append(self.construct(discovery_container, resource, prop))
            return return_list

    def reduceby_string(self, discovery_container, resource_collection, properties, str_list):
        if resource_collection is None:
            return None
        return_list = []
        for resource in resource_collection:
            error = False
            for counter, arg in enumerate(self.format_body(discovery_container, resource, properties)):
                if self.construct(discovery_container, resource, arg) != str_list[counter]:
                    error = True
            if error is False:
                return_list.append(resource)
        return return_list

    def reduceby_string_or(self, discovery_container, resource_collection, properties, str_list):
        if resource_collection is None:
            return None
        return_list = []
        for resource in resource_collection:
            error = True
            for counter, arg in enumerate(self.format_body(discovery_container, resource, properties)):
                if self.construct(discovery_container, resource, arg) == str_list[counter]:
                    error = False
            if error is False:
                return_list.append(resource)
        return return_list

    def reduceby_not_null(self, discovery_container, resource_collection, properties):
        if resource_collection is None:
            return None
        return_list = []
        for resource in resource_collection:
            error = False
            for arg in self.format_body(discovery_container, resource, properties):
                if self.construct(discovery_container, resource, arg) is None:
                    error = True
            if error is False:
                return_list.append(resource)
        return return_list

    def reduceby_match_collection(self, discovery_container, resource_collection, match_resource, properties, return_parent):
        if resource_collection is None:
            return None
        return_list = []
        return_list2 = []
        for member in resource_collection:
            for resource in match_resource:
                for arg in self.format_body(discovery_container, member, properties):
                    if self.construct(discovery_container, member, arg) is not None:
                        if self.construct(discovery_container, member, arg) in resource:
                            if resource not in return_list:
                                return_list.append(resource)
                            if return_parent is True:
                                if member not in return_list2:
                                    return_list2.append(member)
        if return_parent is True:
            return return_list, return_list2
        else:
            return return_list

    def all_not_bool_test(self, discovery_container, resource_collection, collection_name, mode, properties):
        """function to test if all requested properties in collection are not true/false"""
        if self.collection_is_not_empty(resource_collection, collection_name) is True:
            for resource in resource_collection:
                print "MESSAGE::%s" % resource
                for subproperty in self.format_body(discovery_container, resource, properties):
                    construct = self.construct(discovery_container, resource, subproperty)
                    if construct is mode:
                        print "MESSAGE::" + ColorPrinter.format_text("Failed", color=ColorPrinter.RED_FORE) + ": " + subproperty + " property is " + str(mode)
                        self.failure = True
                    elif construct is None:
                        print "MESSAGE::" + ColorPrinter.format_text("Failed", color=ColorPrinter.RED_FORE) + ": " + subproperty + " property is Null"
                        self.failure = True
                    else:
                        print "MESSAGE::" + ColorPrinter.format_text("Passed", color=ColorPrinter.GREEN_FORE) + ": " + subproperty + " property is not " + str(mode)

    def all_not_null_test(self, discovery_container, resource_collection, collection_name, properties):
        """function to test if all requested properties in collection are not null"""
        if self.collection_is_not_empty(resource_collection, collection_name) is True:
            for resource in resource_collection:
                print "MESSAGE::%s" % resource
                for subproperty in self.format_body(discovery_container, resource, properties):
                    construct = self.construct(discovery_container, resource, subproperty)
                    if construct is None:
                        print "MESSAGE::" + ColorPrinter.format_text("Failed", color=ColorPrinter.RED_FORE) + ": " + subproperty + " property is Null"
                        self.failure = True
                    else:
                        print "MESSAGE::" + ColorPrinter.format_text("Passed", color=ColorPrinter.GREEN_FORE) + ": " + subproperty + " property is not Null"

    def all_lists_not_empty_test(self, discovery_container, resource_collection, collection_name, properties):
        """function to test if all requested list properties in collection are not empty"""
        if self.collection_is_not_empty(resource_collection, collection_name) is True:
            for resource in resource_collection:
                print "MESSAGE::%s" % resource
                for subproperty in self.format_body(discovery_container, resource, properties):
                    construct = self.construct(discovery_container, resource, subproperty)
                    if construct is None:
                        print "MESSAGE::" + ColorPrinter.format_text("Failed", color=ColorPrinter.RED_FORE) + ": " + subproperty + " property is empty"
                        self.failure = True
                    else:
                        print "MESSAGE::" + ColorPrinter.format_text("Passed", color=ColorPrinter.GREEN_FORE) + ": " + subproperty + " property is not empty"

    def numeric_logical_test(self, discovery_container, resource_collection, collection_name, properties):
        """function to test if requested properties are integer and higher than zero"""
        if self.collection_is_not_empty(resource_collection, collection_name) is True:
            for resource in resource_collection:
                print "MESSAGE::%s" % resource
                for subproperty in self.format_body(discovery_container, resource, properties):
                    construct = self.construct(discovery_container, resource, subproperty)
                    if type(construct) != int:
                        print "MESSAGE::" + ColorPrinter.format_text("Failed", color=ColorPrinter.RED_FORE) + ": " + subproperty + " property is not an integer"
                        self.failure = True
                    elif construct <= 0:
                        print "MESSAGE::" + ColorPrinter.format_text("Failed", color=ColorPrinter.RED_FORE) + ": " + subproperty + " property is not > 0"
                        self.failure = True
                    else:
                        print "MESSAGE::" + ColorPrinter.format_text("Passed", color=ColorPrinter.GREEN_FORE) + ": " + subproperty + " property is > 0"

    def string_logical_test(self, discovery_container, resource_collection, collection_name, properties, str_value,
                            consists=False):
        """function to test if requested properties are equal to string"""
        if self.collection_is_not_empty(resource_collection, collection_name) is True:
            for resource in resource_collection:
                print "MESSAGE::%s" % resource
                for subproperty in self.format_body(discovery_container, resource, properties):
                    construct = self.construct(discovery_container, resource, subproperty)
                    if consists is False:
                        if str_value in re.split('\W+', construct):
                            print "MESSAGE::" + ColorPrinter.format_text("Passed",
                                                                         color=ColorPrinter.GREEN_FORE) + ": " + subproperty + " property is " + str_value
                        elif construct != str_value:
                            print "MESSAGE::" + ColorPrinter.format_text("Failed",
                                                                         color=ColorPrinter.RED_FORE) + ": " + subproperty + " property is not " + str_value
                            self.failure = True
                        else:
                            print "MESSAGE::" + ColorPrinter.format_text("Passed",
                                                                         color=ColorPrinter.GREEN_FORE) + ": " + subproperty + " property is " + str_value
                    else:
                        if str_value not in construct:
                            print "MESSAGE::" + ColorPrinter.format_text("Failed",
                                                                         color=ColorPrinter.RED_FORE) + ": " + subproperty + " property does not contain " + str_value
                            self.failure = True
                        else:
                            print "MESSAGE::" + ColorPrinter.format_text("Passed",
                                                                         color=ColorPrinter.GREEN_FORE) + ": " + subproperty + " property does contain " + str_value

    def string_logical_test_special(self, discovery_container, resource_collection, collection_name, property, str_list):
        """function to test if at least one of members matches one of strings"""
        if self.collection_is_not_empty(resource_collection, collection_name) is True:
            i = 0
            for resource in resource_collection:
                i = i + 1
                for string in str_list:
                    for prop in self.format_body(discovery_container, resource, [property]):
                        construct = self.construct(discovery_container, resource, prop)
                        if construct == string:
                            print "MESSAGE::" + ColorPrinter.format_text("Passed", color=ColorPrinter.GREEN_FORE) + ": " + \
                                  "One of " + str(property) + " property equals one of strings " + str(str_list)
                            return
                else:
                    if i == len(resource_collection):
                        print "MESSAGE::" + ColorPrinter.format_text("Failed", color=ColorPrinter.RED_FORE) + ": " + \
                              "None of " + str(property) + " property equals one of strings " + str(str_list)
                        self.failure = True

    def collection_is_not_empty(self, resource_collection, name):
        """function to test if collection is empty"""
        if resource_collection is not None:
            if len(resource_collection) == 0:
                self.failure = True
                print "MESSAGE::" + ColorPrinter.format_text("Failed", color=ColorPrinter.RED_FORE) + ": Collection of " + name + " is empty"
                return False
            else:
                print "MESSAGE::" + ColorPrinter.format_text("Passed", color=ColorPrinter.GREEN_FORE) + ": Collection of " + name + " is not empty"
                return True
        else:
            self.failure = True
            print "MESSAGE::" + ColorPrinter.format_text("Failed",
                                                         color=ColorPrinter.RED_FORE) + ": Collection of " + name + " does not exist!"
            return False

    def all_elements_unique(self, elements, name):
        """function to test if all elements in list are unique"""
        if not elements:
            self.failure = True
            print "MESSAGE::" + ColorPrinter.format_text("Failed",
                                                         color=ColorPrinter.RED_FORE) + ": List is empty or does not exist!"
            return False
        else:
            if len(elements) > len(set(elements)):
                self.failure = True
                print "MESSAGE::" + ColorPrinter.format_text("Failed", color=ColorPrinter.RED_FORE) + ": All " + name + " are not unique"
                return False
            else:
                print "MESSAGE::" + ColorPrinter.format_text("Passed", color=ColorPrinter.GREEN_FORE) + ": All " + name + " are unique"
                return True

    def all_elements_identical(self, elements, name):
        """function to test if all elements in list are identical"""
        if not elements:
            self.failure = True
            print "MESSAGE::" + ColorPrinter.format_text("Failed",
                                                         color=ColorPrinter.RED_FORE) + ": List is empty or does not exist!"
            return False
        else:
            if len(set(elements)) > 1:
                self.failure = True
                print "MESSAGE::" + ColorPrinter.format_text("Failed", color=ColorPrinter.RED_FORE) + ": All " + name + " are not identical"
                return False
            else:
                print "MESSAGE::" + ColorPrinter.format_text("Passed", color=ColorPrinter.GREEN_FORE) + ": All " + name + " are identical"
                return True

    def test_result(self):
        if self.failure is False:
            status = ValidationStatus.PASSED
        else:
            status = ValidationStatus.FAILED
        self.failure = False
        return status

    def warn_on_null(self, resource_collection, name):
        if len(resource_collection) == 0:
            cts_message("Warning! Collection of " + name + " is empty!")

    def parse_json(self, discovery_container, data):
        for key, value in data["resources"].items():
            self.resources[key] = self.get_members(discovery_container, value[0])
            self.warn_on_null(self.resources[key], value[1])
        global resources
        for action in data["actions"]:
            if action[0] == "reduceby_string":
                self.resources[action[1][0]] = self.reduceby_string(discovery_container, self.resources[action[1][1]],
                                                                    action[1][2],
                                                                    action[1][3])
            elif action[0] == "reduceby_string_or":
                self.resources[action[1][0]] = self.reduceby_string_or(discovery_container, self.resources[action[1][1]],
                                                                       action[1][2],
                                                                       action[1][3])
            elif action[0] == "reduceby_not_null":
                self.resources[action[1][0]] = self.reduceby_not_null(discovery_container, self.resources[action[1][1]],
                                                                      action[1][2])
            elif action[0] == "reduceby_match_collection":
                if action[1][4] is True:
                    self.resources[action[1][0][0]], self.resources[action[1][0][1]] = self.reduceby_match_collection(discovery_container,
                                                                                                                      self.resources[action[1][1]], self.resources[action[1][2]], action[1][3], action[1][4])
                else:
                    self.resources[action[1][0]] = self.reduceby_match_collection(discovery_container, self.resources[action[1][1]],
                                                                                  self.resources[action[1][2]], action[1][3], action[1][4])
            elif action[0] == "createlist":
                self.resources[action[1][0]] = self.createlist(discovery_container, self.resources[action[1][1]], action[1][2],
                                                               action[1][3])
            elif action[0] == "all_elements_unique":
                self.all_elements_unique(self.resources[action[1][0]], action[1][1])
            elif action[0] == "all_elements_identical":
                self.all_elements_identical(self.resources[action[1][0]], action[1][1])
            elif action[0] == "all_not_null_test":
                self.all_not_null_test(discovery_container, self.resources[action[1][0]], action[1][1], action[1][2])
            elif action[0] == "all_not_bool_test":
                self.all_not_bool_test(discovery_container, self.resources[action[1][0]], action[1][1], action[1][2],
                                       action[1][3])
            elif action[0] == "string_logical_test":
                self.string_logical_test(discovery_container, self.resources[action[1][0]], action[1][1], action[1][2],
                                         action[1][3])
            elif action[0] == "string_logical_test_special":
                self.string_logical_test_special(discovery_container, self.resources[action[1][0]], action[1][1],
                                                 action[1][2], action[1][3])
            elif action[0] == "numeric_logical_test":
                self.numeric_logical_test(discovery_container, self.resources[action[1][0]], action[1][1], action[1][2])
            elif action[0] == "collection_is_not_empty":
                self.collection_is_not_empty(self.resources[action[1][0]], action[1][1])
            elif action[0] == "all_lists_not_empty_test":
                self.all_lists_not_empty_test(discovery_container, self.resources[action[1][0]], action[1][1], action[1][2])
            else:
                print "MESSAGE::Incorrect action: " + str(action)
        return self.test_result()
