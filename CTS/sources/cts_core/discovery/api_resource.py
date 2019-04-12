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
import weakref
from collections import namedtuple
from os.path import dirname

from cts_core.commons.error import cts_warning, cts_error
from cts_core.commons.json_helpers import sanitize_odata_type
from cts_core.metadata.literals import *

ALLOWABLE = "@Redfish.AllowableValues"
Location = namedtuple('Location', 'id parent_id')


class ApiResourceProxy:
    """ Proxy for ApiResource objects.
        This is fix for dangling api resource references that are created when resource is recreated and re-inserted
        into discovery container. The proxy object holds weak reference that points to None when underlying api
        resource has been collected. This 'None' is being captured and valid reference is being recreated.
    """

    def __init__(self, api_resource, discovery_container):
        self.url = api_resource.url
        self.ref = weakref.ref(api_resource)
        self.discovery_container = discovery_container

    def __getattr__(self, name):
        obj = self._dereference()
        return getattr(obj, name)

    def _dereference(self):
        obj = self.ref()
        if obj is None:
            try:
                obj = self.discovery_container[self.url]
                self.ref = weakref.ref(obj)
            except KeyError:
                cts_error('ApiResources {url} is no longer valid', url=self.url)
                return None
        return obj


class ApiResource:
    """
    Represents API Resource exposed on REST service.
    """
    def __init__(self, url, netloc, json_body, expected_odata_type):
        """
        :param url: URL of api resource to add
        :type url: str
        :param netloc: address and port of the API service that owns the resource
        :type netloc: str
        :param json_body: Python representation of API resource
        :type json_body: OrderedDict
        :param expected_odata_type: When CTS issues GET request, it usually expects a resource with a specific
        @odata.type
        :type expected_odata_type: str
        """
        self.url = url
        self.netloc = netloc
        self.odata_id = json_body.get(".".join([ODATA, ID]))
        self.odata_type = sanitize_odata_type(json_body.get(".".join([ODATA, TYPE])))
        if self.odata_type is None:
            self.odata_type = sanitize_odata_type(expected_odata_type)
        self.expected_odata_type = sanitize_odata_type(expected_odata_type)
        self.body = json_body
        self.location = ApiResource._get_location(json_body)
        self.contains = ApiResource._get_contains(json_body)
        self.contained_by = ApiResource._get_contained_by(json_body)
        self.parent_url = self._get_parent_url()

        if not self.odata_id:
            cts_warning("@odata.id property not present in {body:response_body}. Setting 'None'.", body=json_body)

        if not self.odata_type:
            cts_warning("@odata.type property not present in {body:response_body}. Setting 'None'.", body=json_body)

    @staticmethod
    def _get_location(json_body):
        """
        :type json_body: dict
        """
        try:
            location_property = json_body[OEM][INTEL_RACKSCALE][LOCATION]
            location = Location(location_property[LOCATION_ID], location_property[PARENT_ID])
            return location
        except Exception:
            return None

    @staticmethod
    def _get_contains(json_body):
        """
        :type json_body: dict
        :rtype: list(str)
        """
        try:
            contains_property = json_body[LINKS][CONTAINS]
            contains = [m[ODATA_ID] for m in contains_property]
            return contains
        except Exception:
            return None

    @staticmethod
    def _get_contained_by(json_body):
        """
        :type json_body: dict
        :rtype: str
        """
        try:
            contained_by_property = json_body[LINKS][CONTAINED_BY]
            contained_by = contained_by_property[ODATA_ID]
            return contained_by
        except Exception:
            return None

    def _get_parent_url(self):
        """
        :rtype: str
        """
        if not self.url:
            return None
        sane_url = self.url
        if self.url[-1] == '/':
            sane_url = self.url[:-1]
        return dirname(sane_url)

    def get_value_from_path(self, variable_path):
        """
        :type variable_path: list[str or int]
        :rtype: *
        """
        partial_path = []
        value = self.body
        try:
            for path_element in variable_path:
                partial_path.append(str(path_element))
                if value is None:
                    partial_path_s = "->".join(partial_path)
                    raise KeyError(partial_path_s)
                value = value[path_element]
        except KeyError as key:
            raise KeyError(key)
        except IndexError as key:
            raise KeyError(key)

        return value


    def get_allowable_from_path(self, variable_path):
        """
        :type variable_path: list[str or int]
        :rtype: *
        """
        try:
            value = self.get_value_from_path(variable_path[:-1])
            allowable_values_property = variable_path[-1] + ALLOWABLE
            allowable_values = value[allowable_values_property]
            return allowable_values
        except:
            return []
