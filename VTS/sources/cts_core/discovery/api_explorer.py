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

from cts_core.discovery.api_resource import ApiResource
from cts_core.discovery.api_caller import ApiCaller
from cts_core.metadata.metadata_enums import ValueTypesCategory

ODATA = "@odata"
TYPE = "type"
ID = "id"


class ApiExplorer:
    def __init__(self, odata_id, expected_odata_type, metadata_container, config_property_reader):
        self._odata_id = odata_id
        self._metadata_container = metadata_container
        self._expected_odata_type = expected_odata_type
        self._api_caller = ApiCaller(config_property_reader)
        self._config_property_reader = config_property_reader

    def process_with_resource(self, discovery_container):
        """
        :type discovery_container: cts_core.discovery.discovery_container.DiscoveryContainer
        :param discovery_container: discovery container containing already visited endpoints
        :return: discovery container updated for visited endpoint and all child elements
        :rtype: cts_core.discovery.discovery_container.DiscoveryContainer
        """
        if discovery_container.is_visited(self._odata_id):
            return discovery_container

        discovery_container.add_resource(self._odata_id)
        response_body = self._get_resource()
        api_resource = ApiResource(response_body, self._expected_odata_type)

        discovery_container.add_resource(self._odata_id, api_resource)

        discovery_container = self.process_subelements(api_resource, discovery_container)

        return discovery_container

    def _get_resource(self):
        status, status_code, response_body, headers = self._api_caller.get_resource(self._odata_id)

        return response_body

    def process_subelements(self, api_resource, discovery_container):
        try:
            entity = self._metadata_container.entities[api_resource.odata_type]
        except KeyError:
            return discovery_container
        try:

            discovery_container = self.process_properties(entity.navigation_properties + entity.properties,
                                                          api_resource.body, discovery_container)
        except Exception:
            import traceback
            print traceback.format_exc()

        return discovery_container

    def process_properties(self, properties_list, api_resource_body, discovery_container):
        for property_description in properties_list:
            try:
                property_body = api_resource_body[property_description.name]
            except KeyError:
                # any structure errors must be reported during analysis
                continue
            try:
                if property_description.is_collection:
                    for property_body_member in property_body:
                        discovery_container = self.process_split_element(property_description, property_body_member,
                                                                         discovery_container)
                else:
                    discovery_container = self.process_split_element(property_description, property_body,
                                                                     discovery_container)
            except TypeError:
                # any type errors must be reported during analysis
                pass

        return discovery_container

    def process_split_element(self, property_description, split_element, discovery_container):
        if not split_element:
            return discovery_container

        if property_description.type in self._metadata_container.entities.keys():
            return self.process_entity(property_description, split_element, discovery_container)
        elif self._metadata_container.types.get_type_category(property_description.type) == ValueTypesCategory.COMPLEX_TYPE:
                return self.process_complex_type(property_description, split_element, discovery_container)
        else:
            return discovery_container

    def process_entity(self, property_description, body, discovery_container):
        try:
            odata_id = body[".".join([ODATA, ID])]
        except KeyError:
            return discovery_container

        try:
            discovery_container= ApiExplorer(odata_id, property_description.type, self._metadata_container,
                                             self._config_property_reader).process_with_resource(discovery_container)
        except Exception:
            import traceback
            print traceback.format_exc()

        return discovery_container

    def process_complex_type(self, property_description, body, discovery_container):
        property_details = self._metadata_container.types[property_description.type]
        return self.process_properties(property_details.properties + property_details.navigation_properties, body,
                                       discovery_container)
