import unittest
import sys, inspect, os

from cts_core.discovery.discovery_container import DiscoveryContainer
from cts_core.discovery.api_resource import ApiResource
from cts_core.metadata.metadata_constants import MetadataConstants2_1
from unit_tests.helpers.stdout_capture import StdoutCapture
from cts_core.validation.validation_status import ValidationStatus
from cts_framework.helpers.python.result_status import ResultStatus
from cts_core.metadata.metadata_manager import MetadataManager

# import module that is above this source tree
current_dir = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))
sys.path.append(os.path.join(current_dir, '../../..', 'tests_packages/Rack_Scale_2_1_POD_Manager/required'))
from hardware_check_list import HardwareCheckList

# Hardware CheckList subclasses TestScript which does bad and unnecessary here things in the __init__
# Use monkey patching to skip the initialization
from cts_framework.helpers.python.cts_test_script import CtsTestScript
def test_script_init_monkey_patch(self, configuration):
    # do nothing..this test need only to execute one method
    pass
CtsTestScript.__init__ = test_script_init_monkey_patch

METADATA = """
<Schemas>
    <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="Resource">
        <EntityType Name="Resource" Abstract="true"/>
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
      <EntityType Name="Chassis" BaseType="Resource.v1_0_0.Resource" Abstract="true"/>
    </Schema>

    <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="Chassis.v1_0_0">
      <EntityType Name="Chassis" BaseType="Chassis.Chassis">
        <Property Name="ChassisType" Nullable="false" Type="Chassis.v1_0_0.ChassisType"/>
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
      <EntityType Name="Chassis" BaseType="Chassis.v1_0_0.Chassis"/>
    </Schema>

    <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="ComputerSystem">
      <EntityType Name="ComputerSystem" BaseType="Resource.v1_0_0.Resource" Abstract="true">
        <Annotation Term="OData.Description" String="A Computer System represents a general purpose machine or system (as opposed to an appliance) instance and the software-visible resources (items within the data plane) such as memory, CPU and other devices that can be accessed from that machine.  Details of those resources or subsystems are also linked through this resource."/>
        <Annotation Term="OData.LongDescription" String="This resource shall be used to represent resources that represent a computing system (as opposed to an appliance) in the Redfish specification."/>
      </EntityType>
    </Schema>

    <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="ComputerSystem.v1_0_0">
      <EntityType Name="ComputerSystem" BaseType="ComputerSystem.ComputerSystem">
        <Property Name="ProcessorSummary" Type="ComputerSystem.v1_0_0.ProcessorSummary"  Nullable="false">
          <Annotation Term="OData.Description" String="This object describes the central processors of the system in general detail."/>
		  <Annotation Term="OData.LongDescription" String="This object shall contain properties which describe the central processors for the current resource."/>
        </Property>
        <Property Name="MemorySummary" Type="ComputerSystem.v1_0_0.MemorySummary"  Nullable="false">
          <Annotation Term="OData.Description" String="This object describes the central memory of the system in general detail."/>
          <Annotation Term="OData.LongDescription" String="This object shall contain properties which describe the central memory for the current resource."/>
        </Property>
        <NavigationProperty Name="Processors" Type="ProcessorCollection.ProcessorCollection" ContainsTarget="true" Nullable="false">
           <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
           <Annotation Term="OData.Description" String="A reference to the collection of Processors associated with this system"/>
           <Annotation Term="OData.LongDescription" String="The value of this property shall be a link to a collection of type ProcessorCollection."/>
           <Annotation Term="OData.AutoExpandReferences"/>
        </NavigationProperty>
      </EntityType>

      <ComplexType Name="ProcessorSummary">
        <Property Name="Count" Type="Edm.Int64"/>
      </ComplexType>

      <ComplexType Name="MemorySummary">
        <Property Name="TotalSystemMemoryGiB" Type="Edm.Int64"/>
      </ComplexType>

    </Schema>
</Schemas>
"""

class AtLeastOneComputeModuleInPodUnitTest(unittest.TestCase):
    def setUp(self):
        self.discovery_container = DiscoveryContainer()

        metadata_manager = MetadataManager(["qualifier"])
        self.metadata_container = metadata_manager.read_metadata_from_strings("Unknown", METADATA)

        HardwareCheckList.MetadataConstants = MetadataConstants2_1
        self.checklist = HardwareCheckList(dict())

        #monkey patch
        self.checklist._load_metadata = lambda : self.metadata_container

    def test_should_fail_on_empty_pod(self):
        with StdoutCapture() as output:
            self.checklist.at_least_one_compute_module_in_pod(self.discovery_container)

        self.assertIn('STATUS::Failed', output)

    def test_should_fail_when_no_compute_module(self):
        chassisRack = {
              "@odata.context": "/redfish/v1/$metadata#Chassis/Members/$entity",
              "@odata.id": "/redfish/v1/Chassis/Rack1",
              "@odata.type": "#Chassis.v1_2_0.Chassis",
              "ChassisType": "Rack",
            }

        self.discovery_container.add_resource(
            ApiResource("/redfish/v1/Chassis/Rack1", 'netloc', chassisRack,
                        "Chassis.v1_2_0.Chassis"))

        with StdoutCapture() as output:
            self.checklist.at_least_one_compute_module_in_pod(self.discovery_container)

        self.assertIn('ERROR::Computer System with memory and processors not found', output.raw)
        self.assertIn('STATUS::Failed', output)

    def test_should_fail_when_memory_invalid_on_computer_system(self):
        system = {
            "@odata.context": "/redfish/v1/$metadata#Systems/Members/$entity",
            "@odata.id": "/redfish/v1/Systems/s1-2",
            "@odata.type": "#ComputerSystem.v1_0_0.ComputerSystem",
            "ProcessorSummary" : {
                "Count" : 1
            },
            "MemorySummary": {
                "TotalSystemMemoryGiB": 0
            }
        }

        self.discovery_container.add_resource(ApiResource("/redfish/v1/ComputerSystem/1",
                                                          'netloc', system, "ComputerSystem.v1_1_0.ComputerSystem"))

        with StdoutCapture() as output:
            self.checklist.at_least_one_compute_module_in_pod(self.discovery_container)

        self.assertIn('ERROR::Computer System with memory and processors not found', output.raw)
        self.assertIn('STATUS::Failed', output)

    def test_should_fail_when_processors_invalid_on_computer_system(self):
        system = {
            "@odata.context": "/redfish/v1/$metadata#Systems/Members/$entity",
            "@odata.id": "/redfish/v1/Systems/s1-2",
            "@odata.type": "#ComputerSystem.v1_0_0.ComputerSystem",
            "ProcessorSummary" : {
                "Count" : 0
            },
            "MemorySummary": {
                "TotalSystemMemoryGiB": 8
            }
        }

        self.discovery_container.add_resource(ApiResource("/redfish/v1/ComputerSystem/1",
                                                          'netloc', system, "ComputerSystem.v1_1_0.ComputerSystem"))

        with StdoutCapture() as output:
            self.checklist.at_least_one_compute_module_in_pod(self.discovery_container)

        self.assertIn('ERROR::Computer System with memory and processors not found', output.raw)
        self.assertIn('STATUS::Failed', output)

    def test_should_pass_when_compute_module_is_present(self):
        system = {
            "@odata.context": "/redfish/v1/$metadata#Systems/Members/$entity",
            "@odata.id": "/redfish/v1/Systems/s1-2",
            "@odata.type": "#ComputerSystem.v1_0_0.ComputerSystem",
            "ProcessorSummary" : {
                "Count" : 1
            },
            "MemorySummary": {
                "TotalSystemMemoryGiB": 8
            }
        }

        self.discovery_container.add_resource(ApiResource("/redfish/v1/ComputerSystem/1",
                                                          'netloc', system, "ComputerSystem.v1_1_0.ComputerSystem"))

        with StdoutCapture() as output:
            self.checklist.at_least_one_compute_module_in_pod(self.discovery_container)

        self.assertIn('Found compute module with processors and memory', "".join(output))
        self.assertIn('STATUS::Passed', output)



