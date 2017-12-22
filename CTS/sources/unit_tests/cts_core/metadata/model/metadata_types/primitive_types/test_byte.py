import unittest

from cts_core.metadata.model.metadata_types.primitive_types.byte import Byte
from cts_core.validation.validation_status import ValidationStatus


class ByteUnitTest(unittest.TestCase):
    def setUp(self):
        self.byte = Byte()

    def test_correct_values(self):
        self.assertEqual(self.byte.validate(0, "", None), ValidationStatus.PASSED)
        self.assertEqual(self.byte.validate(100, "", None), ValidationStatus.PASSED)
        self.assertEqual(self.byte.validate(255, "", None), ValidationStatus.PASSED)

    def test_none_value(self):
        self.assertEqual(self.byte.validate(None, "", None), ValidationStatus.FAILED)

    def test_incorrect_integers(self):
        self.assertEqual(self.byte.validate(-1, "", None), ValidationStatus.FAILED)
        self.assertEqual(self.byte.validate(256, "", None), ValidationStatus.FAILED)

    def test_str(self):
        self.assertEqual(self.byte.validate("", "", None), ValidationStatus.FAILED)
        self.assertEqual(self.byte.validate("example_value", "", None), ValidationStatus.FAILED)

    def test_dict(self):
        self.assertEqual(self.byte.validate(dict(), "", None), ValidationStatus.FAILED)

    def test_list(self):
        self.assertEqual(self.byte.validate(list(), "", None), ValidationStatus.FAILED)
