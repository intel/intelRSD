import unittest
from cts_core.commons.external_script import ExecuteExternalScript


class ExternalScriptTest(unittest.TestCase):
    def setUp(self):
        self.execute_script = ExecuteExternalScript(script_path="/non-existing-path.json",
                                                    raw_response=None,
                                                    expected_result=None)
        self.achieved_result = self.execute_script._ExecuteExternalScript__achieved_result = 1

    def test_initialize_with_path_to_non_exist_file(self):
        self.assertIsNone(self.execute_script.script_path)

    def test_verify_expected_result_with_achieved(self):
        self.assertEqual(self.achieved_result, 1)

    def test_verify_expected_result_with_achieved_but_different_type(self):
        self.assertNotEqual(self.achieved_result, "1")
