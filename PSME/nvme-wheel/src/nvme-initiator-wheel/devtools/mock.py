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

    MOCK_DISCOVERY_2_4 = \
        """
        Discovery Log Number of Records 2, Generation counter 4 =====Discovery Log Entry 0====== trtype: rdma adrfam: ipv4 subtype: nvme subsystem treq: not specified portid: 1 trsvcid: 4420 subnqn: asd 
traddr: 10.6.0.101 rdma_prtype: not specified rdma_qptype: connected rdma_cms: rdma-cm rdma_pkey: 0x0000 =====Discovery Log Entry 1====== trtype: rdma adrfam: ipv4 subtype: nvme subsystem treq: 
not specified portid: 1 trsvcid: 4420 subnqn: qwe traddr: 10.6.0.101 rdma_prtype: not specified rdma_qptype: connected rdma_cms: rdma-cm rdma_pkey: 0x0000
        """

    LSMOD_RETURN = \
        """
    nvme_rdma              22666  0
nvme_fabrics           12307  1 nvme_rdma
rdma_cm                53139  3 ib_iser,nvme_rdma,rdma_ucm
ib_core               239553  12 ib_iser,ib_cm,rdma_cm,ib_umad,nvme_rdma,ib_uverbs,ib_ipoib,iw_cm,mlx5_ib,ib_ucm,rdma_ucm,mlx4_ib
nvme                   23250  3
nvme_core              46201  7 nvme_fabrics,nvme_rdma,nvme
mlx_compat              2549  19 ib_iser,ib_cm,rdma_cm,ib_umad,nvme_fabrics,ib_core,nvme_rdma,ib_uverbs,nvme,nvme_core,mlx4_en,ib_ipoib,mlx5_core,iw_cm,mlx5_ib,mlx4_core,ib_ucm,rdma_ucm,mlx4_ib
    """
