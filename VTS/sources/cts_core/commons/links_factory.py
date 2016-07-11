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


class LinksConstants:
    REDFISH = "redfish"
    VERSION = "v1"


class LinksFactory:
    def __init__(self, api_endpoint):
        self.api_endpoint = api_endpoint

    def _build_link(self, link_elements):
        if link_elements[0].startswith("/"):
            link_elements[0] = link_elements[0][1:]

        return "/".join([self.api_endpoint] + link_elements)

    def get_root_url(self):
        return self._build_link([LinksConstants.REDFISH, LinksConstants.VERSION])

    def get_resource_link(self, odata_id):
        return self._build_link([str(odata_id)])
