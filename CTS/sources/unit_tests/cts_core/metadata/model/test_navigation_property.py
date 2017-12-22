import unittest

from bs4 import BeautifulSoup

from cts_core.metadata.metadata_container import MetadataContainer
from cts_core.metadata.model.navigation_property import Property

PROPERTY = "navigationproperty"


class NavigationReadPropertyUnitTest(unittest.TestCase):
    def _get_navigation_property_soup_from_navigation_property_text(self, navigation_property_text):
        return BeautifulSoup(navigation_property_text, "lxml").find_all(PROPERTY)[0]

    def test_empty_navigation_property(self):
        namespace_name = "namespace_name"
        navigation_property_name = "navigation_property_name"
        navigation_property_type = "navigation_property_type"

        navigation_property_doc = """
        <NavigationProperty name="{navigation_property_name}" type="{navigation_property_type}" />
        """.format(navigation_property_name=navigation_property_name, navigation_property_type=navigation_property_type)

        navigation_property_soup = self._get_navigation_property_soup_from_navigation_property_text(navigation_property_doc)

        tested_navigation_property = Property(MetadataContainer(), namespace_name, navigation_property_soup)

        self.assertEqual(tested_navigation_property.name, navigation_property_name)
        self.assertEqual(tested_navigation_property.type, ".".join([namespace_name, navigation_property_type]))
        self.assertFalse(tested_navigation_property.is_collection)
        self.assertTrue(tested_navigation_property.nullable)
        self.assertFalse(tested_navigation_property.is_required)
        self.assertFalse(tested_navigation_property.annotations)

    def test_navigation_property_annotation(self):
        navigation_property_doc = """
        <NavigationProperty name="navigation_property_name" type="navigation_property_type">
            <Annotation />
        </NavigationProperty>
        """

        navigation_property_soup = self._get_navigation_property_soup_from_navigation_property_text(navigation_property_doc)

        tested_navigation_property = Property(MetadataContainer(), "", navigation_property_soup)

        self.assertEqual(len(tested_navigation_property.annotations), 1)

    def test_navigation_property_collection(self):
        navigation_property_doc = """
            <NavigationProperty name="navigation_property_name" type="Collection(navigation_property_type)">
                <Annotation />
            </NavigationProperty>
            """

        navigation_property_soup = self._get_navigation_property_soup_from_navigation_property_text(navigation_property_doc)

        tested_navigation_property = Property(MetadataContainer(), "", navigation_property_soup)

        self.assertTrue(tested_navigation_property.is_collection)

    def test_navigation_property_nullable(self):
        navigation_property_doc = """
            <NavigationProperty name="navigation_property_name" type="navigation_property_type" nullable="true"/>
            """

        navigation_property_soup = self._get_navigation_property_soup_from_navigation_property_text(navigation_property_doc)

        tested_navigation_property = Property(MetadataContainer(), "", navigation_property_soup)

        self.assertTrue(tested_navigation_property.nullable)

    def test_navigation_property_required(self):
        navigation_property_doc = """
            <NavigationProperty name="navigation_property_name" type="navigation_property_type">
                <Annotation Term="Redfish.Required" />
            </NavigationProperty>
            """

        navigation_property_soup = self._get_navigation_property_soup_from_navigation_property_text(navigation_property_doc)

        tested_navigation_property = Property(MetadataContainer(), "", navigation_property_soup)

        self.assertTrue(tested_navigation_property.is_required)
