import unittest
import sys, inspect, os

from cts_core.discovery.discovery_container import DiscoveryContainer
from cts_core.discovery.api_resource import ApiResource
from cts_core.metadata.metadata_constants import MetadataConstants2_2
from unit_tests.helpers.stdout_capture import StdoutCapture
from cts_core.validation.validation_status import ValidationStatus
from cts_core.metadata.metadata_manager import MetadataManager

# import module that is above this source tree
current_dir = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))
sys.path.append(os.path.join(current_dir, '../../..', 'tests_packages/Rack_Scale_2_2_POD_Manager/required'))
from hardware_check_list import HardwareCheckList

# Hardware CheckList subclasses TestScript which does bad and unnecessary here things in the __init__
# Use monkey patching to skip the initialization
from cts_framework.helpers.python.cts_test_script import CtsTestScript
def test_script_init_monkey_patch(self, configuration):
    # do nothing..this test need only to execute one method
    pass
CtsTestScript.__init__ = test_script_init_monkey_patch

METADATA = """
    <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="Resource">
        <EntityType Name="Resource" Abstract="true"/>
        </EntityType>
    </Schema>

    <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="Resource.v1_0_0">
      <EntityType Name="Resource" BaseType="Resource.Resource" Abstract="true">
        <Key>
          <PropertyRef Name="Id"/>
        </Key>
        <Property Name="Id" Type="Resource.Id" Nullable="false">
            <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
        </Property>
        <Property Name="Description" Type="Resource.Description">
            <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
        </Property>
        <Property Name="Name" Type="Resource.Name" Nullable="false">
            <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
        </Property>
      </EntityType>
    </Schema>

    <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="Chassis">
      <EntityType Name="Chassis" BaseType="Resource.v1_0_0.Resource" Abstract="true">
      </EntityType>
    </Schema>

    <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="Chassis.v1_0_0">
      <EntityType Name="Chassis" BaseType="Chassis.Chassis">
        <Property Name="ChassisType" Nullable="false" Type="Chassis.v1_0_0.ChassisType">
        </Property>
      </EntityType>

      <EnumType Name="ChassisType">
        <Member Name="Rack">
          <Annotation Term="OData.Description" String="An equipment rack, typically a 19-inch wide freestanding unit"/>
        </Member>
        <Member Name="Module">
          <Annotation Term="OData.Description" String="A small, typically removable, chassis or card which contains devices for a particular subsystem or function"/>
        </Member>
      </EnumType>
    </Schema>

    <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="Chassis.v1_2_0">
      <EntityType Name="Chassis" BaseType="Chassis.v1_0_0.Chassis">
      </EntityType>
    </Schema>

"""

#/redfish/v1/Chassis/rack-with-powerzones
RACK_WITH_POWER_ZONES = {
    "@odata.type": "#Chassis.v1_0_0.Chassis",
    "Name": "ChassisRack",
    "@odata.id": "/redfish/v1/Chassis/rack-with-powerzones",
    "@odata.context": "/redfish/v1/$metadata#Chassis/Members/$entity",
    "PowerZones": {
        "@odata.id": "/redfish/v1/Chassis/rack-with-powerzones/PowerZones"
    },
    "Power": {
        "@odata.id": "/redfish/v1/Chassis/rack-with-powerzones/Power"
    },
    "ChassisType": "Rack",
    "Description": "Rack 1"
}

#/redfish/v1/Chassis/rack-with-power
RACK_WITH_POWER = {
    "@odata.type": "#Chassis.v1_0_0.Chassis",
    "Name": "ChassisRack",
    "@odata.id": "/redfish/v1/Chassis/rack-with-power",
    "@odata.context": "/redfish/v1/$metadata#Chassis/Members/$entity",
    "PowerZones": {
        "@odata.id": "/redfish/v1/Chassis/rack-with-power/PowerZones"
    },
    "Power": {
        "@odata.id": "/redfish/v1/Chassis/rack-with-power/Power"
    },
    "ChassisType": "Rack",
    "Model": "RackScale_Rack",
    "Manufacturer": "Intel",
    "Id": "2-c-rack",
    "Description": "Rack 1"
}

#/redfish/v1/Chassis/rack-with-powerzones/PowerZones
POWER_ZONES = {
    "@odata.type": "#PowerZoneCollection.PowerZoneCollection",
    "Description": "Power Zones Collection",
    "Members@odata.count": 0,
    "@odata.id": "/redfish/v1/Chassis/rack-with-powerzones/PowerZones",
    "@odata.context": "/redfish/v1/$metadata#Chassis/Members/1-c-1/PowerZones/$entity",
    "Members": [
        {"@odata.id": "/redfish/v1/Chassis/rack-with-powerzones/PowerZones/1"}
    ],
    "Name": "Power Zones Collection"
}

#/redfish/v1/Chassis/rack-with-powerzones/PowerZones/1
POWER_ZONE_1 = {
    "@odata.id": "/redfish/v1/Chassis/rack-with-powerzones/PowerZones/1",
    "@odata.type": "#PowerZone.v1_0_0.PowerZone",
    "PowerConsumedWatts": 135,
    "Id": "2-c-rack-pz-1",
    "Name": "PowerZone"
}


#/redfish/v1/Chassis/rack-with-powerzones/Power
POWER_EMPTY = {
    "@odata.context" : "/redfish/v1/$metadata#Chassis/Members/c3-2/Power/$entity",
    "@odata.id" : "/redfish/v1/Chassis/rack-with-powerzones/Power",
    "@odata.type" : "Power.v1_1_0.Power",
    "Id" : "c3p-2",
    "Name" : "RACK_POWER",
    "Description" : "PowerDescription",
    "Redundancy" : [ ],
    "PowerControl" : [ ],
    "Voltages" : [ ],
    "PowerSupplies" : [ ]
}

#/redfish/v1/Chassis/rack-with-power/PowerZones
POWER_ZONES_EMPTY = {
    "@odata.type": "#PowerZoneCollection.PowerZoneCollection",
    "Description": "Power Zones Collection",
    "Members@odata.count": 0,
    "@odata.id": "/redfish/v1/Chassis/rack-with-power/PowerZones",
    "@odata.context": "/redfish/v1/$metadata#Chassis/Members/1-c-1/PowerZones/$entity",
    "Members": [],
    "Name": "Power Zones Collection"
}


#/redfish/v1/Chassis/rack-with-power/Power
POWER_WITH_POWER_CONTROL = {
    "@odata.context" : "/redfish/v1/$metadata#Chassis/Members/c3-2/Power/$entity",
    "@odata.id" : "/redfish/v1/Chassis/rack-with-power/Power",
    "@odata.type" : "Power.v1_1_0.Power",
    "Id" : "c3p-2",
    "Name" : "RACK_POWER",
    "Description" : "PowerDescription",
    "Redundancy" : [ ],
    "PowerControl" : [
        {"@odata.id" : "/redfish/v1/Chassis/rack-with-power/Power/PowerControl/1"}
    ],
    "Voltages" : [ ],
    "PowerSupplies" : [ ]
}

#/redfish/v1/Chassis/rack-with-power/Power/PowerControl/1
POWER_CONTROL_1 = {
    "@odata.id" : "/redfish/v1/Chassis/rack-with-power/Power/PowerControl/1",
    "@odata.type" : "Power.v1_1_0.PowerControl",

    "PowerConsumedWatts" : 22.33
}


class PowerMonitoringUnitTest(unittest.TestCase):
    def setUp(self):
        self.discovery_container = DiscoveryContainer()

        metadata_manager = MetadataManager(["qualifier"])
        self.metadata_container = metadata_manager.read_metadata_from_strings(METADATA)

        HardwareCheckList.MetadataConstants = MetadataConstants2_2
        self.checklist = HardwareCheckList(dict())

        #monkey patch
        self.checklist._load_metadata = lambda : self.metadata_container

    def test_should_fail_on_empty_pod(self):
        with StdoutCapture() as output:
            self.checklist.at_least_one_compute_module_in_pod(self.discovery_container)

        self.assertEqual(ValidationStatus.FAILED, self.checklist.validation_status)

    def test_should_pass_for_chassis_with_power_zone(self):

        self.discovery_container.add_resource(ApiResource(
            "/redfish/v1/Chassis/rack-with-powerzones", 'netloc', RACK_WITH_POWER_ZONES, "Chassis.v1_1_0.Chassis"))
        self.discovery_container.add_resource(ApiResource(
            "/redfish/v1/Chassis/rack-with-powerzones/PowerZones", 'netloc', POWER_ZONES,
                                                          "PowerZoneCollection.PowerZoneCollection"))
        self.discovery_container.add_resource(ApiResource(
            "/redfish/v1/Chassis/rack-with-powerzones/PowerZones/1", 'netloc',POWER_ZONE_1,
            "PowerZone.v1_0_0.PowerZone"))
        self.discovery_container.add_resource(ApiResource(
            "/redfish/v1/Chassis/rack-with-powerzones/Power", 'netloc', POWER_EMPTY, "Power.v1_1_0.Power"))

        with StdoutCapture() as output:
            self.checklist.power_monitoring_support(self.discovery_container)

        self.assertEqual(ValidationStatus.PASSED, self.checklist.validation_status)

    def test_should_pass_for_chassis_with_power(self):

        self.discovery_container.add_resource(ApiResource("/redfish/v1/Chassis/rack-with-power",
                                                          'netloc', RACK_WITH_POWER, "Chassis.v1_1_0.Chassis"))
        self.discovery_container.add_resource(ApiResource(
            "/redfish/v1/Chassis/rack-with-power/PowerZones", 'netloc', POWER_ZONES_EMPTY, "PowerZoneCollection.PowerZoneCollection"))
        self.discovery_container.add_resource(ApiResource(
            "/redfish/v1/Chassis/rack-with-power/Power", 'netloc', POWER_WITH_POWER_CONTROL, "Power.v1_1_0.Power"))
        self.discovery_container.add_resource(ApiResource(
            "/redfish/v1/Chassis/rack-with-power/Power/PowerControl/1", 'netloc', POWER_CONTROL_1, "Power.v1_1_0.PowerControl"))

        with StdoutCapture() as output:
            self.checklist.power_monitoring_support(self.discovery_container)

        self.assertEqual(ValidationStatus.PASSED, self.checklist.validation_status)
