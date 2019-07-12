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
from collections import namedtuple
from urlparse import urlsplit, urlunsplit


class LinksConstants:
    REDFISH = "redfish"
    VERSION = "v1"
    HTTPS = "https"
    HTTP = "http"


Link = namedtuple("Link", ["link", "netloc"])


class LinksFactory:
    def __init__(self, api_endpoint, ssl=None):
        """
        :type api_endpoint: str
        """
        self.api_endpoint = api_endpoint
        self.scheme = LinksConstants.HTTPS if ssl else LinksConstants.HTTP

    def get_resource_link(self, url, api_endpoint_override=None):
        parts = urlsplit(url)
        scheme = parts.scheme if parts.scheme else self.scheme
        netloc = parts.netloc if parts.netloc else (
            api_endpoint_override if api_endpoint_override else self.api_endpoint
        )

        path = urlunsplit((scheme,
                           netloc,
                           parts.path,
                           parts.query,
                           parts.fragment))

        return Link(path, netloc)
