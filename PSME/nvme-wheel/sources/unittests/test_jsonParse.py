from unittest import TestCase

from nvme_framework.helpers.json_parser import JsonParse


class TestJsonParse(TestCase):
    def setUp(self):
        self.jp = JsonParse()

    def test_clean_output(self):
        expected_result = ['trtype:rdma', 'adrfam:ipv4', 'subtype:nvmesubsystem', 'treq:notspecified', 'portid:1', 'trsvcid:4420', 'subnqn:asd', 'traddr:10.6.0.101', 'rdma_prtype:notspecified', 'rdma_qptype:connected', 'rdma_cms:rdma-cm']
        match = self.jp.clean_output(self.MOCK_DISCOVERY)

        self.assertEqual(expected_result, match)

    def test_clean_output_with_nqn_prefix(self):
        expected_result = ['trtype:rdma', 'adrfam:ipv4', 'subtype:nvmesubsystem', 'treq:notspecified', 'portid:1', 'trsvcid:4420', 'subnqn:nqn.2014-08.org.nvmexpress:uuid:asd', 'traddr:10.6.0.101', 'rdma_prtype:notspecified', 'rdma_qptype:connected', 'rdma_cms:rdma-cm']
        match = self.jp.clean_output(self.MOCK_DISCOVERY_SUBNQN_WITH_PREFIX)

        self.assertEqual(expected_result, match)

    def test_try_convert_discovery_output_to_dictionary(self):
        expected_result = [{'subtype': 'nvmesubsystem', 'adrfam': 'ipv4', 'portid': '1', 'traddr': '10.6.0.101', 'rdma_prtype': 'notspecified', 'subnqn': 'nqn.2014-08.org.nvmexpress:uuid:asd', 'treq': 'notspecified', 'rdma_qptype': 'connected', 'trsvcid': '4420', 'rdma_cms': 'rdma-cm'}]
        match = self.jp.try_convert_discovery_output_to_dictionary(self.MOCK_DISCOVERY_SUBNQN_WITH_PREFIX)

        self.assertEqual(expected_result, match)

    MOCK_DISCOVERY = \
        """
            Discovery Log Number of Records 1, Generation counter 1
        =====Discovery Log Entry 0======
        trtype:  rdma
        adrfam:  ipv4
        subtype: nvme subsystem
        treq:    not specified
        portid:  1
        trsvcid: 4420

        subnqn:  asd
        traddr:  10.6.0.101

        rdma_prtype: not specified
        rdma_qptype: connected
        rdma_cms:    rdma-cm
        rdma_pkey: 0x0000
        """

    MOCK_DISCOVERY_SUBNQN_WITH_PREFIX = \
        """
            Discovery Log Number of Records 1, Generation counter 1
        =====Discovery Log Entry 0======
        trtype:  rdma
        adrfam:  ipv4
        subtype: nvme subsystem
        treq:    not specified
        portid:  1
        trsvcid: 4420

        subnqn:  nqn.2014-08.org.nvmexpress:uuid:asd
        traddr:  10.6.0.101

        rdma_prtype: not specified
        rdma_qptype: connected
        rdma_cms:    rdma-cm
        rdma_pkey: 0x0000
        """