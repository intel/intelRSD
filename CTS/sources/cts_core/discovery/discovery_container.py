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
import re
import pickle

from pandas import concat, DataFrame
from pandas.io.json import json_normalize
from urlparse import urlsplit, urlunsplit, urlparse

from cts_core.commons.json_helpers import split_path_into_segments
from cts_core.commons.error import cts_error, cts_warning, cts_message
from cts_core.commons.odata_id_generalized_mapper import OdataIdGeneralizedMapper
from cts_core.metadata.model.annotation import Annotation
from cts_core.validation.validation_status import ValidationStatus
from cts_core.validation.get.metadata_get_validator import MetadataGetValidator
from cts_core.discovery.api_resource import ApiResource, ApiResourceProxy
from cts_framework.commons.enums import RequestStatus

ID_SUBSTITUTE_CHAR = "*"


class constraint():
    def __init__(self, field, value):
        self._field = field
        self._value = value


class equal(constraint):
    def apply(self, dataframe):
        return dataframe[dataframe[self._field] == self._value]


class not_equal(constraint):
    def apply(self, dataframe):
        return dataframe[dataframe[self._field] != self._value]


class less_or_equal(constraint):
    def apply(self, dataframe):
        return dataframe[dataframe[self._field] <= self._value]


class less(constraint):
    def apply(self, dataframe):
        return dataframe[dataframe[self._field] < self._value]


class greater_or_equal(constraint):
    def apply(self, dataframe):
        return dataframe[dataframe[self._field] >= self._value]


class greater(constraint):
    def apply(self, dataframe):
        return dataframe[dataframe[self._field] > self._value]


class first(constraint):
    def __init__(self, number):
        self._number = number

    def apply(self, dataframe):
        return dataframe.iloc[0:self._number]


class last(constraint):
    def __init__(self, number):
        self._number = number

    def apply(self, dataframe):
        return dataframe.iloc[-self._number:]


class order_by(constraint):
    def __init__(self, field, ascending=None):
        if ascending is None:
            ascending = True
        self._field = field
        self._ascending = ascending

    def apply(self, dataframe):
        return dataframe.sort_values(by=[self._field], ascending=self._ascending)


class from_collection(constraint):
    def __init__(self, collection):
        if not collection.endswith("/"):
            collection = collection + "/"
        self._collection = collection

    def apply(self, dataframe):
        return dataframe[dataframe.url.str.startswith(self._collection) &
                         dataframe.url.str[len(self._collection):]]


class odata_ids(constraint):
    def __init__(self, odata_id_set):
        self._odata_ids = odata_id_set

    def apply(self, dataframe):
        temp_df = DataFrame()
        for odata_id in self._odata_ids:
            temp_df = concat([dataframe[dataframe["@odata.id"] == odata_id], temp_df], axis=0, ignore_index=True)
        return temp_df


class urls(constraint):
    def __init__(self, url_set):
        self._urls = url_set

    def apply(self, dataframe):
        temp_df = DataFrame()
        for url in self._urls:
            temp_df = concat([dataframe[dataframe.url == url], temp_df], axis=0, ignore_index=True)
        return temp_df


class DiscoveryContainer(dict):
    def __init__(self, service_root=None, metadata_container=None):
        if service_root is not None:
            self._service_root = service_root
        else:
            self._service_root = ""
        self.metadata_container = metadata_container
        self._registered_url_patterns = dict()
        self._dataframes = dict()

    def __getitem__(self, item):
        obj = super(DiscoveryContainer, self).__getitem__(item)
        return ApiResourceProxy(obj, self)

    def _is_a_collection(self, url):
        return "Members" in self[url].body and "Members@odata.count" in self[url].body

    # e.g. "/redfish/v1/EthernetSwitches/1/Ports/7" -> "/redfish/v1/EthernetSwitches/*/Ports/*"
    def _generalize_url(self, url):
        parts = urlsplit(url)
        simplified_url = urlunsplit((parts.scheme,
                                     parts.netloc,
                                     '',
                                     '',
                                     ''))
        url = simplified_url
        segments = split_path_into_segments(parts.path)
        parent_is_collection = False

        for segment in segments:
            simplified_url = simplified_url + '/' + (ID_SUBSTITUTE_CHAR if parent_is_collection else segment)
            url = url + '/' + segment
            if url in self and self._is_a_collection(url):
                parent_is_collection = True
            else:
                parent_is_collection = False

        generalized_path = urlsplit(simplified_url).path

        return urlunsplit((parts.scheme,
                           parts.netloc,
                           generalized_path,
                           parts.query,
                           parts.fragment))

    def get_expected_odata_type_for_url(self, url):
        generalized_url = self._generalize_url(url)
        if generalized_url in self._registered_url_patterns:
            return self._registered_url_patterns[generalized_url]
        else:
            print "MESSAGE::Could not find expected @odata.type for %s (%s)" % (url, generalized_url)
            return None

    def register_url_pattern(self, url, expected_odata_type):
        generalized_url = self._generalize_url(url)
        OdataIdGeneralizedMapper.register_generalized_url(url, generalized_url)
        if generalized_url not in self._registered_url_patterns:
            self._registered_url_patterns[generalized_url] = expected_odata_type

    def is_visited(self, url):
        return url in self.keys()

    def add_resource(self, resource, check_resource_against_metadata=None):
        """
        :type resource: cts_core.discovery.api_resource.ApiResource
        :return: cts_framework.commons.enums.RequestStatus
        """

        if resource is None:
            return
        if resource.odata_id is None:
            return
        self[resource.url] = resource

        odata_type = resource.odata_type

        if odata_type:  # do not add the resource to any table if it is of an unknown type
            self.register_url_pattern(resource.url, odata_type)
            resource.expected_odata_type = odata_type
            new_row = json_normalize(resource.body)
            new_row["url"] = resource.url

            if odata_type in self._dataframes:
                # delete the deprecated definition of the resource if it exists
                if len(self._dataframes[odata_type][self._dataframes[odata_type]["url"] ==
                                                    resource.url].index):
                    self._dataframes[odata_type] = \
                        self._dataframes[odata_type][self._dataframes[odata_type]["url"] != resource.url]
                self._dataframes[odata_type] = \
                    concat([self._dataframes[odata_type], new_row], axis=0, ignore_index=True)
            else:
                self._dataframes[odata_type] = new_row

        if not check_resource_against_metadata:
            return RequestStatus.SUCCESS

        # this method is called in ApiCaller.get_resource() method, hence RequestStatus as a return type
        if self.metadata_container:
            status = MetadataGetValidator(self.metadata_container).validate_single_entity(resource)
            if status == ValidationStatus.PASSED:
                print "DEBUG::Metadata validation for resource {} passed".format(resource.url)
                return RequestStatus.SUCCESS
            else:
                cts_error("Resource {odata_id:id} did not pass metadata validation", odata_id=resource.url)
                return RequestStatus.FAILED
        else:
            cts_warning("Metadata not known, skipping resource validation")
            return RequestStatus.SUCCESS

    def delete_resource(self, url):
        """
        :type url: str
        :return:
        """
        odata_type = self[url].odata_type
        if odata_type:
            self._dataframes[odata_type] = \
                self._dataframes[odata_type][self._dataframes[odata_type].url != url]

        del self[url]

    def _get_proper_odata_type_versions(self, odata_type):
        if not self.metadata_container:
            return [odata_type]
        if odata_type not in self.metadata_container.entities:
            cts_error("Asked for resources of type {odata_type} but the type is not present in metadata",
                      odata_type=odata_type)
            return []

        return [key for key in self._dataframes.keys() if
                self.metadata_container.entities[odata_type].is_compatible(key)]

    def validate_redfish_uris_consistency(self, api_resource, metadata_container):
        odata_id = api_resource.odata_id
        odata_type = api_resource.odata_type
        entity = metadata_container.entities.get(odata_type)

        if entity is None:
            mapped_type = metadata_container._map_types.get(odata_type)
            entity = metadata_container.entities.get(mapped_type)
            if entity is None:
                cts_error("No entity in metadata with @odata.type={odata_type}".format(odata_type=odata_type))
                return ValidationStatus.FAILED

        if Annotation.REDFISH_URIS not in entity._annotations:
            return ValidationStatus.PASSED

        redfish_uris_list = entity._annotations[Annotation.REDFISH_URIS].redfish_uris
        matched_uris = [redfish_uri for redfish_uri in redfish_uris_list
                        if re.match(re.sub(r"{.+}", ".+", redfish_uri), odata_id)]

        if not matched_uris:
            cts_error("Resource {res} does not match expected RedfishUri in metadata. Expected {expected}"
                                .format(res=odata_id, expected=redfish_uris_list))
            return ValidationStatus.FAILED
        else:
            cts_message("Resource {res} is compliant with expected RedfishUri in metadata"
                            .format(res=odata_id))
            return ValidationStatus.PASSED

    def count_resources(self, odata_type, any_child_version=None):
        """
        :type odata_type: str
        :type any_child_version: bool
        :return: int
        """
        if any_child_version:
            return sum(self.count_resources(type) for type in self._get_proper_odata_type_versions(odata_type))

        if odata_type not in self._dataframes:
            return 0
        return len(self._dataframes[odata_type].index)

    def get_resources(self, odata_type, any_child_version=None, constraints=None):
        """
        :type odata_type: str
        :type any_child_version: bool
        :type constraints: list[constraint]
        :return: list[str, ApiResource]
        """
        if constraints is None:
            constraints = []

        if any_child_version:
            temp_df = DataFrame()
            for odata_type in self._get_proper_odata_type_versions(odata_type):
                if odata_type in self._dataframes.keys():
                    temp_df = concat([self._dataframes[odata_type], temp_df], axis=0, ignore_index=True)
        else:
            if odata_type not in self._dataframes:
                return []
            temp_df = self._dataframes[odata_type]

        for requirement in constraints:
            if not len(temp_df.index):
                break
            temp_df = requirement.apply(temp_df)

        return [(url, ApiResourceProxy(self[url], self)) for url in [row.url for _,
                                                                                 row in temp_df.iterrows()]]

    def iter_all(self, odata_type):
        """
        :type odata_type: str
        returns generator of resources of given @odata.type
        """
        t = self.get_resources(odata_type, any_child_version=True)
        resources = (ApiResourceProxy(r, self) for _, r in t)
        return resources

    def physical_location_aware_resources(self):
        """
        :rtype: list[ApiResource]
        """
        resources = [ApiResourceProxy(r, self) for _, r in self.iteritems() if r.location]
        return resources

    def root_resources(self):
        """
        :rtype: list[ApiResource]
        """
        resources = [ApiResourceProxy(r, self) for r in self.physical_location_aware_resources() if
                     not r.location.parent_id]
        return resources

    def get_netloc(self, object):
        if isinstance(object, ApiResource):
            return object.netloc

        # is object url ?
        if isinstance(object, basestring):
            return self.__getitem__(object).netloc

        return None

    def odataid_lookup(self, *odata_id):
        ids = [re.sub(r'#.*$', '', str(id)) for id in odata_id]
        resources = [ApiResourceProxy(r, self) for r in self.itervalues() if r.odata_id in ids]

        if len(resources) < len(odata_id):
            found_ids = set([r.odata_id for r in resources])
            requested_ids = set(ids)
            diff = requested_ids.difference(found_ids)
            if len(diff) > 0:
                cts_error('References to unknown resources: {refs}', refs=', '.join(diff))

        return resources

    def odata_type_lookup(self, odata_type):
        """
        Find all resources with similar odata_type
        :param odata_type:
        :return:
        """

        def get_elements_with_proper_odata_type(elements):
            return [element for element in elements if element.odata_type is not None]

        resources = [ApiResourceProxy(r, self) for r in get_elements_with_proper_odata_type(self.itervalues())
                     if odata_type in r.odata_type]
        return set([r.odata_type for r in resources])

    def dump_content(self, filename):
        with open(filename, 'wb') as f:
            pickle.dump(dict(self), f, 0)
