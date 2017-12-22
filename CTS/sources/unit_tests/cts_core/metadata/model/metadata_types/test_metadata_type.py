import unittest

from bs4 import BeautifulSoup

from cts_core.metadata.metadata_container import MetadataContainer
from cts_core.metadata.model.metadata_types.metadata_type import MetadataType

TAG = "tag"


class MetadataTypeReadUnitTest(unittest.TestCase):
    def _get_metadata_type_soup_from_metadata_type_text(self, metadata_type_text):
        return BeautifulSoup(metadata_type_text, "lxml").find_all(TAG)[0]

    def test_empty_metadata_type(self):
        namespace_name = "namespace_name"
        metadata_type_name = "metadata_type_name"

        metadata_type_doc = """
         <tag name="{metadata_type_name}"/>
        """.format(metadata_type_name=metadata_type_name)

        metadata_type_soup = self._get_metadata_type_soup_from_metadata_type_text(metadata_type_doc)
        metadata_type = MetadataType(MetadataContainer(), namespace_name, metadata_type_soup)

        self.assertEqual(metadata_type.name, ".".join([namespace_name, metadata_type_name]))
        self.assertFalse(metadata_type.annotations)
        self.assertIsNone(metadata_type.base_type)
        self.assertIsNone(metadata_type.type_category)

    def test_annotation_given(self):
        namespace_name = "namespace_name"
        metadata_type_name = "metadata_type_name"

        metadata_type_doc = """
         <tag name="{metadata_type_name}">
          <Annotation />
         </tag>
        """.format(metadata_type_name=metadata_type_name)

        metadata_type_soup = self._get_metadata_type_soup_from_metadata_type_text(metadata_type_doc)
        metadata_type = MetadataType(MetadataContainer(), namespace_name, metadata_type_soup)

        self.assertEqual(len(metadata_type.annotations), 1)

    def test_not_qualified_annotations_not_filtered_out(self):
        namespace_name = "namespace_name"
        metadata_type_name = "metadata_type_name"

        metadata_type_doc = """
                <tag name="{metadata_type_name}">
                 <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
                 <Annotation Term="OData.Description" String="This indicates drive firmware version."/>
                </tag>
               """.format(metadata_type_name=metadata_type_name)

        metadata_type_soup = self._get_metadata_type_soup_from_metadata_type_text(metadata_type_doc)
        metadata_type = MetadataType(MetadataContainer(), namespace_name, metadata_type_soup, ["A", "B"])

        self.assertEqual(len(metadata_type.annotations), 2)

    def test_annotations_with_other_qualifiers_are_filtered_out(self):
        namespace_name = "namespace_name"
        metadata_type_name = "metadata_type_name"

        metadata_type_doc = """
                <tag name="{metadata_type_name}">
                 <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read" Qualifier="C"/>
                 <Annotation Term="OData.Description" String="This indicates drive firmware version."/>
                </tag>
               """.format(metadata_type_name=metadata_type_name)

        metadata_type_soup = self._get_metadata_type_soup_from_metadata_type_text(metadata_type_doc)
        metadata_type = MetadataType(MetadataContainer(), namespace_name, metadata_type_soup, ["A", "B"])

        self.assertEqual(len(metadata_type.annotations), 1)

    def test_annotations_with_my_qualifiers_are_not_filtered_out(self):
        namespace_name = "namespace_name"
        metadata_type_name = "metadata_type_name"

        metadata_type_doc = """
                <tag name="{metadata_type_name}">
                 <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read" Qualifier="A"/>
                 <Annotation Term="OData.Description" String="This indicates drive firmware version."/>
                </tag>
               """.format(metadata_type_name=metadata_type_name)

        metadata_type_soup = self._get_metadata_type_soup_from_metadata_type_text(metadata_type_doc)
        metadata_type = MetadataType(MetadataContainer(), namespace_name, metadata_type_soup, ["A", "B"])

        self.assertEqual(len(metadata_type.annotations), 2)
