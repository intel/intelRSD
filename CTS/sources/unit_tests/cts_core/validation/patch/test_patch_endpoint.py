import unittest
from mock import mock
from cts_core.validation.patch.patching_strategy_2_1 import PatchingStrategy2_1
from cts_core.discovery.discovery_container import DiscoveryContainer
from cts_core.metadata.metadata_manager import MetadataManager
from cts_core.validation.validation_status import ValidationStatus
from unit_tests.helpers.stdout_capture import StdoutCapture
from cts_core.discovery.api_resource import ApiResource
from cts_core.validation.patch.metadata_patch_validator import MetadataPatchValidator


METADATA = """
  <Schemas>
    <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="Resource">
        <EntityType Name="Resource" Abstract="true"/>


        <TypeDefinition Name="UUID" UnderlyingType="Edm.Guid"/>

        <!--Complex type that serves as a "property bag" for OEM properties-->
        <ComplexType Name="Oem">
            <Annotation Term="OData.AdditionalProperties" Bool="True"/>
            <Annotation Term="OData.Description" String="Oem extension object."/>
            <Annotation Term="OData.LongDescription"
                        String="This object represents the Oem property.  All values for resources described by this schema shall comply to the requirements as described in the Redfish specification."/>
        </ComplexType>

        <!--This indicates the known state of the resource, such as if it is enabled.-->
        <EnumType Name="State">
            <Member Name="Enabled">
                <Annotation Term="OData.Description" String="This function or resource has been enabled"/>
            </Member>
            <Member Name="Disabled">
                <Annotation Term="OData.Description" String="This function or resource has been disabled"/>
            </Member>
            <Member Name="StandbyOffline">
                <Annotation Term="OData.Description"
                            String="This function or resource is enabled, but awaiting an external action to activate it"/>
            </Member>
            <Member Name="StandbySpare">
                <Annotation Term="OData.Description"
                            String="This function or resource is part of a redundancy set and is awaiting a failover or other external action to activate it."/>
            </Member>
            <Member Name="InTest">
                <Annotation Term="OData.Description" String="This function or resource is undergoing testing"/>
            </Member>
            <Member Name="Starting">
                <Annotation Term="OData.Description" String="This function or resource is starting"/>
            </Member>
            <Member Name="Absent">
                <Annotation Term="OData.Description" String="This function or resource is not present or not detected"/>
            </Member>
        </EnumType>


        <ComplexType Name="Status">
            <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
            <Annotation Term="OData.AdditionalProperties" Bool="False"/>
            <Property Name="State" Type="Resource.State">
                <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
                <Annotation Term="OData.Description"
                            String="This indicates the known state of the resource, such as if it is enabled."/>
                <Annotation Term="OData.LongDescription"
                            String="This property shall represent if this component is available or not and why.  Enabled indicates the resource is available.  Disabled indicates the resource has been intentionally made unavailable but it can be enabled.  Offline indicates the resource is unavailable intentionally and requires action to be made available.  InTest indicates that the component is undergoing testing.  Starting indicates that the resource is on its way to becoming available.  Absent indicates the resources is physically unavailable."/>
            </Property>
            <Property Name="HealthRollup" Type="Resource.Health">
                <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
                <Annotation Term="OData.Description"
                            String="This represents the overall health state from the view of this resource."/>
                <Annotation Term="OData.LongDescription"
                            String="This property shall represent the HealthState of the resource and its dependent resources.  The values shall conform to those defined in the Redfish specification."/>
            </Property>
            <Property Name="Health" Type="Resource.Health">
                <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
                <Annotation Term="OData.Description"
                            String="This represents the health state of this resource in the absence of its dependent resources."/>
                <Annotation Term="OData.LongDescription"
                            String="This property shall represent the HealthState of the resource without considering its dependent resources. The values shall conform to those defined in the Redfish specification."/>
            </Property>
            <Property Name="Oem" Type="Resource.Oem" Nullable="false"/>
        </ComplexType>

        <!--This represents the state of the health of a resource.-->
        <EnumType Name="Health">
            <Member Name="OK">
                <Annotation Term="OData.Description" String="Normal"/>
            </Member>
            <Member Name="Warning">
                <Annotation Term="OData.Description" String="A condition exists that requires attention"/>
            </Member>
            <Member Name="Critical">
                <Annotation Term="OData.Description"
                            String="A critical condition exists that requires immediate attention"/>
            </Member>
        </EnumType>


    </Schema>

    <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="Resource.v1_0_0">
        <EntityType Name="Resource" BaseType="Resource.Resource" Abstract="true">
            <Key>
                <PropertyRef Name="Id"/>
            </Key>
        </EntityType>
    </Schema>

    <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="Endpoint">

      <EntityType Name="Endpoint" BaseType="Resource.v1_0_0.Resource">
        <Annotation Term="OData.Description" String="This is the schema definition for the Endpoint resource.  It represents the properties of an entity that sends or receives protocol defined messages over a transport."/>
        <Annotation Term="OData.LongDescription" String="This resource shall be used to represent a fabric endpoint for a Redfish implementation."/>
      </EntityType>
    </Schema>

    <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="Endpoint.v1_0_0">

      <EntityType Name="Endpoint" BaseType="Endpoint.Endpoint">
        <Annotation Term="OData.Description" String="This is the schema definition for the Endpoint resource. It represents the properties of an entity that sends or receives protocol defined messages over a transport."/>
        <Annotation Term="OData.LongDescription" String="This resource shall be used to represent a fabric endpoint for a Redfish implementation."/>
        <Property Name="Status" Type="Resource.Status"/>
        <Property Name="ConnectedEntities" Type="Collection(Endpoint.v1_0_0.ConnectedEntity)">
          <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
          <Annotation Term="OData.Description" String="All the entities connected to this endpoint."/>
          <Annotation Term="OData.LongDescription" String="This value of this property shall contain all the entities which this endpoint allows access to."/>
        </Property>

        <Property Name="PciId" Type="Endpoint.v1_0_0.PciId">
          <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
          <Annotation Term="OData.Description" String="The PCI ID of the endpoint."/>
          <Annotation Term="OData.LongDescription" String="The value of this property shall be the PCI ID of the endpoint."/>
        </Property>
        <NavigationProperty Name="Redundancy" Type="Collection(Redundancy.Redundancy)" Nullable="false">
          <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
          <Annotation Term="OData.Description" String="Redundancy information for the lower level endpoints supporting this endpoint"/>
          <Annotation Term="OData.AutoExpand"/>
        </NavigationProperty>
        <Property Name="HostReservationMemoryBytes" Type="Edm.Int64">
          <Annotation Term="Measures.Unit" String="By"/>
          <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
          <Annotation Term="OData.Description" String="The amount of memory in Bytes that the Host should allocate to connect to this endpoint."/>
          <Annotation Term="OData.LongDescription" String="The value of this property shall be the amount of memory in Bytes that the Host should allocate to connect to this endpoint."/>
        </Property>

        <Property Name="Actions" Type="Endpoint.v1_0_0.Actions" Nullable="false">
          <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
          <Annotation Term="OData.Description" String="The Actions object contains the available custom actions on this resource."/>
        </Property>
      </EntityType>

      <ComplexType Name="Actions">
        <Annotation Term="OData.AdditionalProperties" Bool="false"/>
        <Property Name="Oem" Type="Endpoint.v1_0_0.OemActions"/>
      </ComplexType>

      <ComplexType Name="OemActions">
        <Annotation Term="OData.AdditionalProperties" Bool="true"/>
      </ComplexType>

      <ComplexType Name="ConnectedEntity">
        <Annotation Term="OData.Description" String="Represents a remote resource that is connected to the network accessible to this endpoint."/>
        <Property Name="EntityType" Type="Endpoint.v1_0_0.EntityType">
          <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
          <Annotation Term="OData.Description" String="The type of the connected entity."/>
          <Annotation Term="OData.LongDescription" String="The value of this property shall indicate if type of connected entity."/>
        </Property>
        <Property Name="EntityRole" Type="Endpoint.v1_0_0.EntityRole">
          <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
          <Annotation Term="OData.Description" String="The role of the connected entity."/>
          <Annotation Term="OData.LongDescription" String="The value of this property shall indicate if the specified entity is an initiator, target, or both."/>
        </Property>
        <Property Name="EntityPciId" Type="Endpoint.v1_0_0.PciId">
          <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
          <Annotation Term="OData.Description" String="The PCI ID of the connected entity."/>
          <Annotation Term="OData.LongDescription" String="The value of this property shall be the PCI ID of the connected PCIe entity."/>
        </Property>
        <Property Name="PciFunctionNumber" Type="Edm.Int64">
          <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
          <Annotation Term="OData.Description" String="The PCI ID of the connected entity."/>
          <Annotation Term="OData.LongDescription" String="The value of this property shall be the PCI Function Number of the connected PCIe entity."/>
        </Property>
        <Property Name="PciClassCode" Type="Edm.String">
          <Annotation Term="OData.Description" String="The Class Code and Subclass code of this PCIe function."/>
          <Annotation Term="OData.LongDescription" String="The value of this property shall be the PCI Class Code and Subclass code of the PCIe device function."/>
          <Annotation Term="Validation.Pattern" String="^0[xX](([a-fA-F]|[0-9]){2}){3}$"/>
        </Property>
        <NavigationProperty Name="EntityLink" Type="Resource.Resource">
          <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
          <Annotation Term="OData.Description" String="A link to the associated entity."/>
          <Annotation Term="OData.LongDescription" String="This property shall be a reference to an entity of the type specified by the description of the value of the EntityType property."/>
        </NavigationProperty>

        <Property Name="Oem" Type="Resource.Oem">
          <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
        </Property>
      </ComplexType>

      <EnumType Name="EntityType">
        <Member Name="StorageInitiator">
          <Annotation Term="OData.Description" String="The entity is a storage initator. The EntityLink property (if present) should be a Storage.StorageController entity"/>
        </Member>
        <Member Name="RootComplex">
          <Annotation Term="OData.Description" String="The entity is a PCI(e) root complex. The EntityLink property (if present) should be a ComputerSystem.ComputerSystem entity"/>
        </Member>
        <Member Name="NetworkController">
          <Annotation Term="OData.Description" String="The entity is a network controller. The EntityLink property (if present) should be an EthernetInterface.EthernetInterface entity"/>
        </Member>
        <Member Name="Drive">
          <Annotation Term="OData.Description" String="The entity is a disk drive. The EntityLink property (if present) should be a Drive.Drive entity"/>
        </Member>
        <Member Name="StorageExpander">
          <Annotation Term="OData.Description" String="The entity is a storage expander. The EntityLink property (if present) should be a Chassis.Chassis entity"/>
        </Member>
        <Member Name="DisplayController">
          <Annotation Term="OData.Description" String="The entity is a display controller."/>
        </Member>
        <Member Name="Bridge">
          <Annotation Term="OData.Description" String="The entity is a PCI(e) bridge."/>
        </Member>
        <Member Name="Processor">
          <Annotation Term="OData.Description" String="The entity is a processor device."/>
        </Member>
      </EnumType>

      <EnumType Name="EntityRole">
        <Member Name="Initiator">
          <Annotation Term="OData.Description" String="The entity is acting as an initiator"/>
        </Member>
        <Member Name="Target">
          <Annotation Term="OData.Description" String="The entity is acting as a target"/>
        </Member>
        <Member Name="Both">
          <Annotation Term="OData.Description" String="The entity is acting as both an initiator and a target"/>
        </Member>
      </EnumType>

      <ComplexType Name="PciId">
        <Property Name="DeviceId" Type="Edm.String">
          <Annotation Term="OData.Description" String="The Device ID of this PCIe function."/>
          <Annotation Term="OData.LongDescription" String="The value of this property shall be the PCI Subsystem Vendor ID of the PCIe device function."/>
          <Annotation Term="Validation.Pattern" String="^0[xX](([a-fA-F]|[0-9]){2}){2}$"/>
        </Property>
        <Property Name="VendorId" Type="Edm.String">
          <Annotation Term="OData.Description" String="The Vendor ID of this PCIe function."/>
          <Annotation Term="OData.LongDescription" String="The value of this property shall be the PCI Vendor ID of the PCIe device function."/>
          <Annotation Term="Validation.Pattern" String="^0[xX](([a-fA-F]|[0-9]){2}){2}$"/>
        </Property>
        <Property Name="SubsystemId" Type="Edm.String">
          <Annotation Term="OData.Description" String="The Subsystem ID of this PCIe function."/>
          <Annotation Term="OData.LongDescription" String="The value of this property shall be the PCI Subsystem Vendor ID of the PCIe device function."/>
          <Annotation Term="Validation.Pattern" String="^0[xX](([a-fA-F]|[0-9]){2}){2}$"/>
        </Property>
        <Property Name="SubsystemVendorId" Type="Edm.String">
          <Annotation Term="OData.Description" String="The Subsystem Vendor ID of this PCIe function."/>
          <Annotation Term="OData.LongDescription" String="The value of this property shall be the PCI Subsystem Vendor ID of the PCIe device function."/>
          <Annotation Term="Validation.Pattern" String="^0[xX](([a-fA-F]|[0-9]){2}){2}$"/>
        </Property>
      </ComplexType>

    </Schema>
  </Schemas>
    """

RESOURCE = \
    {
	    "@odata.type": "#Endpoint.v1_0_0.Endpoint",
	    "Status": {
	        "HealthRollup": "OK",
	        "State": "Enabled",
	        "Health": "OK"
	    },
	    "PciId": {},
	    "HostReservationMemoryBytes": None,
	    "@odata.id": "/redfish/v1/Fabrics/1/Endpoints/8",
	    "@odata.context": "/redfish/v1/$metadata#Endpoint.Endpoint",
	    "ConnectedEntities": [
	        {
	            "EntityPciId": None,
	            "Oem": {},
	            "EntityType": "Drive",
	            "PciFunctionNumber": None,
	            "PciClassCode": None,
	            "EntityRole": "Target",
	            "EntityLink": {
	                "@odata.id": "/redfish/v1/Chassis/1/Drives/17"
	            }
	        }
	    ],
	    "Redundancy": [],
	    "Actions": {
	        "Oem": {}
	    },
	    "Oem": {},
	}

class PatchEndpointTest(unittest.TestCase):
    def setUp(self):
        metadata_manager = MetadataManager(["qualifier"])
        self.metadata_container = metadata_manager.read_metadata_from_strings("Unknown", METADATA)
        self.discovery_container = DiscoveryContainer()
        self.discovery_container.add_resource(ApiResource("/redfish/v1/Fabrics/1/Endpoints/8",
                                                          'netloc', RESOURCE,
                                                          "#Endpoint.v1_0_0.Endpoint"))

    def test_patch(self):
        with mock.patch('cts_core.commons.api_caller.ApiCaller.__init__') as api_caller_init_mock:
            api_caller_init_mock.return_value = None

            with mock.patch('cts_core.validation.patch.metadata_patch_validator.MetadataPatchValidator._validate_patchable_property') as validate_property:
                validate_property.return_value = ValidationStatus.PASSED

                validator = MetadataPatchValidator(self.metadata_container, None, PatchingStrategy2_1())

                with StdoutCapture() as output:
                    self.assertEqual(ValidationStatus.PASSED, validator.validate(self.discovery_container))
