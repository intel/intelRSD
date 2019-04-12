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

from cts_core.discovery.api_resource import ApiResource, ApiResourceProxy
from cts_core.discovery.discovery_container import DiscoveryContainer
from unit_tests.helpers.stdout_capture import StdoutCapture


class ApiResourceProxyTest(unittest.TestCase):
    def test_reference_is_recreated(self):
        container = DiscoveryContainer()
        container.add_resource(ApiResource("id_1", 'netloc', {'@odata.id': 'id1',
                                                              'Oem':
                                                                  {'Intel_RackScale':
                                                                       {'Location':
                                                                            {'Id': 1, 'ParentId': 2}
                                                                        }
                                                                   }
                                                              }, None))
        container.add_resource(ApiResource("id_2", 'netloc', {'@odata.id': 'id2',
                                                              'Oem':
                                                                  {'Intel_RackScale':
                                                                       {'Location':
                                                                            {'Id': 3, 'ParentId': 4}
                                                                        }
                                                                   }
                                                              }, None))

        res = container["id_2"]
        self.assertTrue(isinstance(res, ApiResourceProxy))
        self.assertEquals(res.odata_id, "id2")
        container.delete_resource("id_2")
        with StdoutCapture() as output:
            with self.assertRaises(AttributeError):
                res.odata_id
        self.assertIn('ERROR::ApiResources id_2 is no longer valid', output.raw)
