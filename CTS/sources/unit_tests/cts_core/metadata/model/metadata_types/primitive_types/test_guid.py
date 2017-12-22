import unittest

from mock import Mock

from cts_core.metadata.model.metadata_types.primitive_types.guid import Guid
from cts_core.validation.validation_status import ValidationStatus


class GuidUnitTest(unittest.TestCase):
    def setUp(self):
        self.guid = Guid()

    def test_correct_value(self):
        self.assertEqual(self.guid.validate("12345678-90aa-bbbb-cccc-ddddeeeeffff", ""), ValidationStatus.PASSED)

    def test_incorrect_strings(self):
        self.assertEqual(self.guid.validate("12345678-aaaa-bbbb-cccc-ddddeeeefffg", ""), ValidationStatus.FAILED)

    def test_none_value(self):
        self.assertEqual(self.guid.validate(None, ""), ValidationStatus.FAILED)

    def test_dict(self):
        self.assertEqual(self.guid.validate(dict(), ""), ValidationStatus.FAILED)

    def test_list(self):
        self.assertEqual(self.guid.validate(list(), ""), ValidationStatus.FAILED)

    def test_numbers(self):
        self.assertEqual(self.guid.validate(1, ""), ValidationStatus.FAILED)

    def test_annotations(self):
        annotations = {"Validation.Pattern": Mock(value="12345678-90aa-bbbb-cccc-ddddeeeeffff")}

        self.assertEqual(self.guid.validate("12345678-90aa-bbbb-cccc-ddddeeeeffff", "", annotations), ValidationStatus.PASSED)
        self.assertEqual(self.guid.validate("12345678-90aa-bbbb-cccc-ddddeeeefffa", "", annotations), ValidationStatus.FAILED)
