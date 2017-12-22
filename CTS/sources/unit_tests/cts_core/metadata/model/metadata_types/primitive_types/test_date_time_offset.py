import unittest

from mock import Mock

from cts_core.metadata.model.metadata_types.primitive_types.date_time_offset import DateTimeOffset
from cts_core.validation.validation_status import ValidationStatus


class DateTimeOffsetUnitTest(unittest.TestCase):
    def setUp(self):
        self.date_time_offset = DateTimeOffset()

    def test_correct_value(self):
        self.assertEqual(self.date_time_offset.validate("2002-10-10T17:00:00Z", ""), ValidationStatus.PASSED)
        self.assertEqual(self.date_time_offset.validate("2016-12-05T15:59:09-08:00", ""), ValidationStatus.PASSED)

    def test_incorrect_strings(self):
        self.assertEqual(self.date_time_offset.validate("2002-13-32T25:61:61Z", ""), ValidationStatus.FAILED)

    def test_none_value(self):
        self.assertEqual(self.date_time_offset.validate(None, ""), ValidationStatus.FAILED)

    def test_dict(self):
        self.assertEqual(self.date_time_offset.validate(dict(), ""), ValidationStatus.FAILED)

    def test_list(self):
        self.assertEqual(self.date_time_offset.validate(list(), ""), ValidationStatus.FAILED)

    def test_numbers(self):
        self.assertEqual(self.date_time_offset.validate(1, ""), ValidationStatus.FAILED)

    def test_annotations(self):
        annotations = {"Validation.Pattern": Mock(value="2002-10-10T17:00:59Z")}

        self.assertEqual(self.date_time_offset.validate("2002-10-10T17:00:59Z", "", annotations), ValidationStatus.PASSED)
        self.assertEqual(self.date_time_offset.validate("2002-10-11T17:00:59Z", "", annotations), ValidationStatus.FAILED)
