import unittest
from mock import Mock, mock, MagicMock
from cts_framework.actions.replay.replay_test_run import ReplayTestRunAction
from cts_framework.tests_managing.test_package.tests_packages_container import TestsPackagesContainer
from unit_tests.helpers.stdout_capture import StdoutCapture


class ReplayActionUnderTest(ReplayTestRunAction):
    def __init__(self):
        self._logger = MagicMock()


class TestsPackagesContainerMock(TestsPackagesContainer):
    def __init__(self):
        package = MagicMock(name='package_name')
        self.packages = [package]

    def filter(self, packages=None, suites=None, scripts=None, script_paths=None, remove_empty=False):
        pass


class ReplayTestRunTest(unittest.TestCase):
    def setUp(self):
        pass

    def test_with_incorrect_replay_id(self):
        replay_action = ReplayActionUnderTest()
        configuration = Mock(replay_id=['incorrect'])

        with StdoutCapture() as output:
            replay_action.process_action(configuration)

        self.assertIn('Replay id has invalid format', output.raw)

    def test_with_nonexistent_replay_id(self):
        with mock.patch('cts_framework.db.dao.script_dao.ScriptDAO.get_script_execution_details') as get_script_execution_details:
            get_script_execution_details.return_value = None

            replay_action = ReplayActionUnderTest()
            configuration = Mock(replay_id=['62'])

            with StdoutCapture() as output:
                replay_action.process_action(configuration)

            self.assertIn('Recording for script execution id=62 not found', output.raw)

    def test_with_invalid_script_path(self):
        with mock.patch('cts_framework.db.dao.script_dao.ScriptDAO.get_script_execution_details') as get_script_execution_details:
            script_execution = Mock(configuration="", script_path="nonexistent")
            get_script_execution_details.return_value = script_execution

            replay_action = ReplayActionUnderTest()
            configuration = Mock(replay_id=['62'])

            with StdoutCapture() as output:
                with self.assertRaises(SystemExit) as cm:
                    replay_action.process_action(configuration)

            self.assertIn('Script to execute not found in any package', output.raw)

    def test_with_correct_script_path(self):
        with mock.patch('cts_framework.db.dao.script_dao.ScriptDAO.get_script_execution_details') as get_script_execution_details:
            script_execution = Mock(configuration="", script_path="valid_script_path")
            get_script_execution_details.return_value = script_execution
            with mock.patch('cts_framework.tests_managing.tests_manager.TestsManager.get_packages') as get_packages:
                get_packages.return_value = TestsPackagesContainerMock()
                replay_action = ReplayActionUnderTest()
                with mock.patch('cts_framework.actions.execute.execute_test_scripts_action.ExecuteTestScriptsAction.execute_configuration_group') \
                        as execute_configuration_group:
                    configuration = Mock(replay_id=['62'])
                    with StdoutCapture() as output:
                        replay_action.process_action(configuration)

                    self.assertIn('Executing', output.raw)
