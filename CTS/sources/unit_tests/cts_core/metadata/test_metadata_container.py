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
import shlex
import subprocess
import unittest

from bs4 import BeautifulSoup
from mock import Mock

from cts_core.metadata.comparator import Comparator
from cts_core.metadata.metadata_manager import MetadataManager
from cts_core.metadata.model.annotation import Annotation


def execute_bash_command(command):
    args = shlex.split(command)
    process = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    stdoutdata, stderrdata = process.communicate()

    return process.returncode, stdoutdata, stderrdata

class MetadataContainerUnitTest(unittest.TestCase):
    def setUp(self):
        self.metadata_container = self.load()
        self.other = self.load()

    def load(self):
        MetadataManager.SERVICE_TO_DIR = {
            "dummy": "example_metadata"
        }
        metadata_manager = MetadataManager([])
        metadata_manager._metadata_home = Mock(return_value=os.path.dirname(os.path.realpath(__file__)))
        metadata_manager.read_metadata_for_services("dummy")
        return metadata_manager.metadata_container

    def diff(self, cmp):
        file_a, file_b = cmp.flush_to_disk()
        ret, out, err = execute_bash_command("diff {file_a} {file_b}".format(
            file_a=file_a.name,
            file_b=file_b.name))
        return out

    def test_compare_identical(self):
        cmp = self.metadata_container.compare(self.other)
        self.assertEqual(cmp.result, Comparator.EQUAL)
        self.assertEqual(self.diff(cmp), "")

    def compress(self, s):
        return s.replace(" ", "").replace("\t", "").replace("\n", "")

    def test_compare_missing_entity(self):
        del self.metadata_container.entities["ServiceRoot.v1_1_0.ServiceRoot"]
        del self.other.entities['Resource.v1_0_0.Resource']
        cmp = self.metadata_container.compare(self.other)
        diff = self.compress(cmp.get_side_by_side())
        expected = self.compress("""                                                            |                                                              
            Entity: Resource.v1_0_0.Resource                            |  ?                                                           
            ------------------------------------------------------------|  ------------------------------------------------------------
            Entity: ServiceRoot.ServiceRoot                             |  Entity: ServiceRoot.ServiceRoot                             
              Property: Description                                     |    ?                                                         
              Property: Id                                              |    ?                                                         
              Property: Name                                            |    ?                                                         
              Property: Oem                                             |    ?                                                         
              Annotation: OData.AdditionalProperties                    |    ?                                                         
            ------------------------------------------------------------|  ------------------------------------------------------------
            Entity: ServiceRoot.v1_0_0.ServiceRoot                      |  Entity: ServiceRoot.v1_0_0.ServiceRoot                      
              Property: Description                                     |    ?                                                         
              Property: Id                                              |    ?                                                         
              Property: Name                                            |    ?                                                         
              Property: Oem                                             |    ?                                                         
              Annotation: OData.AdditionalProperties                    |    ?                                                         
            ------------------------------------------------------------|  ------------------------------------------------------------
            Entity: ServiceRoot.v1_0_2.ServiceRoot                      |  Entity: ServiceRoot.v1_0_2.ServiceRoot                      
              Property: Description                                     |    ?                                                         
              Property: Id                                              |    ?                                                         
              Property: Name                                            |    ?                                                         
              Property: Oem                                             |    ?                                                         
              Annotation: OData.AdditionalProperties                    |    ?                                                         
            ------------------------------------------------------------|  ------------------------------------------------------------
            Entity: ServiceRoot.v1_0_3.ServiceRoot                      |  Entity: ServiceRoot.v1_0_3.ServiceRoot                      
              Property: Description                                     |    ?                                                         
              Property: Id                                              |    ?                                                         
              Property: Name                                            |    ?                                                         
              Property: Oem                                             |    ?                                                         
              Annotation: OData.AdditionalProperties                    |    ?                                                         
            ------------------------------------------------------------|  ------------------------------------------------------------
            ?                                                           |  Entity: ServiceRoot.v1_1_0.ServiceRoot                      
            ------------------------------------------------------------|  ------------------------------------------------------------
            Entity: ServiceRoot.v1_1_1.ServiceRoot                      |  Entity: ServiceRoot.v1_1_1.ServiceRoot                      
              ?                                                         |    Property: RedfishVersion                                  
              ?                                                         |    Property: UUID                                            
              ?                                                         |    Annotation: OData.Description                             
              ?                                                         |    Annotation: OData.LongDescription                         
            ------------------------------------------------------------|  ------------------------------------------------------------
                                                                        |   """)

        self.assertEqual(diff, expected)
        self.assertEqual(Comparator.NON_EQUAL, cmp.result)

    def test_compare_missing_type(self):
        del self.metadata_container.types['Org.OData.Core.V1.Tag']
        del self.other.types['Resource.v1_1_0.Identifier']
        cmp = self.metadata_container.compare(self.other)
        diff = self.compress(cmp.get_side_by_side())
        expected = self.compress("""      
                                                                        |                                                              
            ?                                                           |  Type: Org.OData.Core.V1.Tag                                 
            ------------------------------------------------------------|  ------------------------------------------------------------
            Type: Resource.v1_1_0.Identifier                            |  ?                                                           
            ------------------------------------------------------------|  ------------------------------------------------------------
                                                                        |  """)

        self.assertEqual(diff, expected)
        self.assertEqual(Comparator.NON_EQUAL, cmp.result)

    def test_compare_entity_base_type(self):
        self.metadata_container.entities["ServiceRoot.ServiceRoot"].base_type = 'Resource.v1_1_0.Resource'
        cmp = self.metadata_container.compare(self.other)
        diff = self.diff(cmp).replace('\n', '<CR>')

        self.assertRegexpMatches(diff, r'Base type: Resource.v1_1_0.Resource.*Base type: Resource.v1_0_0.Resource')
        self.assertEqual(Comparator.NON_EQUAL, cmp.result)

    def test_compare_type_base_type(self):
        self.metadata_container.types["Message.v1_0_0.Message"].base_type = 'Another.Base'
        cmp = self.metadata_container.compare(self.other)
        diff = self.diff(cmp).replace('\n', '<CR>')

        self.assertRegexpMatches(diff, r'Base type: Another.Base.*Base type: Message.Message')
        self.assertEqual(Comparator.NON_EQUAL, cmp.result)

    def test_compare_entity_properties_of_different_types(self):
        self.metadata_container.entities["ServiceRoot.v1_0_0.ServiceRoot"].properties['RedfishVersion'].type = \
            "Edm.Int64"

        cmp = self.metadata_container.compare(self.other)
        diff = self.diff(cmp).replace('\n', '<CR>')

        self.assertRegexpMatches(diff, r'Type: Edm.String.*Type: Edm.Int64')
        self.assertEqual(Comparator.NON_EQUAL, cmp.result)

    def test_compare_type_properties_of_different_types(self):
        self.metadata_container.types["Message.v1_0_0.Message"].properties['MessageArgs'].type = \
            "Resource.Oem"
        self.metadata_container.types["Message.v1_0_0.Message"].properties['MessageArgs'].is_collection = False

        cmp = self.metadata_container.compare(self.other)
        diff = self.compress(cmp.get_side_by_side())
        expected = self.compress("""                                                                                      |                                                              
            Type: Message.v1_0_0.Message                                |  Type: Message.v1_0_0.Message                                
              Property: MessageArgs                                     |    Property: MessageArgs                                     
                Type: Resource.Oem                                      |      Type: Collection(Edm.String)                            
                Annotation: OData.AdditionalProperties                  |      ?                                                       
                Annotation: Redfish.DynamicPropertyPatterns             |      ?                                                       
            ------------------------------------------------------------|  ------------------------------------------------------------
                                                                        |      """)

        self.assertEqual(diff, expected)
        self.assertEqual(Comparator.NON_EQUAL, cmp.result)

    def test_compare_entity_property_nullable(self):
        self.metadata_container.entities["ServiceRoot.v1_0_0.ServiceRoot"].properties['RedfishVersion'].nullable = \
            True

        cmp = self.metadata_container.compare(self.other)
        diff = self.diff(cmp).replace('\n', '<CR>')

        self.assertRegexpMatches(diff, r'Nullable.*Not Nullable')
        self.assertEqual(Comparator.NON_EQUAL, cmp.result)

    def test_compare_type_properties_nullable(self):
        self.metadata_container.types["Message.v1_0_0.Message"].properties['MessageArgs'].nullable = \
            True

        cmp = self.metadata_container.compare(self.other)
        diff = self.diff(cmp).replace('\n', '<CR>')

        self.assertRegexpMatches(diff, r'Nullable.*Not Nullable')
        self.assertEqual(Comparator.NON_EQUAL, cmp.result)

    def test_different_entity_annotations(self):
        pass

    def test_different_type_annotations(self):
        pass

    def build_annotation(self, term, value_type, value):
        annotation_doc = """
                      <Annotation Term="{term}" {value_type}="{value}" />
                     """.format(term=term, value=value, value_type=value_type)
        annotation_soup = BeautifulSoup(annotation_doc, "lxml").find_all("annotation")[0]
        return Annotation(annotation_soup)

    def test_different_property_is_required(self):
        self.metadata_container.entities["ServiceRoot.v1_0_0.ServiceRoot"]. \
            properties['RedfishVersion']. \
            _annotations['Redfish.Required'] = "Anything"

        cmp = self.metadata_container.compare(self.other)
        diff = self.diff(cmp).replace('\n', '<CR>')

        self.assertRegexpMatches(diff, r'Required.*Optional')
        self.assertEqual(Comparator.NON_EQUAL, cmp.result)

    def test_different_property_permission(self):
        self.metadata_container.entities["ServiceRoot.v1_0_0.ServiceRoot"]. \
            properties['RedfishVersion']. \
            _annotations['OData.Permissions'] = \
            self.build_annotation('OData.Permissions', "EnumMember", "OData.Permission/ReadWrite")

        cmp = self.metadata_container.compare(self.other)
        diff = self.compress(cmp.get_side_by_side())
        expected = self.compress("""                                                                    |                                                              
            Entity: ServiceRoot.v1_0_0.ServiceRoot                      |  Entity: ServiceRoot.v1_0_0.ServiceRoot                      
              Property: RedfishVersion                                  |    Property: RedfishVersion                                  
                [ReadWrite]                                             |      [ReadOnly]                                              
                Annotation: OData.Permissions                           |      Annotation: OData.Permissions                           
                  enummember=OData.Permission/ReadWrite                 |        enummember=OData.Permission/Read                      
            ------------------------------------------------------------|  ------------------------------------------------------------
            Entity: ServiceRoot.v1_0_2.ServiceRoot                      |  Entity: ServiceRoot.v1_0_2.ServiceRoot                      
              Property: RedfishVersion                                  |    Property: RedfishVersion                                  
                [ReadWrite]                                             |      [ReadOnly]                                              
                Annotation: OData.Permissions                           |      Annotation: OData.Permissions                           
                  enummember=OData.Permission/ReadWrite                 |        enummember=OData.Permission/Read                      
            ------------------------------------------------------------|  ------------------------------------------------------------
            Entity: ServiceRoot.v1_0_3.ServiceRoot                      |  Entity: ServiceRoot.v1_0_3.ServiceRoot                      
              Property: RedfishVersion                                  |    Property: RedfishVersion                                  
                [ReadWrite]                                             |      [ReadOnly]                                              
                Annotation: OData.Permissions                           |      Annotation: OData.Permissions                           
                  enummember=OData.Permission/ReadWrite                 |        enummember=OData.Permission/Read                      
            ------------------------------------------------------------|  ------------------------------------------------------------
            Entity: ServiceRoot.v1_1_0.ServiceRoot                      |  Entity: ServiceRoot.v1_1_0.ServiceRoot                      
              Property: RedfishVersion                                  |    Property: RedfishVersion                                  
                [ReadWrite]                                             |      [ReadOnly]                                              
                Annotation: OData.Permissions                           |      Annotation: OData.Permissions                           
                  enummember=OData.Permission/ReadWrite                 |        enummember=OData.Permission/Read                      
            ------------------------------------------------------------|  ------------------------------------------------------------
            Entity: ServiceRoot.v1_1_1.ServiceRoot                      |  Entity: ServiceRoot.v1_1_1.ServiceRoot                      
              Property: RedfishVersion                                  |    Property: RedfishVersion                                  
                [ReadWrite]                                             |      [ReadOnly]                                              
                Annotation: OData.Permissions                           |      Annotation: OData.Permissions                           
                  enummember=OData.Permission/ReadWrite                 |        enummember=OData.Permission/Read                      
            ------------------------------------------------------------|  ------------------------------------------------------------
                                                                        |  """)

        self.assertEqual(diff, expected)
        self.assertEqual(Comparator.NON_EQUAL, cmp.result)

