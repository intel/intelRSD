import unittest
from cts_core.commons.json_comparator import JsonComparator


class JsonComparatorUnitTest(unittest.TestCase):
    def test_empty_dicts(self):
        self.assertTrue(JsonComparator.compare_jsons(dict(), dict()))

    def test_empty_lists(self):
        self.assertTrue(JsonComparator.compare_jsons(list(), list()))

    def test_none(self):
        self.assertTrue(JsonComparator.compare_jsons(None, None))

    def test_type_mismatch(self):
        self.assertFalse(JsonComparator.compare_jsons(dict(), list()))

    def test_key_mismatch(self):
        self.assertFalse(JsonComparator.compare_jsons(dict(a=1), dict(b=1)))

    def test_values_mismatch(self):
        self.assertFalse(JsonComparator.compare_jsons(dict(a=1), dict(a=2)))

    def test_elements_in_list_mismatch(self):
        self.assertFalse(JsonComparator.compare_jsons([1, 2, 3], [1, 2, 4]))

    def test_list_length_mismatch(self):
        self.assertFalse(JsonComparator.compare_jsons([1, 2, 3], [1, 2]))

    def test_fields_not_in_golden(self):
        json = dict(
                a=1,
                b="xyz",
                c=[1, 2 ,3])

        golden = dict(
                  a=1,
                  b="xyz")

        self.assertFalse(JsonComparator.compare_jsons(json, golden))
        self.assertTrue(JsonComparator.compare_json_to_golden(json, golden))


    def test_fields_in_ignore_list(self):
        json = dict(
                a=1,
                b="xyz",
                c=[1, 2 ,3])

        golden = dict(
                  a=1,
                  b="xyz",
                  d=dict())

        self.assertFalse(JsonComparator.compare_jsons(json, golden))
        self.assertFalse(JsonComparator.compare_json_to_golden(json, golden))
        self.assertTrue(JsonComparator.compare_json_to_golden(json, golden, ignore = ["/d"]))
