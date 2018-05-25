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

import logging

from data_model.resource import Resource


class ResourceRetriever:
    _authorization_header = {'Authorization': 'Basic {}'.format('YWRtaW46YWRtaW4=')}

    @staticmethod
    def retrieve_resource(connection, path):
        logging.debug('Retrieving RESOURCE: {path}'.format(path=path))
        connection.request('GET', path, headers=ResourceRetriever._authorization_header)
        response = connection.getresponse()
        body_encoded = response.read()
        body = body_encoded.decode('utf-8')
        status_code = response.status
        headers = response.getheaders()
        resource = Resource(path, status_code, headers, body)
        return resource
