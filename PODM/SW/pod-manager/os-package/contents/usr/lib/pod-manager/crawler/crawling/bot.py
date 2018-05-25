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

import http.client
import json
import time

from crawling.resource_retriever import ResourceRetriever
from data_model.protocols import Protocols
from data_model.service import Service
from utils.json_utils import JsonUtils


class Bot:
    _security_context = None
    _neighbours_key = '@odata.id'

    @staticmethod
    def set_security_context(security_context):
        Bot._security_context = security_context

    def __init__(self, service_description):
        self._service_description = service_description

        host = self._service_description.get_service_localization().get_host()
        port = self._service_description.get_service_localization().get_port()

        if service_description.get_service_parameters().get_protocol() == Protocols.HTTPS:
            self._connection = http.client.HTTPSConnection(
                host=host,
                port=port,
                context=Bot._security_context
            )
        else:
            self._connection = http.client.HTTPConnection(
                host=host,
                port=port
            )

    def crawl(self):
        paths_queued = [self._service_description.get_service_parameters().get_root_path()]
        paths_visited = set()

        service = Service(self._service_description)

        while len(paths_queued) > 0:
            path = paths_queued.pop(0)

            if path in paths_visited:
                continue
            else:
                paths_visited.add(path)

            try:
                begin = time.time()
                resource = ResourceRetriever.retrieve_resource(self._connection, path)
                end = time.time()
                duration = end - begin
                resource.set_retrieval_duration(duration)
            except:
                continue

            service.add_resource(resource)

            try:
                node_json = json.loads(resource.get_body())

                neighbour_paths = sorted(JsonUtils.extract_descendants(node_json, Bot._neighbours_key))
                neighbour_paths.reverse()

                for neighbour_path in neighbour_paths:
                    if neighbour_path is not None:
                        neighbour_path_splited = neighbour_path.split('#')
                        paths_queued.insert(0, neighbour_path_splited[0])
            except:
                continue

        return service
