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
import unittest

from cts_core.discovery.api_resource import ApiResource
from unit_tests.helpers.stdout_capture import StdoutCapture

class ApiResourceTest(unittest.TestCase):
    def test_location_set_to_null_when_not_defined(self):
        self.assertIsNone(ApiResource('link', 'netloc', {}, None).location)

    def test_location_set_to_null_when_definition_is_invalid(self):
        self.assertIsNone(ApiResource('url', 'netloc', {'Oem' : 1}, None).location)
        self.assertIsNone(ApiResource('url', 'netloc', {'Oem': {}}, None).location)
        self.assertIsNone(ApiResource('url', 'netloc', {'Oem': {'Intel_RackScale' : 2}}, None).location)
        self.assertIsNone(ApiResource('url', 'netloc', {'Oem': {'Intel_RackScale': {}}}, None).location)
        self.assertIsNone(ApiResource('url', 'netloc', {'Oem': {'Intel_RackScale': {'Location' : 2}}}, None).location)
        self.assertIsNone(ApiResource('url', 'netloc', {'Oem': {'Intel_RackScale': {'Location': {}}}}, None).location)
        self.assertIsNone(ApiResource('url', 'netloc', {'Oem': {'Intel_RackScale': {'Location': {'Id' : 2}}}}, None).location)
        self.assertIsNone(ApiResource('url', 'netloc', {'Oem': {'Intel_RackScale': {'Location': {'ParentId': 2}}}}, None).location)

    def test_location_set_correctly(self):
        self.assertIsNotNone(ApiResource('url', 'netloc', {'Oem': {'Intel_RackScale': {'Location': {'Id' : 1, 'ParentId': 2}}}}, None).location)

    def test_contains_set_to_null_when_not_defined(self):
        self.assertIsNone(ApiResource('url', 'netloc', {}, None).contains)

    def test_contains_set_to_null_when_definition_is_invalid(self):
        self.assertIsNone(ApiResource('url', 'netloc', {'Links': 1}, None).contains)
        self.assertIsNone(ApiResource('url', 'netloc', {'Links': {'Contains': 1}}, None).contains)
        self.assertIsNone(ApiResource('url', 'netloc', {'Links': {'Contains': [1]}}, None).contains)
        self.assertIsNone(ApiResource('url', 'netloc', {'Links': {'Contains': [{1 : 2}]}}, None).contains)

    def test_contains_set_correctly(self):
        self.assertIsNotNone(ApiResource('url', 'netloc', {'Links': {'Contains': []}}, None).contains)
        self.assertEqual(1, len(ApiResource('url', 'netloc', {'Links': {'Contains': [{"@odata.id" : 1}]}}, None).contains))

        contains = ApiResource('url', 'netloc', {'Links': {'Contains': [{"@odata.id": 'id_1'}, {"@odata.id": 'id_2'}]}}, None).contains
        self.assertEqual(2, len(contains))
        self.assertIn('id_1', contains)
        self.assertIn('id_2', contains)


    def test_contained_by_set_to_null_when_not_defined(self):
        self.assertIsNone(ApiResource('url', 'netloc', {}, None).contained_by)

    def test_contained_by_set_to_null_when_definition_is_invalid(self):
        self.assertIsNone(ApiResource('url', 'netloc', {'Links': 1}, None).contained_by)
        self.assertIsNone(ApiResource('url', 'netloc', {'Links': {'ContainedBy': 1}}, None).contained_by)
        self.assertIsNone(ApiResource('url', 'netloc', {'Links': {'ContainedBy': []}}, None).contained_by)
        self.assertIsNone(ApiResource('url', 'netloc', {'Links': {'ContainedBy': {"foo" : "bar"}}}, None).contained_by)

    def test_contained_by_set_correctly(self):
        self.assertEqual("alamakota", ApiResource('url', 'netloc', {'Links': {'ContainedBy': {"@odata.id" : "alamakota"}}}, None).contained_by)

    def test_allowable(self):
        resource = ApiResource(
            'url', 'netloc',
            {
                'a' : 'A',
                'b' : 'B',
                'b@Redfish.AllowableValues' : [
                    'B',
                    'C'
                ],
                'c' : {
                    'ca' : 1,
                    'ca@Redfish.AllowableValues' : [
                        1,
                        2
                    ]
                }
            }
        , None)
        self.assertEqual([], resource.get_allowable_from_path(['a']))
        self.assertEqual(['B', 'C'], resource.get_allowable_from_path(['b']))
        self.assertEqual([], resource.get_allowable_from_path(['c']))
        self.assertEqual([1,2], resource.get_allowable_from_path(['c', 'ca']))

    def test_parent_odata_id(self):
        child_parent_pairs = [
            ("", None),
            (None, None),
            ("/", ""),
            ("/anything", "/"),
            ("/some/resource/path", "/some/resource"),
            ("/some/resource/path/", "/some/resource"),
            ("/redfish/v1/EthernetSwitches/1/Ports/1/VLANs/1", "/redfish/v1/EthernetSwitches/1/Ports/1/VLANs"),
            ("/redfish/v1/EthernetSwitches/1/Ports/1/VLANs/", "/redfish/v1/EthernetSwitches/1/Ports/1"),
            ("/redfish/v1/Chassis/cr1-1/Thermal#/Temperatures/1", "/redfish/v1/Chassis/cr1-1/Thermal#/Temperatures")
        ]
        with StdoutCapture() as output: # some warnings for Nones and empty strings expected
            for pair in child_parent_pairs:
                resource = ApiResource(pair[0], 'netloc', { '@odata.id' : pair[0] }, None)
                self.assertEqual(resource._get_parent_url(), pair[1])
