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


class JsonComparator:
    def __init__(self):
        pass

    @staticmethod
    def compare_jsons(jsonA, jsonB):
        """
        :return: comparation status
        :rtype: bool
        """
        if type(jsonA) != type(jsonB):
            print "ERROR:: missmatch between %s and %s types" % (jsonA, jsonB)
            return False

        try:
            compare_func = {dict: JsonComparator.compare_dicts, list:JsonComparator.compare_lists}[type(jsonA)]
        except KeyError:
            compare_func = JsonComparator.compare_variables

        return compare_func(jsonA, jsonB)

    @staticmethod
    def compare_lists(listA, listB):
        """
        :type listA: list[]
        :type listB: list[]
        :return: comparation status
        :rtype: bool
        """
        status = True
        if len(listA) != len(listB):
            print "ERROR::lists %s and %s have different length"
            return False

        listA = sorted(listA)
        listB = sorted(listB)
        for it in range(len(listA)):
            if not JsonComparator.compare_jsons(listA[it], listB[it]):
                status = False

        return status

    @staticmethod
    def compare_dicts(dictA, dictB):
        """
        :dict dictA: dict[]
        :dict dictB: dict[]
        :return: comparation status
        :rtype: bool
        """
        if len(dictA.keys()) != len(dictB.keys()):
            print "ERROR: dictionaries %s and %s have different keys numbers" % (dictA, dictB)
            return False
        status = True

        for key in dictA.keys():
            try:
                if not JsonComparator.compare_jsons(dictA[key], dictB[key]):
                    status = False
            except KeyError:
                print "ERROR::Key %s present in dictionary %s but not present in dictionary %s" % (key, dictA, dictB)
                status = False

        return status

    @staticmethod
    def compare_variables(varA, varB):
        """
        :return: comparation status
        :rtype: bool
        """
        if varA != varB:
            print "ERROR::mismatch of values %s and %s" % (varA, varB)
        return varA == varB
