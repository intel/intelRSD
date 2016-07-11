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

class DiscoveryContainer(dict):
    def is_visited(self, odata_id):
        return odata_id in self.keys()

    def add_resource(self, odata_id, resource=None):
        """
        :type odata_id: str
        :type resource: cts_core.discovery.api_resource.ApiResource
        :return:
        """
        self[odata_id] = resource

    def create_api_stub(self):
        return  """
\"\"\"
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
\"\"\"

from flask import Flask
from flask import jsonify

app = Flask(__name__)
data = %s
@app.route('/', defaults={'path': ''})
@app.route('/<path:path>')
def catch_all(path):
    if path[:-1] == "/":
        path = path[:-1]

    try:
        return jsonify(data["/" + path])
    except KeyError:
        return jsonify(data["/" + path + "/"])

if __name__ == '__main__':
    app.run(debug=True)

""" % {key: value.body for key,value in self.iteritems()}
