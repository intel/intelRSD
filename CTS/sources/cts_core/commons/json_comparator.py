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
from json import loads, dumps

from cts_core.commons.error import cts_error


class JsonComparator:
    def __init__(self):
        pass

    @staticmethod
    def compare_json_to_golden(json, golden, ignore = []):
        """
        Recursively compares two jsons while ignoring the fields present in ignore list and the ones not present in golden.
        :param json: json to validate
        :param golden: the golden json (its fields have to match the given json's or be present in ignore list)
        :param ignore: list() of parameters to ignore (them not matching will not generate errors), if the element is nested,
        the convention is: "/dict1/dict2/.../parameter"
        :return: comparison status
        :rtype: bool
        """
        def compare_recursively(prefix, golden, json, hide_errors=None):
            status = True
            if type(golden) in [dict, OrderedDict]:
                if type(json) not in [dict, OrderedDict]:
                    cts_error("{prefix} should be a dict", **locals())
                    status = False
                else:
                    for key in golden.keys():
                        if "{}/{}".format(prefix, key) in ignore:
                            continue
                        if key in json:
                            if not compare_recursively(prefix = "{}/{}".format(prefix, key), golden=golden[key], json=json[key]):
                                status = False
                        else:
                            cts_error("Key {key} not present in the compared json", key=key)
                            status = False
            elif type(golden) == list:
                if type(json) != list:
                    cts_error("{prefix} should be a list", **locals())
                    status = False
                else:
                    for golden_key in golden:
                        key_found = False
                        for key in json:
                            if compare_recursively('', golden_key, key, hide_errors=True):
                                key_found = True
                                break
                        if not key_found:
                            cts_error("{prefix} list element {golden_key} not found in the other json", **locals())
                            status = False
                            break
            elif not JsonComparator.compare_jsons(golden, json, hide_errors=hide_errors):
                status = False

            return status

        return compare_recursively('', golden, json)

    @staticmethod
    def compare_jsons(json_a, json_b, hide_errors=None):
        """
        :return: comparison status
        :rtype: bool
        """
        if type(json_a) != type(json_b):
            # considering string is the same as unicode for jsons
            if not (isinstance(json_a, basestring) and isinstance(json_b, basestring)):
                cts_error("Mismatch between {a} ({a_type}) and {b} ({b_type}) types",
                          a=json_a, a_type=type(json_a), b=json_b, b_type=type(json_b))
                return False

        try:
            compare_func = {dict: JsonComparator.compare_dicts, list: JsonComparator.compare_lists}[type(json_a)]
        except KeyError:
            compare_func = JsonComparator.compare_variables

        return compare_func(json_a, json_b, hide_errors=hide_errors)

    @staticmethod
    def compare_lists(list_a, list_b, hide_errors=None):
        """
        :type list_a: list[]
        :type list_b: list[]
        :rtype: bool
        """
        status = True
        if len(list_a) != len(list_b):
            if not hide_errors:
                cts_error("lists {list_a!r} and {list_b!r} have different length", **locals())
            return False

        list_a = sorted(list_a)
        list_b = sorted(list_b)
        for it in range(len(list_a)):
            if not JsonComparator.compare_jsons(list_a[it], list_b[it], hide_errors=hide_errors):
                status = False

        return status

    @staticmethod
    def compare_dicts(dict_a, dict_b, hide_errors=None):
        """
        :dict dictA: dict[]
        :dict dictB: dict[]
        :rtype: bool
        """
        if len(dict_a.keys()) != len(dict_b.keys()):
            if not hide_errors:
                cts_error("Dictionaries {dict_a!r} and {dict_b!r} have different keys numbers", **locals())
            return False
        status = True

        for key in dict_a.keys():
            try:
                if not JsonComparator.compare_jsons(dict_a[key], dict_b[key], hide_errors=hide_errors):
                    status = False
            except KeyError:
                if not hide_errors:
                    cts_error("Key {key} present in dictionary {dict_a!r} but not present in dictionary {dict_b!r}",
                              **locals())
                status = False

        return status

    @staticmethod
    def compare_variables(var_a, var_b, hide_errors):
        """
        :rtype: bool
        """
        float_epsilon = 1e-8

        if type(var_a) == float and abs(var_a - var_b) < float_epsilon:
            var_a = var_b

        if var_a != var_b:
            if not hide_errors:
                cts_error("Mismatch of values {var_a} and {var_b}", **locals())

        return var_a == var_b

    @staticmethod
    def odict_to_dict(ordered_dict):
        return loads(dumps(ordered_dict))
