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
import pickle
from copy import copy
from os import environ, getenv

from bs4 import BeautifulSoup
from jsonpointer import resolve_pointer, JsonPointerException

from cts_core.commons.api_caller import ApiCaller
from cts_core.commons.error import cts_error, cts_warning
from cts_core.commons.json_helpers import get_odata_type, is_special_property
from cts_core.discovery.api_resource import ApiResource
from cts_core.discovery.discovery_container import DiscoveryContainer
from cts_core.measure.performance_information import PerformanceData, performance_measure
from cts_core.metadata.model.metadata_types.dynamic_properties import match_dynamic_property
from cts_core.metadata.model.metadata_types.metadata_type_categories import MetadataTypeCategories
from cts_core.metadata.model.property import Property
from cts_core.validation.validation_status import ValidationStatus
from cts_framework.commons.enums import RequestStatus

ODATA = "@odata"
TYPE = "type"
ID = "id"


class Path:
    def __init__(self):
        self.path = []

    def append(self, property):
        path = Path()
        path.path = copy(self.path)
        path.path.append(property)
        return path

    def __str__(self):
        return '.'.join(self.path)


class ApiExplorer:
    def __init__(self, metadata_container, configuration):
        """
        :type metadata_container: cts_core.metadata.metadata_container.MetadataContainer
        :type configuration:
        """
        self._metadata_container = metadata_container
        self._api_caller = ApiCaller(configuration)
        self._config_property_reader = configuration
        self._status = ValidationStatus.UNKNOWN
        self._discovery_container = None
        self._bfs_queue = []
        self._performance = []

    def _add_duration_for_object(self, object, duration, method, status):
        self._performance.append(PerformanceData(object, duration, method, status))

    def get_performance_information(self):
        return self._performance

    @property
    def get_average_duration(self):
        return self._calculate_average_duration()

    def _calculate_average_duration(self):
        pass

    def discover(self, url, expected_odata_type, discovery_container=None,
                 api_endpoint_override=None):
        """
        :type discovery_container: cts_core.discovery.discovery_container.DiscoveryContainer
        :rtype: (cts_core.discovery.discovery_container.DiscoveryContainer, cts_core.validation.validation_status.ValidationStatus)
        """
        self._status = ValidationStatus.PASSED
        if getenv('CTS_UNPICKLE', None):
            self._discovery_container = pickle.load(open(environ['CTS_UNPICKLE'], 'rb'))
            self._discovery_container.metadata_container = self._metadata_container
            return self._discovery_container, self._status

        self._discovery_container = discovery_container if discovery_container is not None \
            else DiscoveryContainer(metadata_container=self._metadata_container)

        self._enqueue_resource(url, expected_odata_type, api_endpoint_override)

        while self._bfs_queue:
            self._explore_next_resource()

        if getenv('CTS_PICKLE', None):
            # workaroud for 'TypeError: can't pickle instancemethod objects'
            metadata_container_backup = self._discovery_container.metadata_container
            self._discovery_container.metadata_container = None  # do not pickle metadata container
            pickle.dump(self._discovery_container, open(environ['CTS_PICKLE'], 'wb'))
            self._discovery_container.metadata_container = metadata_container_backup

        return self._discovery_container, self._status

    def _enqueue_resource(self, url, expected_odata_type, api_endpoint_override):
        self._bfs_queue.append((url, expected_odata_type, api_endpoint_override))

    def _explore_next_resource(self):
        if self._bfs_queue:
            url = self._bfs_queue[0][0]
            expected_odata_type = self._bfs_queue[0][1]
            api_endpoint_override = self._bfs_queue[0][2]
            self._bfs_queue.pop(0)
            self._explore_resource(url, expected_odata_type, api_endpoint_override)

    def _explore_resource(self, url, expected_odata_type, api_endpoint_override):
        """
        :type url: str
        """
        if url is None:
            return

        link = self._api_caller.links_factory.get_resource_link(url,
                                                                api_endpoint_override=api_endpoint_override)
        url = link.link

        if self._discovery_container.is_visited(url):
            return

        self._discovery_container.register_url_pattern(url, expected_odata_type)

        link, status, response_body, _ = self._get_resource(url,
                                                            api_endpoint_override=api_endpoint_override)

        if link is None:
            cts_error("GET {id:id} Response from service is not correct. CTS can not verify this endpoint", id=url)
            self._status = ValidationStatus.FAILED
            return

        # try to detect all others odata_id
        self._get_all_odata_id(response_body, api_endpoint_override)

        if not response_body:
            cts_error("GET {id:id} Empty response body", id=url)
            self._status = ValidationStatus.FAILED
            return

        if status == RequestStatus.SUCCESS and response_body:
            api_resource = ApiResource(link.link, link.netloc, response_body, expected_odata_type)
            self._discovery_container.add_resource(api_resource)
            self._process_resource(api_resource)

    def _get_all_odata_id(self, response_body, api_endpoint_override):
        for newly_discovered_odata in self._find_all_odata_id(dict(response_body)):
            short_link = newly_discovered_odata[1][0]
            link = self._api_caller.links_factory.get_resource_link(short_link,
                                                                    api_endpoint_override=api_endpoint_override)
            if self._discovery_container.is_visited(link.link):
                continue

            _, status, _, response_body, _ = self._api_caller.get_resource(short_link,
                                                                           None,
                                                                           api_endpoint_override=api_endpoint_override)
            if status == RequestStatus.SUCCESS and response_body:
                self._enqueue_resource(short_link,
                                       self._clear_type(response_body),
                                       api_endpoint_override=api_endpoint_override)

    @staticmethod
    def _clear_type(response_body):
        raw_odata_type = None
        try:
            raw_odata_type = response_body["@odata.type"].replace("#", "")
            if len(raw_odata_type.split(".")) == 3:
                return '.'.join((raw_odata_type[0], raw_odata_type[2]))
        finally:
            return raw_odata_type

    def _find_all_odata_id(self, json_soup):
        flat_dict = list(self.__transform_soup_into_flat_list(json_soup))
        return [(i.split('=')[0], i.split('=')[1:]) for i in flat_dict if i.split('=')[0] == "@odata.id"]

    def __transform_soup_into_flat_list(self, json_soup):
        try:
            for soup_key, soup_values in json_soup.items():
                if isinstance(soup_values, dict):
                    for flat_list_element in self.__transform_soup_into_flat_list(soup_values):
                        yield flat_list_element
                elif isinstance(soup_values, list):
                    for value in soup_values:
                        for flat_list_element in self.__transform_soup_into_flat_list(value):
                            yield flat_list_element
                else:
                    yield str('{key}={value}'.format(key=soup_key, value=soup_values))
        except:
            pass

    @performance_measure
    def _get_resource(self, url, api_endpoint_override=None):
        link, status, status_code, response_body, headers = self._api_caller.get_resource(url,
                                                                                          self._discovery_container,
                                                                                          api_endpoint_override=api_endpoint_override)

        if status != RequestStatus.SUCCESS:
            print "ERROR::Error while executing GET %s" % url
            self._status = ValidationStatus.FAILED

        status, body = self._dereference_jsonpointer_if_any(url, response_body)
        return link, status, body, status_code

    def _dereference_jsonpointer_if_any(self, url, response_body):
        pointer_pos = url.find("#")
        if pointer_pos != -1:
            pointer = url[pointer_pos + 1:]
            if not pointer or pointer[0] != '/':
                pointer = '/' + pointer
            try:
                return RequestStatus.SUCCESS, resolve_pointer(response_body, pointer)
            except JsonPointerException as exception:
                pointer_response_body = self._check_resource_list(response_body, pointer, url)
                if not pointer_response_body:
                    cts_error("JSON pointer exception while dereferencing {path} from {url:id} "
                              "resource; Error: {error}",
                              path=pointer, url=url, error=exception)
                    self._status = ValidationStatus.FAILED
                    return RequestStatus.FAILED, {}
                return RequestStatus.SUCCESS, pointer_response_body
        return RequestStatus.SUCCESS, response_body

    @staticmethod
    def _check_resource_list(response_body, pointer, url):
        cts_warning("Could not find requestes resource {path} from {url:id} by JSON pointer dereferencing. "
                    "Trying to use alternative, non-RSD based method using requested @odata.id",
                    path=pointer, url=url)
        dissected_pointer = pointer.rsplit("/", 1)
        resource_list = resolve_pointer(response_body, dissected_pointer[0])
        if isinstance(resource_list, list):
            requested_resource_odata = url[url.find("/redfish"):]
            for resource in resource_list:
                if resource["@odata.id"] == requested_resource_odata:
                    return resource
        cts_error("Could not find {path} from {url:id} with search by @odata.id",
                  path=pointer, url=url)
        return None

    def _process_resource(self, api_resource):
        """
        :type api_resource: cts_core.discovery.api_resource.ApiResource
        """
        try:
            type_definition = self._metadata_container.entities[api_resource.odata_type]
        except KeyError as key:
            cts_error("{url:id}: Unknown @odata.type {type}. Not able to process sub-elements",
                      url=api_resource.url, type=key)
            self._status = ValidationStatus.FAILED
            return
        except AttributeError as attribute_error:
            cts_error("{url:id}: Missing entities in response {type}. Not able to process sub-elements",
                      url=api_resource.url, type=attribute_error)
            self._status = ValidationStatus.FAILED
            return

        try:
            self._process_properties(api_resource.body,
                                     type_definition,
                                     url=api_resource.url)
        except Exception as err:
            self.status = ValidationStatus.FAILED
            cts_error("{id:id} Unexpected error {err:exception}", id=api_resource.url, **locals())

    def _process_properties(self, json_body, type_definition, url, path=None):
        if path is None:
            path = Path()

        property_description_list = type_definition.all_properties

        self._append_additional_properties(property_description_list, json_body, type_definition,
                                           url, path)

        for property_description in property_description_list:
            try:
                property_body = json_body[property_description.name]
            except KeyError:
                # this is discovery phase
                # any structure errors must be reported during subsequent analysis
                continue  # next property
            try:
                if property_description.is_collection:
                    for property_body_member in property_body:
                        self._process_property(property_description,
                                               property_body_member,
                                               url,
                                               path)
                else:
                    self._process_property(property_description,
                                           property_body,
                                           url,
                                           path)
            except (TypeError, KeyError):
                # this is discovery phase
                # any structure errors must be reported during subsequent analysis
                pass

    def _append_additional_properties(self, property_list, body, type_definition, url, path):
        additionals = set(body.keys()) - set([property.name for property in property_list])
        # filter out special properties (properties that contain @ and #)
        additionals = filter(lambda property: not is_special_property(property), additionals)

        if len(additionals) > 0:
            if type_definition.allow_additional_properties:
                for property_name in additionals:
                    # process only objects
                    try:
                        property_value = body[property_name]
                        if isinstance(property_value, dict):
                            odata_type = match_dynamic_property(property_name, type_definition)
                            if odata_type is None:
                                try:
                                    odata_type = get_odata_type(property_value)
                                except KeyError as key:
                                    cts_error(
                                        "{url:id}#{path}: @odata.type not found in complex "
                                        "additional property",
                                        url=url, path=path.append(property_name))
                                    continue

                            adhoc_description = self.ad_hoc_type_definition(property_name, odata_type)
                            property_list.append(adhoc_description)
                    except KeyError:
                        pass
            else:
                cts_error("{url:id}#{path}: Object of type {type}; unexpected properties: [{"
                          "properties}]",
                          type=type_definition.name,
                          properties=", ".join(additionals),
                          url=url,
                          path=path
                          )
                self._status = ValidationStatus.FAILED

    def ad_hoc_type_definition(self, property_name, odata_type):
        raw_soup = """
                    <Property Name="{name}" Type="{type}">
                    </Property>
                   """
        soup = BeautifulSoup(raw_soup.format(name=property_name, type=odata_type),
                             "lxml").find_all("property")[0]
        adhoc_description = Property(self._metadata_container, None, soup)
        return adhoc_description

    def _process_property(self, property_description, json_body, url, path):
        if not json_body:
            return

        if property_description.type in self._metadata_container.entities.keys():
            return self._process_entity(url,
                                        property_description,
                                        json_body,
                                        path=path.append(property_description.name))
        elif property_description.type in self._metadata_container.types.keys():
            if self._metadata_container.types[property_description.type].type_category == \
                    MetadataTypeCategories.COMPLEX_TYPE:
                return self._process_complex_type(property_description,
                                                  json_body,
                                                  url,
                                                  path=path.append(property_description.name))
        else:
            cts_error("{url:id}#{path} : Unknown type {type}",
                      url=url,
                      path=path,
                      type=property_description.type)

    def _process_entity(self, url, property_description, body, path):
        try:
            odata_id = body[".".join([ODATA, ID])]
        except KeyError:
            cts_error("{odata_id:id}#{path}: @odata.id expected in the resource body", odata_id=odata_id,
                      path=path)
            return

        try:
            if odata_id is not None:
                netloc = self._discovery_container.get_netloc(url)
                self._enqueue_resource(odata_id, property_description.type, netloc)

        except Exception as err:
            self.status = ValidationStatus.FAILED
            cts_error("{odata_id:id} Exception {err:exception}. Not able to process sub-elements",
                      **locals())
            self._status = ValidationStatus.FAILED

    def _process_complex_type(self, property_description, body, url, path):
        try:
            # use override @odata.type if present.
            # At this stage do not verify if override type is consistent with type from property_description;
            # This will be validated in validate_get_responses test
            odata_type = get_odata_type(body)
        except KeyError:
            odata_type = property_description.type

        # no need to verify @odata.type if there is on ignore types list
        if odata_type not in self._metadata_container.get_ignored_types:
            type_definition = self._metadata_container.types[odata_type]
            self._process_properties(body, type_definition, url, path)
