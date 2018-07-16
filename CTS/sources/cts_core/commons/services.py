
class ServiceTypes:
    PODM_1_2 = "PODM_1_2"
    PSME_1_2 = "PSME_1_2"
    RMM_1_2  = "RMM_1_2"
    SS_1_2   = "SS_1_2"

    PODM_2_1 = "PODM_2_1"
    PSME_2_1 = "PSME_2_1"
    RMM_2_1  = "RMM_2_1"
    SS_2_1   = "SS_2_1"

    PODM_2_1_2 = "PODM_2_1_2"
    PSME_2_1_2 = "PSME_2_1_2"
    RMM_2_1_2  = "RMM_2_1_2"
    SS_2_1_2   = "SS_2_1_2"

    PODM_2_1_3 = "PODM_2_1_3"
    PSME_2_1_3 = "PSME_2_1_3"
    RMM_2_1_3  = "RMM_2_1_3"
    SS_2_1_3   = "SS_2_1_3"

    PODM_2_1_4 = "PODM_2_1_4"
    PSME_2_1_4 = "PSME_2_1_4"
    RMM_2_1_4  = "RMM_2_1_4"
    SS_2_1_4   = "SS_2_1_4"

    PODM_2_2 = "PODM_2_2"
    PSME_2_2 = "PSME_2_2"
    RMM_2_2  = "RMM_2_2"
    SS_2_2   = "SS_2_2"

    PODM_2_3 = "PODM_2_3"
    PSME_2_3 = "PSME_2_3"
    RMM_2_3  = "RMM_2_3"
    SS_2_3   = "SS_2_3"


    @staticmethod
    def all():
        return [ServiceTypes.PODM_1_2,
                ServiceTypes.PSME_1_2,
                ServiceTypes.RMM_1_2,
                ServiceTypes.SS_1_2,
                ServiceTypes.PODM_2_1,
                ServiceTypes.PSME_2_1,
                ServiceTypes.RMM_2_1,
                ServiceTypes.SS_2_1,
                ServiceTypes.PODM_2_1_2,
                ServiceTypes.PSME_2_1_2,
                ServiceTypes.RMM_2_1_2,
                ServiceTypes.SS_2_1_2,
                ServiceTypes.PODM_2_1_3,
                ServiceTypes.PSME_2_1_3,
                ServiceTypes.RMM_2_1_3,
                ServiceTypes.SS_2_1_3,
                ServiceTypes.PODM_2_1_4,
                ServiceTypes.PSME_2_1_4,
                ServiceTypes.RMM_2_1_4,
                ServiceTypes.SS_2_1_4,
                ServiceTypes.PODM_2_2,
                ServiceTypes.PSME_2_2,
                ServiceTypes.RMM_2_2,
                ServiceTypes.SS_2_2,
                ServiceTypes.PODM_2_3,
                ServiceTypes.PSME_2_3,
                ServiceTypes.RMM_2_3,
                ServiceTypes.SS_2_3,
                ]

    @staticmethod
    def list_s():
        return ", ".join(ServiceTypes.all())


def RegisterService(*services):
    def inner(cls):
        cls.registered_services = services
        return cls
    return inner
