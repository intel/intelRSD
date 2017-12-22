import unittest
import os
from mock import mock, Mock, MagicMock
from cts_core.commons.replay_controller import ReplayController

class ReplayControllerTest(unittest.TestCase):
    def setUp(self):
        pass

    def test_reply_mode_off_when_script_execution_id_not_set_via_env_var(self):
        with mock.patch('cts_core.commons.replay_controller.ReplayController._read_request_ids_from_database') as read_ids:
            controller_class = ReplayController().__class__()
            controller_class.initialize()
            self.assertFalse(controller_class.replay_mode_on())

    @mock.patch.dict(os.environ, {ReplayController.CTS_REPLAY_SCRIPT_EXECUTION_ID: '11'})
    def test_reply_mode_off_when_list_of_registered_requests_is_empty(self):
        with mock.patch('cts_core.commons.replay_controller.ReplayController._read_request_ids_from_database') as read_ids:
            controller_class = ReplayController().__class__()
            read_ids.return_value = [] #empty list
            controller_class.initialize()
            self.assertFalse(controller_class.replay_mode_on())

    @mock.patch.dict(os.environ, {ReplayController.CTS_REPLAY_SCRIPT_EXECUTION_ID: '11'})
    def test_reply_mode_on_when_correct_script_execution_id(self):
        with mock.patch('cts_core.commons.replay_controller.ReplayController._read_request_ids_from_database') as read_ids:
            controller_class = ReplayController().__class__()
            read_ids.return_value = [22,33]
            controller_class.initialize()
            self.assertTrue(controller_class.replay_mode_on())

    @mock.patch.dict(os.environ, {ReplayController.CTS_REPLAY_SCRIPT_EXECUTION_ID: '11'})
    def test_retrieve_request_from_database(self):
        with mock.patch('cts_core.commons.replay_controller.ReplayController._read_request_ids_from_database') as read_ids:
            controller_class = ReplayController().__class__()
            read_ids.return_value = [22,33]
            controller_class.initialize()

            with mock.patch('cts_framework.db.dao.http_request_dao.HttpRequestDAO.retrieve') as retrieve:
                retrieve.return_value = ('_method', 'url', '{"anything":"something"}', '_response', '_status_code')
                self.assertEqual('_response', controller_class.request("http_method", "url", anything='something'))
                retrieve.assert_called_with(22)

    def test_request_returns_none_when_replay_mode_is_off(self):
        controller_class = ReplayController().__class__()
        controller_class.initialize()

        self.assertIsNone(controller_class.request("http_method", "url"))
