from unittest import TestCase


class TestExecuteAction(TestCase):
    def setUp(self):
        from nvme_framework.actions.execute.run import ExecuteAction
        self.execute_action = ExecuteAction

    def test__update_nqn_with_prefix(self):
        target_name = 'nqn.2014-08.org.nvmexpress:uuid:248756f4-c631-e711-8065-a81e84b54893'

        match = self.execute_action._update_nqn_with_prefix(target_name)
        self.assertEqual(target_name, match)

    def test__update_nqn_with_prefix_wrong_uuid(self):
        target_name = '248756f4-c631-e711-8065-a81e84b54893'
        desired_target_name = 'nqn.2014-08.org.nvmexpress:uuid:248756f4-c631-e711-8065-a81e84b54893'

        match = self.execute_action._update_nqn_with_prefix(target_name)
        self.assertEqual(desired_target_name, match)

    def test__remove_prefix_from_nqn(self):
        target_name = 'nqn.2014-08.org.nvmexpress:uuid:248756f4-c631-e711-8065-a81e84b54893'
        desired_target_name = '248756f4-c631-e711-8065-a81e84b54893'

        match = self.execute_action._remove_prefix_from_nqn(target_name)
        self.assertEqual(desired_target_name, match)

    def test__remove_prefix_from_nqn_without_prefix(self):
        target_name = '248756f4-c631-e711-8065-a81e84b54893'
        desired_target_name = '248756f4-c631-e711-8065-a81e84b54893'

        match = self.execute_action._remove_prefix_from_nqn(target_name)
        self.assertEqual(desired_target_name, match)
