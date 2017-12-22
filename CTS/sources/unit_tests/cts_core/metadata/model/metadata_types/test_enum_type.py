import unittest

from bs4 import BeautifulSoup

from cts_core.metadata.metadata_container import MetadataContainer
from cts_core.metadata.model.metadata_types import EnumType
from cts_core.metadata.model.metadata_types.enum_type import EnumMember
from cts_core.metadata.model.metadata_types.metadata_type_categories import MetadataTypeCategories
from cts_core.validation.validation_status import ValidationStatus

ENUM_TYPE = "enumtype"


class EnumTypeReadUnitTest(unittest.TestCase):
    def _get_enum_type_soup_from_enum_type_text(self, enum_type_text):
        return BeautifulSoup(enum_type_text, "lxml").find_all(ENUM_TYPE)[0]

    def test_empty_enum_type(self):
        namespace_name = "namespace_name"
        enum_type_name = "enum_type_name"

        enum_type_doc = """
         <EnumType name="{enum_type_name}"/>
        """.format(enum_type_name=enum_type_name)

        enum_type_soup = self._get_enum_type_soup_from_enum_type_text(enum_type_doc)
        enum_type = EnumType(MetadataContainer(), namespace_name, enum_type_soup)

        self.assertEqual(enum_type.name, ".".join([namespace_name, enum_type_name]))
        self.assertFalse(enum_type.annotations)
        self.assertIsNone(enum_type.base_type)
        self.assertEqual(enum_type.type_category, MetadataTypeCategories.ENUM_TYPE)
        self.assertFalse(enum_type.members)

    def test_members_given(self):
        namespace_name = "namespace_name"
        enum_type_name = "enum_type_name"

        enum_type_doc = """
         <EnumType name="{enum_type_name}">
          <Member name="member1"/>
          <Member name="member2"/>
         </EnumType>
        """.format(enum_type_name=enum_type_name)

        enum_type_soup = self._get_enum_type_soup_from_enum_type_text(enum_type_doc)
        enum_type = EnumType(MetadataContainer(), namespace_name, enum_type_soup)

        self.assertEqual(len(enum_type.members), 2)
        self.assertIn("member1", enum_type.members)
        self.assertIn("member2", enum_type.members)


ENUM_MEMBER = "member"


class EnumMemberReadUnitTest(unittest.TestCase):
    def _get_enum_member_soup_from_enum_member_text(self, enum_member_text):
        return BeautifulSoup(enum_member_text, "lxml").find_all(ENUM_MEMBER)[0]

    def test_empty_member(self):
        enum_member_doc = """
        <Member name="member" />
        """
        member = EnumMember(self._get_enum_member_soup_from_enum_member_text(enum_member_doc))
        self.assertEqual(member.name, "member")
        self.assertFalse(member.annotations)

    def test_member_annotation(self):
        enum_member_doc = """
            <Member name="member">
             <Annotation />
            </Member>
            """
        member = EnumMember(self._get_enum_member_soup_from_enum_member_text(enum_member_doc))
        self.assertEqual(member.name, "member")
        self.assertEqual(len(member.annotations), 1)


class EnumMemberValidationUnitTest(unittest.TestCase):
    BASE_ENUM_TYPE = "base.enum_type"
    DERIVED_ENUM_TYPE = "derived.enum_type"

    def _get_enum_type_soup_from_enum_type_text(self, enum_type_text):
        return BeautifulSoup(enum_type_text, "lxml").find_all(ENUM_TYPE)[0]

    def setUp(self):
        self.metadata_container = MetadataContainer()
        base_enum_type_soup = self._get_enum_type_soup_from_enum_type_text("""
         <EnumType name="{base_enum_type}">
          <Member name="member1"/>
          <Member name="member2"/>
         </EnumType>
         """.format(base_enum_type=EnumMemberValidationUnitTest.BASE_ENUM_TYPE))

        derived_enum_type_soup = self._get_enum_type_soup_from_enum_type_text("""
         <EnumType name="{derived_enum_type}" BaseType="{base_enum_type}">
          <Member name="member3"/>
         </EnumType>
        """.format(derived_enum_type=EnumMemberValidationUnitTest.DERIVED_ENUM_TYPE,
                   base_enum_type=EnumMemberValidationUnitTest.BASE_ENUM_TYPE))

        self.metadata_container.types[EnumMemberValidationUnitTest.BASE_ENUM_TYPE] = EnumType(self.metadata_container, "base",
                                                                                              base_enum_type_soup)

        self.metadata_container.types[EnumMemberValidationUnitTest.DERIVED_ENUM_TYPE] = EnumType(
            self.metadata_container, "derived", derived_enum_type_soup)

    def test_correct_enum_member(self):
        self.assertEqual(self.metadata_container.types[EnumMemberValidationUnitTest.BASE_ENUM_TYPE].validate("member1", "", None),
                         ValidationStatus.PASSED)

    def test_incorrect_enum_member(self):
        self.assertEqual(self.metadata_container.types[EnumMemberValidationUnitTest.BASE_ENUM_TYPE].validate("member3", "", None),
                         ValidationStatus.FAILED)

    def test_derived_enum_member(self):
        self.assertEqual(
            self.metadata_container.types[EnumMemberValidationUnitTest.DERIVED_ENUM_TYPE].validate("member1", "", None),
            ValidationStatus.PASSED)
