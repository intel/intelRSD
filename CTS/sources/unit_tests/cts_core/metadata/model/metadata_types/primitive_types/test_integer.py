import unittest

from mock import Mock

from cts_core.metadata.model.metadata_types.primitive_types.integer import Integer16, Integer
from cts_core.validation.validation_status import ValidationStatus


class Integer16UnitTest(unittest.TestCase):
    def setUp(self):
        self.integer_16 = Integer16()

    def test_correct_values(self):
        self.assertEqual(self.integer_16.validate(-1000, ""), ValidationStatus.PASSED)
        self.assertEqual(self.integer_16.validate(1000, ""), ValidationStatus.PASSED)
        self.assertEqual(self.integer_16.validate(0, ""), ValidationStatus.PASSED)

    def test_value_out_of_range(self):
        self.assertEqual(self.integer_16.validate(-1 * (2**16 + 1), ""), ValidationStatus.FAILED)
        self.assertEqual(self.integer_16.validate(2 ** 16, ""), ValidationStatus.FAILED)


class Integer32UnitTest(unittest.TestCase):
    def setUp(self):
        self.integer_32 = Integer16()

    def test_correct_values(self):
        self.assertEqual(self.integer_32.validate(-1000, ""), ValidationStatus.PASSED)
        self.assertEqual(self.integer_32.validate(1000, ""), ValidationStatus.PASSED)
        self.assertEqual(self.integer_32.validate(0, ""), ValidationStatus.PASSED)

    def test_value_out_of_range(self):
        self.assertEqual(self.integer_32.validate(-1 * (2 ** 32 + 1), ""), ValidationStatus.FAILED)
        self.assertEqual(self.integer_32.validate(2 ** 32, ""), ValidationStatus.FAILED)


class Integer64UnitTest(unittest.TestCase):
    def setUp(self):
        self.integer_64 = Integer16()

    def test_correct_values(self):
        self.assertEqual(self.integer_64.validate(-1000, ""), ValidationStatus.PASSED)
        self.assertEqual(self.integer_64.validate(1000, ""), ValidationStatus.PASSED)
        self.assertEqual(self.integer_64.validate(0, ""), ValidationStatus.PASSED)

    def test_value_out_of_range(self):
        self.assertEqual(self.integer_64.validate(-1 * (2 ** 64 + 1), ""), ValidationStatus.FAILED)
        self.assertEqual(self.integer_64.validate(2 ** 64, ""), ValidationStatus.FAILED)


class IntegerUnitTest(unittest.TestCase):
    def setUp(self):
        self.integer = Integer(10)

    def test_none_value(self):
        self.assertEqual(self.integer.validate(None, ""), ValidationStatus.FAILED)

    def test_str(self):
        self.assertEqual(self.integer.validate("", ""), ValidationStatus.FAILED)
        self.assertEqual(self.integer.validate("example_value", ""), ValidationStatus.FAILED)

    def test_dict(self):
        self.assertEqual(self.integer.validate(dict(), ""), ValidationStatus.FAILED)

    def test_list(self):
        self.assertEqual(self.integer.validate(list(), ""), ValidationStatus.FAILED)

    def test_minimum_annotation(self):
        annotations = {"Validation.Minimum": Mock(value=10)}
        self.assertEqual(self.integer.validate(5, "", annotations), ValidationStatus.FAILED)
        self.assertEqual(self.integer.validate(15, "", annotations), ValidationStatus.PASSED)

    def test_maximum_annotation(self):
        annotations = {"Validation.Maximum": Mock(value=10)}
        self.assertEqual(self.integer.validate(5, "", annotations), ValidationStatus.PASSED)
        self.assertEqual(self.integer.validate(15, "", annotations), ValidationStatus.FAILED)

    def test_mixed_annotation(self):
        annotations = {"Validation.Minimum": Mock(value=8), "Validation.Maximum": Mock(value=9)}
        self.assertEqual(self.integer.validate(7, "", annotations), ValidationStatus.FAILED)
        self.assertEqual(self.integer.validate(8, "", annotations), ValidationStatus.PASSED)
        self.assertEqual(self.integer.validate(9, "", annotations), ValidationStatus.PASSED)
        self.assertEqual(self.integer.validate(10, "", annotations), ValidationStatus.FAILED)
