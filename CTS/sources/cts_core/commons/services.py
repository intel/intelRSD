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

    PODM_2_4 = "PODM_2_4"
    PSME_2_4 = "PSME_2_4"
    RMM_2_4  = "RMM_2_4"
    SS_2_4   = "SS_2_4"

    SCENARIO_2_4 = "SCENARIO_2_4"
    RACKSCALE_2_4 = "RACKSCALE_2_4"

    PODM_2_5 = "PODM_2_5"
    PSME_2_5 = "PSME_2_5"
    RMM_2_5  = "RMM_2_5"
    SS_2_5   = "SS_2_5"

    SCENARIO_2_5 = "SCENARIO_2_5"
    RACKSCALE_2_5 = "RACKSCALE_2_5"
    PROFILES_2_5 = "PROFILES_2_5"

    REDFISH_2017_3 = "REDFISH_2017_3"
    REDFISH_2018_1 = "REDFISH_2018_1"
    REDFISH_2018_2 = "REDFISH_2018_2"
    REDFISH_2018_3 = "REDFISH_2018_3"
    REDFISH_2019_1 = "REDFISH_2019_1"

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
                ServiceTypes.PODM_2_4,
                ServiceTypes.PSME_2_4,
                ServiceTypes.RMM_2_4,
                ServiceTypes.SS_2_4,
                ServiceTypes.SCENARIO_2_4,
                ServiceTypes.PODM_2_5,
                ServiceTypes.PSME_2_5,
                ServiceTypes.RMM_2_5,
                ServiceTypes.SS_2_5,
                ServiceTypes.SCENARIO_2_5,
                ServiceTypes.PROFILES_2_5,
                ServiceTypes.REDFISH_2017_3,
                ServiceTypes.REDFISH_2018_1,
                ServiceTypes.REDFISH_2018_2,
                ServiceTypes.REDFISH_2018_3,
                ServiceTypes.REDFISH_2019_1
                ]

    @staticmethod
    def list_s():
        return ", ".join(ServiceTypes.all())


def RegisterService(*services):
    def inner(cls):
        cls.registered_services = services
        return cls
    return inner
