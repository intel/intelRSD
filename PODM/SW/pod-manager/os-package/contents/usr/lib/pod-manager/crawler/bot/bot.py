# Copyright (c) 2016-2017 Intel Corporation
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

from http.client import HTTPSConnection
from http.client import HTTPConnection

from bot.resource_retriever import ResourceRetriever
from bot.security_context_initializer import SecurityContextInitializer
from data_model.protocols import Protocols
from data_model.service import Service
from utils.json_utils import JsonUtils


class Bot:
    _security_context = SecurityContextInitializer.init_security_context()
    _neighbours_key = '@odata.id'

    def __init__(self, service_description):
        self._service_description = service_description

        host = self._service_description.get_service_localization().get_host()
        port = self._service_description.get_service_localization().get_port()

        if service_description.get_service_parameters().get_protocol() == Protocols.HTTPS:
            self._connection = HTTPSConnection(
                host=host,
                port=port,
                context=Bot._security_context
            )
        else:
            self._connection = HTTPConnection(
                host=host,
                port=port
            )

    def crawl(self):
        paths_queued = [self._service_description.get_service_parameters().get_root_path()]
        paths_visited = []

        service = Service(self._service_description)

        while len(paths_queued) > 0:
            path = paths_queued.pop()
            paths_visited.append(path)

            try:
                resource = ResourceRetriever.retrieve_resource(self._connection, path)
            except:
                continue

            service.add_resource(resource)

            try:
                node_json = JsonUtils.loads(resource.get_body())

                neighbour_paths = JsonUtils.extract_descendants(node_json, Bot._neighbours_key)

                for neighbour_path in neighbour_paths:
                    if neighbour_path not in paths_visited:
                        if neighbour_path is not None:
                            paths_queued.append(neighbour_path)
            except:
                continue

        return service
