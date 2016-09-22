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

ODATA = "@odata"
ID = "id"
TYPE = "type"


class ApiResource:
    def __init__(self, json_body, expected_odata_type):
        self.odata_id = json_body.get(".".join([ODATA, ID]))
        self.odata_type = json_body.get(".".join([ODATA, TYPE]))
        self.odata_type = self.odata_type.replace("#", "") if self.odata_type else self.odata_type
        self.expected_odata_type = expected_odata_type
        self.body = json_body

        if (not self.odata_id):
            print "WARNING:: @odata.id not present in %s. Setting 'None'." % json_body

        if (not self.odata_type):
            print "WARNING:: @odata.type not present in %s. Setting 'None'." % json_body
