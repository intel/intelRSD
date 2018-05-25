import unittest

from bs4 import BeautifulSoup
from cts_core.metadata.metadata_manager import MetadataManager
from cts_core.metadata.metadata_container import MetadataContainer
from cts_core.metadata.model.entity import Entity

ENTITY_TYPE = "entitytype"


class EntityReadUnitTest(unittest.TestCase):
    def _get_entity_soup_from_entity_text(self, entity_text):
        return BeautifulSoup(entity_text, "lxml").find_all(ENTITY_TYPE)[0]

    def test_empty_entity(self):
        namespace_name = "namespace_name"
        entity_name = "entity_name"
        entity_doc = """
        <EntityType Name="{entity_name}" />
        """.format(entity_name=entity_name)

        entity_soup = self._get_entity_soup_from_entity_text(entity_doc)

        entity = Entity(MetadataContainer(), namespace_name, entity_soup)
        self.assertEqual(entity.name, ".".join([namespace_name, entity_name]))
        self.assertTrue(entity.allow_additional_properties)
        self.assertIsNone(entity.base_type)
        self.assertFalse(entity.abstract)
        self.assertFalse(entity.annotations)
        self.assertFalse(entity.properties)
        self.assertFalse(entity.navigation_properties)

    def test_entity_properties(self):
        entity_doc = """
        <EntityType Name="entity_name">
            <Property Name="property_name" type="property_type" />
        </EntityType>
        """

        entity_soup = self._get_entity_soup_from_entity_text(entity_doc)

        entity = Entity(MetadataContainer(), "", entity_soup)
        self.assertFalse(entity.navigation_properties)
        self.assertEqual(len(entity.properties), 1)

    def test_entity_sees_inherited_properties(self):
        metadata = """
                <Schemas>
                   <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="A">
                       <EntityType Name="Resource" Abstract="false">
                           <Property Name="GrandpaProperty" Type="Resource.Description">
                                <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
                           </Property>
                       </EntityType>
                   </Schema>

                   <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="B">
                     <EntityType Name="Resource" BaseType="A.Resource" Abstract="false">
                       <Property Name="DaddyProperty" Type="Resource.Id" Nullable="false">
                           <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
                       </Property>
                     </EntityType>
                   </Schema>

                   <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="C">
                     <EntityType Name="Resource" BaseType="B.Resource" Abstract="false">
                        <Property Name="KidProperty" Type="Resource.Id" Nullable="false">
                           <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
                        </Property>
                     </EntityType>
                   </Schema>
                </Schemas>
                    """

        metadata_manager = MetadataManager(["qualifier"])
        metadata_container = metadata_manager.read_metadata_from_strings("Unknown", metadata)

        resource_definition = metadata_container.entities['C.Resource']
        properties = resource_definition.properties
        self.assertIn('KidProperty', properties)
        self.assertIn('DaddyProperty', properties)
        self.assertIn('GrandpaProperty', properties)

    def test_entity_navigation_property(self):
        entity_doc = """
        <EntityType Name="entity_name">
            <NavigationProperty Name="property_name" type="property_type" />
        </EntityType>
        """

        entity_soup = self._get_entity_soup_from_entity_text(entity_doc)

        entity = Entity(MetadataContainer(), "", entity_soup)
        self.assertFalse(entity.properties)
        self.assertEqual(len(entity.navigation_properties), 1)

    def test_entity_allow_additional_properties(self):
        entity_doc = """
        <EntityType Name="entity_name">
            <Annotation Term="OData.AdditionalProperties" Bool="true"/>
        </EntityType>
        """

        entity_soup = self._get_entity_soup_from_entity_text(entity_doc)

        entity = Entity(MetadataContainer(), "", entity_soup)
        self.assertTrue(entity.allow_additional_properties)
