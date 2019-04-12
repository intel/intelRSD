"""
 * @copyright
 * Copyright (c) 2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION
"""

import httplib
import json
from fpgaof_helpers.uuid_generator import UuidGenerator
from fpgaof_helpers.message import Message


class JsonRpc:
    def __init__(self, url, uuid=UuidGenerator().get_uuid()):
        self.uuid = uuid
        self.url = url

    def send_post(self, payload):
        try:
            connection = httplib.HTTPConnection(self.url)
            connection.request('POST', "", json.dumps(payload))

            response = connection.getresponse()
        except:
            Message.error("Unable to send POST to {}".format(self.url))
            return None

        if response.status != 200:
            Message.error("Unexpected status code: {}".format(response.status))
            return None

        json_data = json.loads(response.read())

        if json_data.get("error", None):
            Message.error("Response error message: {}".format(json_data["error"]["message"]))
            return None
        return json_data["result"]

    def call(self):
        request = {"jsonrpc": "2.0", "id": 1, "method": "discover", "params": {"host_id": self.uuid}}

        return self.send_post(request)
