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

from cts_core.metadata_validation.get.validators.type_validator import ComplexTypeValidator


class MetadataGetValidator:
    def __init__(self, metadata_container):
        self._metadata_container = metadata_container

    def validate(self, discovery_container):
        status = True
        for api_resource in discovery_container.values():
            status = self.validate_single_entity(api_resource) and status

        return status

    def validate_single_entity(self, api_resource):
        status = True
        if not api_resource or not api_resource.body:
            print "WARNING:: %s can't be analyzed - resource empty" % api_resource.odata_id
            return False

        try:
            entity_details = self._metadata_container.entities[api_resource.odata_type]
        except KeyError:
            print "ERROR:: Unable to find definition of entity %s for resource %s" % (api_resource.odata_type,
                                                                                      api_resource.odata_id)
            return False

        print "MESSAGE:: validating resource %s" % api_resource.odata_id
        for property_description in entity_details.properties + entity_details.navigation_properties:
            status = self._validate_property(api_resource, property_description) and status

        print "MESSAGE:: validating resource %s ended" % api_resource.odata_id

        return status

    def _validate_property(self, api_resource, property_description):
        try:
            complex_type_validator = ComplexTypeValidator(None, self._metadata_container)
            status = complex_type_validator.validate_property(property_description, api_resource.body)
        except Exception:
            print "WARNING:: Unexpected exception while handling %s" % api_resource.odata_id
            status = False
        return status
