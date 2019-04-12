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

import unittest
import os
from mock import MagicMock, patch, mock_open

import cts_framework
from cts_core.commons.api_caller import ApiCaller
from cts_core.metadata.metadata_manager import MetadataManager
from cts_core.validation.validation_status import ValidationStatus
from cts_framework.commons.enums import RequestStatus
from cts_framework.configuration.configuration import Configuration
from unit_tests.helpers.stdout_capture import StdoutCapture

BASE_NAMESPACE_NAME = "base_namespace_name"
BASE_ENTITY_NAME = "base_entity_name"
BASE_COMPLEX_TYPE_NAME = "base_complex_type_name"

PROPERTY_NAME = "property_name"
PROPERTY_TYPE = "property_type"
STRING_PROPERTY = "string_property"
INT16_PROPERTY = "int16_property"
BOOLEAN_PROPERTY = "boolean_property"

DERIVED_NAMESPACE_NAME = "derived_namespace_name"
DERIVED_ENTITY_NAME = "derived_entity_name"
DERIVED_COMPLEX_TYPE_NAME = "derived_complex_type_name"

API_ENDPOINT = "1.2.3.4:567"

METADATA_ROOT = """
<?xml version="1.0" encoding="UTF-8"?>
<edmx:Edmx xmlns:edmx="http://docs.oasis-open.org/odata/ns/edmx" Version="4.0">
  <edmx:Reference Uri="Org.OData.Core.V1.xml">
    <edmx:Include Namespace="Org.OData.Core.V1" Alias="OData"/>
  </edmx:Reference>
  <edmx:DataServices>
    <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="Service">
      <EntityContainer Name="Service" Extends="ServiceRoot.v1_1_1.ServiceContainer">
        <Annotation Term="OData.Description" String="Default Entity Container"/>
      </EntityContainer>
    </Schema>
  </edmx:DataServices>
</edmx:Edmx>
"""

METADATA = """
<Schemas>
  <Schema Namespace="{BASE_NAMESPACE_NAME}">
      <EntityType Name="{BASE_ENTITY_NAME}">
          <Property Name="{PROPERTY_NAME}" Type="{BASE_COMPLEX_TYPE_NAME}"/>
      </EntityType>

      <ComplexType Name="{BASE_COMPLEX_TYPE_NAME}">
          <Property Name="{PROPERTY_NAME}" Type="{BASE_COMPLEX_TYPE_NAME}" />
      </ComplexType>
  </Schema>

  <Schema Namespace="{DERIVED_NAMESPACE_NAME}">
      <EntityType Name="{DERIVED_ENTITY_NAME}" BaseType="{BASE_NAMESPACE_NAME}.{BASE_ENTITY_NAME}">
          <Property Name="{PROPERTY_NAME}" Type="{DERIVED_COMPLEX_TYPE_NAME}" />
          <Property Name="{STRING_PROPERTY}" Type="Edm.String" />
          <Property Name="{INT16_PROPERTY}" Type="Edm.Int16" />
          <Property Name="{BOOLEAN_PROPERTY}" Type="Edm.Boolean" />
      </EntityType>

      <ComplexType Name="{DERIVED_COMPLEX_TYPE_NAME}" BaseType="{BASE_NAMESPACE_NAME}.{BASE_ENTITY_NAME}">
          <Property Name="{PROPERTY_NAME}" Type="{DERIVED_COMPLEX_TYPE_NAME}" />
      </ComplexType>
  </Schema>
</Schemas>
""".format(BASE_NAMESPACE_NAME=BASE_NAMESPACE_NAME, BASE_ENTITY_NAME=BASE_ENTITY_NAME, PROPERTY_NAME=PROPERTY_NAME,
           BASE_COMPLEX_TYPE_NAME=BASE_COMPLEX_TYPE_NAME, DERIVED_NAMESPACE_NAME=DERIVED_NAMESPACE_NAME,
           DERIVED_ENTITY_NAME=DERIVED_ENTITY_NAME, DERIVED_COMPLEX_TYPE_NAME=DERIVED_COMPLEX_TYPE_NAME,
           PROPERTY_TYPE=PROPERTY_TYPE, STRING_PROPERTY=STRING_PROPERTY, INT16_PROPERTY=INT16_PROPERTY,
           BOOLEAN_PROPERTY=BOOLEAN_PROPERTY)


class MetadataManagerUnitTest(unittest.TestCase):
    def setUp(self):
        self.metadata_manager = MetadataManager(["qualifier"])

    def test_entities_read(self):
        metadata_container = self.metadata_manager.read_metadata_from_strings("Unknown", METADATA)
        self.assertIn(".".join([BASE_NAMESPACE_NAME, BASE_ENTITY_NAME]), metadata_container.entities)
        self.assertIn(".".join([DERIVED_NAMESPACE_NAME, DERIVED_ENTITY_NAME]), metadata_container.entities)

    def test_complex_type_read(self):
        metadata_container = self.metadata_manager.read_metadata_from_strings("Unknown", METADATA)
        self.assertIn(".".join([BASE_NAMESPACE_NAME, BASE_COMPLEX_TYPE_NAME]), metadata_container.types)
        self.assertIn(".".join([DERIVED_NAMESPACE_NAME, DERIVED_COMPLEX_TYPE_NAME]), metadata_container.types)
        self.assertIn(PROPERTY_NAME,
                      metadata_container.entities[".".join([DERIVED_NAMESPACE_NAME, DERIVED_ENTITY_NAME])].properties)
        self.assertIn(PROPERTY_NAME,
                      metadata_container.entities[".".join([BASE_NAMESPACE_NAME, BASE_ENTITY_NAME])].properties)

        self.assertEqual(".".join([BASE_NAMESPACE_NAME, BASE_COMPLEX_TYPE_NAME]),
                         metadata_container.types[".".join([BASE_NAMESPACE_NAME,
                                                                 BASE_COMPLEX_TYPE_NAME])].properties[PROPERTY_NAME].type)

        self.assertEqual(".".join([DERIVED_NAMESPACE_NAME, DERIVED_COMPLEX_TYPE_NAME]),
                         metadata_container.types[".".join([DERIVED_NAMESPACE_NAME,
                                                                 DERIVED_COMPLEX_TYPE_NAME])].properties[PROPERTY_NAME].type)

    def test_base_types(self):
        metadata_container = self.metadata_manager.read_metadata_from_strings("Unknown", METADATA)
        self.assertEqual(metadata_container.entities[".".join([BASE_NAMESPACE_NAME, BASE_ENTITY_NAME])].validate({
            "@odata.type": ".".join([DERIVED_NAMESPACE_NAME, DERIVED_ENTITY_NAME]),
            STRING_PROPERTY: "text",
            INT16_PROPERTY: 12,
            BOOLEAN_PROPERTY: True
        }, ""), ValidationStatus.PASSED)


    def test_follow_references_when_loading_local_matadata(self):
        MetadataManager.SERVICE_TO_DIR = {
            "SERVICE_A": "a"
        }
        os.path.isdir = MagicMock(return_value=True)

        manager = MetadataManager([])
        with StdoutCapture() as output:
          with patch("__builtin__.open", mock_open(read_data=METADATA_ROOT)) as mock_file:
              with patch("cts_framework.commons.digest.DirDigest.__init__") as init:
                  init.return_value = None
                  with patch("cts_framework.commons.digest.DirDigest.is_valid") as is_valid:
                      is_valid.return_value = True

                      manager.read_metadata_for_services("SERVICE_A")

        arguments = [arg[0][0] for arg in mock_file.call_args_list]
        self.assertGreater(len([a for a in arguments if "Org.OData.Core.V1.xml" in a]), 0)

    def test_follow_references_when_loading_remote_matadata(self):
        configuration = Configuration(
            **dict(UseSSL='True', ApiEndpoint=API_ENDPOINT))
        manager = MetadataManager([])
        with StdoutCapture() as output:
            with patch("cts_core.commons.api_caller.ApiCaller.get_xml") as get_xml:
                get_xml.side_effect = [(None, RequestStatus.SUCCESS, 200, METADATA_ROOT, None),
                                       (None, RequestStatus.SUCCESS, 200, METADATA_ROOT, None)]

                manager.download_metadata(configuration)

        arguments = [arg[0][0] for arg in get_xml.call_args_list]
        self.assertGreater(len([a for a in arguments if "Org.OData.Core.V1.xml" in a]), 0)
        self.assertEqual(-1, output.raw.find("ERROR::"))
