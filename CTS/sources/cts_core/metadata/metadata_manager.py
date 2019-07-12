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
import os
from os import getenv

from lxml import etree

from cts_core.commons.api_caller import ApiCaller
from cts_core.commons.error import cts_warning, cts_error, cts_message
from cts_core.commons.services import ServiceTypes
from cts_core.metadata.metadata_container import MetadataContainer, EnumType
from cts_core.metadata.model.entity import Entity
from cts_core.metadata.model.metadata_types import ComplexType
from cts_core.metadata.model.metadata_types import TypeDefinition
from cts_framework.commons.commons import Commons
from cts_framework.commons.digest import DirDigest
from cts_framework.helpers.vars.env_consts import Constants

TYPE_DEFINITION = "typedefinition"
COMPLEX_TYPE = "complextype"
ENUM_TYPE = "enumtype"
SCHEMA = "schema"
NAMESPACE = "namespace"
ENTITY_TYPE = "entitytype"
TERM = "term"
ROOT_XML_FILE = "$metadata.xml"
REFERENCE = "edmx:reference"
URI = "uri"


class MetadataMalformed(BaseException):
    pass


class MetadataManager:
    METADATA_URL = "/redfish/v1/$metadata"

    SERVICE_TO_DIR = {
        ServiceTypes.PODM_1_2: "1.2/PODM",
        ServiceTypes.PSME_1_2: "1.2/PSME",
        ServiceTypes.RMM_1_2: "1.2/RMM",
        ServiceTypes.SS_1_2: "1.2/SS",

        ServiceTypes.PODM_2_1: "2.1.3/PODM",
        ServiceTypes.PSME_2_1: "2.1.3/PSME",
        ServiceTypes.RMM_2_1: "2.1.3/RMM",
        ServiceTypes.SS_2_1: "2.1.3/SS",

        ServiceTypes.PODM_2_1_2: "2.1.2/PODM",
        ServiceTypes.PSME_2_1_2: "2.1.2/PSME",
        ServiceTypes.RMM_2_1_2: "2.1.2/RMM",
        ServiceTypes.SS_2_1_2: "2.1.2/SS",

        ServiceTypes.PODM_2_1_3: "2.1.3/PODM",
        ServiceTypes.PSME_2_1_3: "2.1.3/PSME",
        ServiceTypes.RMM_2_1_3: "2.1.3/RMM",
        ServiceTypes.SS_2_1_3: "2.1.3/SS",

        ServiceTypes.PODM_2_1_4: "2.1.4/PODM",
        ServiceTypes.PSME_2_1_4: "2.1.4/PSME",
        ServiceTypes.RMM_2_1_4: "2.1.4/RMM",
        ServiceTypes.SS_2_1_4: "2.1.4/SS",

        ServiceTypes.PODM_2_2: "2.2/PODM",
        ServiceTypes.PSME_2_2: "2.2/PSME",
        ServiceTypes.RMM_2_2: "2.2/RMM",
        ServiceTypes.SS_2_2: "2.2/SS",

        ServiceTypes.PODM_2_3: "2.3/PODM",
        ServiceTypes.PSME_2_3: "2.3/PSME",
        ServiceTypes.RMM_2_3: "2.3/RMM",
        ServiceTypes.SS_2_3: "2.3/SS",

        ServiceTypes.PODM_2_4: "2.4/PODM",
        ServiceTypes.PSME_2_4: "2.4/PSME",
        ServiceTypes.RMM_2_4: "2.4/RMM",
        ServiceTypes.SS_2_4: "2.4/SS",

        ServiceTypes.SCENARIO_2_4: "2.4/SCENARIO",
        ServiceTypes.RACKSCALE_2_4: "2.4/RACKSCALE",

        ServiceTypes.PODM_2_5: "2.5/PODM",
        ServiceTypes.PSME_2_5: "2.5/PSME",
        ServiceTypes.RMM_2_5: "2.5/RMM",
        ServiceTypes.SS_2_5: "2.5/SS",

        ServiceTypes.SCENARIO_2_5: "2.5/SCENARIO",
        ServiceTypes.RACKSCALE_2_5: "2.5/RACKSCALE",

        ServiceTypes.REDFISH_2018_1: "redfish/2018.1",
        ServiceTypes.REDFISH_2018_2: "redfish/2018.2",
        ServiceTypes.REDFISH_2018_3: "redfish/2018.3",
        ServiceTypes.REDFISH_2019_1: "redfish/2019.1"

    }

    def __init__(self, qualifiers, ignore_types=None, map_types=None):
        self.qualifiers = qualifiers
        self.ignore_types = ignore_types if ignore_types is not None else set()
        self.map_types = map_types if map_types is not None else dict()
        self._metadata_container = MetadataContainer(ignore_types=self.ignore_types,
                                                     map_types=self.map_types)
        self.loaded_xmls = set()
        self.api_caller = None

    def read_metadata_for_services(self, *services):
        """
        :type services: list[string]
        """

        digest = DirDigest(self._metadata_home(), '.xml', DirDigest.LABEL_METADATA)
        if not digest.is_valid() and not getenv('CTS_SKIP', None):
            cts_error("Metadata located in {dir} is corrupted or has been tampered. Expected: {expected}, "
                      "Is: {current}",
                      dir=self._metadata_home(), expected=digest.official_digest, current=digest.digest)
            cts_message("{count} xml files have been found in {dir}".format(count=len(digest),
                                                                            dir=self._metadata_home()))
            digest.report_differences()

        for service in services:
            try:
                self.metadata_dir = os.path.join(self._metadata_home(),
                                                 MetadataManager.SERVICE_TO_DIR[service])
                self.read_metadata_from_dir(self.metadata_dir)
            except KeyError:
                cts_error("Internal error. Unknown metadata for service {service}", service=service)
                return False
        return True

    def read_metadata_from_dir(self, dir, root_file=None):
        if root_file is None:
            root_file = ROOT_XML_FILE

        if os.path.isdir(dir):
            self.metadata_dir = dir
            self._process_schema_file(root_file)
            return True
        else:
            cts_error("Internal error. Metadata directory {metadata_dir} not found", metadata_dir=dir)
            return False

    def _validate_xml_file(self, file_uri, metadata_text):
        try:
            doc = etree.fromstring(metadata_text.lstrip())
        except etree.XMLSyntaxError as err:
            cts_error('XML Syntax Error in file {file}', file=file_uri)
            raise MetadataMalformed()

    def read_metadata_from_strings(self, file_uri, *metadata):
        """
        :type file_uri: string
        :type metadata: list[string]
        :rtype: cts_core.metadata.metadata_container.MetadataContainer
        """
        for metadata_text in metadata:
            self._validate_xml_file(file_uri, metadata_text)

            metadata_soup = Commons.text_to_soup(metadata_text)

            for reference in metadata_soup.find_all(REFERENCE):
                self._process_reference(reference)

            for schema in metadata_soup.find_all(SCHEMA):
                self._process_namespace(schema)

        return self.metadata_container

    def download_metadata(self, configuration):
        self.api_caller = ApiCaller(configuration)
        self._process_schema_file(MetadataManager.METADATA_URL)
        return True

    @staticmethod
    def _metadata_home():
        return Constants.METADATA_HOME_DIR

    def _process_reference(self, reference_soup):
        try:
            self._process_schema_file(reference_soup[URI])
        except KeyError as uri:
            cts_error("Incorrect reference. URI not found")

    @property
    def metadata_container(self):
        return self._metadata_container

    def _process_schema_file(self, file_uri):
        if file_uri in self.loaded_xmls:
            return
        self.loaded_xmls.add(file_uri)

        if self.api_caller is None:
            self._process_local_schema_file(file_uri)
        else:
            self._process_remote_schema_file(file_uri)

    def _process_local_schema_file(self, file_uri):
        try:
            full_uri = os.path.join(self.metadata_dir, file_uri)
            with open(full_uri) as f:
                self.read_metadata_from_strings(file_uri, f.read())
        except IOError:
            cts_error("Metadata file {file} not found", file=full_uri)

    def _process_remote_schema_file(self, file_uri):
        _, status, _, response_body, _ = self.api_caller.get_xml(file_uri)
        if not status:
            cts_error("Error while accessing metadata {uri}", uri=file_uri)
        else:
            self.read_metadata_from_strings(file_uri, response_body)

    def _process_namespace(self, schema):
        """
        :type schema: bs4.element.Tag
        """
        try:
            namespace_name = schema[NAMESPACE]
        except KeyError:
            cts_warning(
                "Incorrect schema definition {schema_definition},\n missing of namespace name",
                schema_definition=str(schema))

        for entity_soup in schema.find_all(ENTITY_TYPE):
            entity = Entity(self._metadata_container, namespace_name, entity_soup, self.qualifiers)
            self._metadata_container.entities[entity.name] = entity

        for type_soup in schema.find_all(ENUM_TYPE):
            enum_type = EnumType(self._metadata_container, namespace_name, type_soup,
                                 self.qualifiers)
            self._metadata_container.types[enum_type.name] = enum_type

        for type_soup in schema.find_all(COMPLEX_TYPE):
            complex_type = ComplexType(self._metadata_container, namespace_name, type_soup,
                                       self.qualifiers)
            self._metadata_container.types[complex_type.name] = complex_type

        for type_soup in schema.find_all(TYPE_DEFINITION):
            type_definition = TypeDefinition(self._metadata_container, namespace_name, type_soup,
                                             self.qualifiers)
            self._metadata_container.types[type_definition.name] = type_definition
