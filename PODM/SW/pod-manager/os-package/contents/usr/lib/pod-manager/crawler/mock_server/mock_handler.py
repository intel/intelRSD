# Copyright (c) 2016-2018 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import http.server
import json
import urllib.parse


class MockHandler(http.server.BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path.endswith('/'):
            self.path = self.path[:-1]

        parsed_uri = urllib.parse.urlsplit(self.path)
        path = parsed_uri.path

        resource = self.server.service.find_resource(path)

        if resource is not None:
            self.send_response(resource.get_status_code())

            for key in resource.get_headers():
                if key != 'Connection' and key != 'Transfer-Encoding':
                    self.send_header(key, resource.get_headers()[key])

            if 'Date' in resource.get_headers():
                self.send_header('_Date', resource.get_headers()['Date'])

            self.end_headers()

            try:
                content = json.dumps(resource.get_body(), indent=4)
            except:
                content = resource.get_body()

            self.wfile.write(content.encode('utf-8'))
        else:
            self.send_response(404)
            self.send_header('Content-Type', 'text/html')
            self.end_headers()
            self.wfile.write('That resource was not mapped'.encode('utf-8'))

    def log_message(self, formatting, *args):
        return
