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

    <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="Manager">
        <EntityType Name="Manager" BaseType="Resource.1.0.0.Resource">
            <Annotation Term="OData.Description"
                        String="This is the schema definition for a Manager.  Examples of managers are BMCs, Enclosure Managers, Management Controllers and other subsystems assigned managability functions."/>
            <Annotation Term="OData.LongDescription"
                        String="This resource shall be used to represent a management subsystem for a Redfish implementation."/>
        </EntityType>

        <Action Name="Reset" IsBound="true">
            <Annotation Term="OData.Description" String="The reset action resets/reboots the manager."/>
            <Annotation Term="OData.LongDescription"
                        String="This defines the name of the custom action supported when used in conjunction with a POST operation to this resource. When issued, this operation will perform a reset of the manager."/>
            <Parameter Name="Manager" Type="Manager.1.0.0.Actions"/>
        </Action>

        <Action Name="Reset" IsBound="true">
            <Parameter Name="Manager" Type="Manager.1.0.0.Actions"/>
            <Parameter Name="ResetType" Type="Resource.ResetType">
                <Annotation Term="OData.Description" String="This is the type of reset to be performed."/>
                <Annotation Term="OData.LongDescription" String="This defines the property name for the action."/>
            </Parameter>
            <Annotation Term="OData.Description" String="This action is used to reset the system."/>
            <Annotation Term="OData.LongDescription"
                        String="This defines the name of the custom action supported on this resource."/>
        </Action>

        <Action Name="ForceFailover" IsBound="true">
            <Annotation Term="OData.Description"
                        String="The ForceFailover action forces a failover of this manager to the manager used in the parameter"/>
            <Annotation Term="OData.LongDescription"
                        String="This defines the name of the custom action supported when used in conjunction with a POST operation to this resource. When issued, this operation will perform a forced failover of the manager's redundancy to the manager supplied as a parameter."/>
            <Parameter Name="Manager" Type="Manager.1.0.0.Actions"/>
            <Parameter Name="NewManager" Type="Manager.Manager" Nullable="false">
                <Annotation Term="OData.Description"
                            String="This Parameter specifies the Manager to fail over to.  In this case, a valid reference is supported."/>
            </Parameter>
        </Action>

        <Action Name="ModifyRedundancySet" IsBound="true">
            <Annotation Term="OData.Description"
                        String="The ModifyRedundancySet operation is used to add or remove members to a redundant group of manager."/>
            <Annotation Term="OData.LongDescription"
                        String="This defines the name of the custom action for which the following properties are supported on this resource.  The ModifyRedundancySet operation shall be used to add or remove members to a redundant group of manager."/>
            <Parameter Name="Manager" Type="Manager.1.0.0.Actions"/>
            <Parameter Name="Add" Type="Collection(Manager.Manager)" Nullable="false">
                <Annotation Term="OData.Description"
                            String="This array defines the Managers to add to the redundancy set.  In this case, a valid reference is supported."/>
            </Parameter>
            <Parameter Name="Remove" Type="Collection(Manager.Manager)" Nullable="false">
                <Annotation Term="OData.Description"
                            String="This array defines the Managers to remove from the redundancy set.  In this case, a valid reference is supported."/>
            </Parameter>
        </Action>

        </Schema>

        <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="Manager.1.0.0">

        <EntityType Name="Manager" BaseType="Manager.Manager">
            <Annotation Term="OData.Description"
                        String="This is the schema definition for a Manager.  Examples of managers are BMCs, Enclosure Managers, Management Controllers and other subsystems assigned managability functions."/>
            <Annotation Term="OData.LongDescription"
                        String="This resource shall be used to represent a management subsystem for a Redfish implementation."/>
            <Property Name="ManagerType" Type="Manager.1.0.0.ManagerType" Nullable="false">
                <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
                <Annotation Term="OData.Description"
                            String="This property represents the type of manager that this resource represents."/>
                <Annotation Term="OData.LongDescription"
                            String="The value of this property shall describe the function of this manager.  The value EnclosureManager shall be used if this manager controls one or more services through aggregation.  The value BMC shall be used if this manager represents a traditional server management controller. The value ManagementController shall be used if none of the other enumerations apply."/>
            </Property>
            <Property Name="Links" Type="Manager.1.0.0.Links" Nullable="false">
                <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
                <Annotation Term="OData.Description"
                            String="This object contains the links to other resources that are related to this resource."/>
            </Property>
            <NavigationProperty Name="EthernetInterfaces" Type="EthernetInterfaceCollection.EthernetInterfaceCollection">
                <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
                <Annotation Term="OData.Description"
                            String="This is a reference to a collection of NICs that this manager uses for network communication.  It is here that clients will find NIC configuration options and settings."/>
                <Annotation Term="OData.LongDescription"
                            String="The value of this property shall be a link to a collection of type EthernetInterfaceCollection."/>
                <Annotation Term="OData.AutoExpandReferences"/>
            </NavigationProperty>
            <NavigationProperty Name="SerialInterfaces" Type="SerialInterfaceCollection.SerialInterfaceCollection">
                <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
                <Annotation Term="OData.Description"
                            String="This is a reference to a collection of serial interfaces that this manager uses for serial and console communication.  It is here that clients will find serial configuration options and settings."/>
                <Annotation Term="OData.LongDescription"
                            String="The value of this property shall be a link to a collection of type SerialInterfaceCollection which are for the use of this manager."/>
                <Annotation Term="OData.AutoExpandReferences"/>
            </NavigationProperty>
            <NavigationProperty Name="NetworkProtocol" Type="ManagerNetworkProtocol.1.0.0.ManagerNetworkProtocol">
                <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
                <Annotation Term="OData.Description"
                            String="This is a reference to the network services and their settings that the manager controls.  It is here that clients will find network configuration options as well as network services."/>
                <Annotation Term="OData.LongDescription"
                            String="The value of this property shall contain a reference to a resource of type ManagerNetworkProtocol which represents the network services for this manager."/>
                <Annotation Term="OData.AutoExpandReferences"/>
                <Annotation Term="OData.AutoExpandReferences"/>
            </NavigationProperty>
            <NavigationProperty Name="LogServices" Type="LogServiceCollection.LogServiceCollection">
                <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
                <Annotation Term="OData.Description" String="This is a reference to a collection of Logs used by the manager."/>
                <Annotation Term="OData.LongDescription"
                            String="The value of this property shall contain a reference to a collection of type LogServiceCollection which are for the use of this manager."/>
                <Annotation Term="OData.AutoExpandReferences"/>
            </NavigationProperty>
            <NavigationProperty Name="VirtualMedia" Type="VirtualMediaCollection.VirtualMediaCollection">
                <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
                <Annotation Term="OData.Description"
                            String="This is a reference to the Virtual Media services for this particular manager."/>
                <Annotation Term="OData.LongDescription"
                            String="The value of this property shall contain a reference to a collection of type VirtualMediaCollection which are for the use of this manager."/>
                <Annotation Term="OData.AutoExpandReferences"/>
            </NavigationProperty>
            <Property Name="ServiceEntryPointUUID" Type="Resource.UUID" Nullable="false">
                <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
                <Annotation Term="OData.Description"
                            String="The UUID of the Redfish Service Entry Point provided by this manager"/>
                <Annotation Term="OData.LongDescription"
                            String="This property shall contain the UUID of the Redfish Service Entry Point provided by this manager.  This property shall not be present if this manager does not provide a Redfish Service Entry Point."/>
            </Property>
            <Property Name="UUID" Type="Resource.UUID">
                <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
                <Annotation Term="OData.Description" String="The Universal Unique Identifier (UUID) for this Manager"/>
                <Annotation Term="OData.LongDescription"
                            String="The value of this property shall contain the universal unique identifier number for the manager."/>
            </Property>
            <Property Name="Model" Type="Edm.String">
                <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
                <Annotation Term="OData.Description"
                            String="The model information of this Manager as defined by the manufacturer"/>
                <Annotation Term="OData.LongDescription"
                            String="The value of this property shall contain the information about how the manufacturer references this manager."/>
            </Property>
            <Property Name="DateTime" Type="Edm.DateTimeOffset">
                <Annotation Term="OData.Permissions" EnumMember="OData.Permission/ReadWrite"/>
                <Annotation Term="OData.Description"
                            String="The current DateTime (with offset) for the manager, used to set or read time."/>
                <Annotation Term="OData.LongDescription"
                            String="The value of this property shall represent the current DateTime value for the manager, with offset from UTC, in Redfish Timestamp format."/>
            </Property>
            <Property Name="DateTimeLocalOffset" Type="Edm.String">
                <Annotation Term="OData.Permissions" EnumMember="OData.Permission/ReadWrite"/>
                <Annotation Term="OData.Description"
                            String="The time offset from UTC that the DateTime property is set to in format: +06:00 ."/>
                <Annotation Term="OData.LongDescription"
                            String="The value is property shall represent the offset from UTC time that the current value of DataTime property contains."/>
                <Annotation Term="Validation.Pattern" String="([-+][0-1][0-9]:[0-5][0-9])"/>
            </Property>
            <Property Name="FirmwareVersion" Type="Edm.String">
                <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
                <Annotation Term="OData.Description" String="The firmware version of this Manager"/>
                <Annotation Term="OData.LongDescription"
                            String="This property shall contain the firwmare version as defined by the manufacturer for the associated manager."/>
            </Property>
            <Property Name="SerialConsole" Type="Manager.1.0.0.SerialConsole" Nullable="false">
                <Annotation Term="OData.Description"
                            String="Information about the Serial Console service provided by this manager."/>
                <Annotation Term="OData.LongDescription"
                            String="The value of this property shall contain information about the Serial Console service of this manager."/>
            </Property>
            <Property Name="CommandShell" Type="Manager.1.0.0.CommandShell">
                <Annotation Term="OData.Description"
                            String="Information about the Command Shell service provided by this manager."/>
                <Annotation Term="OData.LongDescription"
                            String="The value of this property shall contain information about the Command Shell service of this manager."/>
            </Property>
            <Property Name="GraphicalConsole" Type="Manager.1.0.0.GraphicalConsole">
                <Annotation Term="OData.Description"
                            String="The value of this property shall contain the information about the Graphical Console (KVM-IP) service of this manager."/>
                <Annotation Term="OData.LongDescription"
                            String="The value of this property shall contain the information about the Graphical Console (KVM-IP) service of this manager."/>
            </Property>
            <Property Name="Actions" Type="Manager.1.0.0.Actions" Nullable="false">
                <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
                <Annotation Term="OData.Description"
                            String="The Actions object contains the available custom actions on this resource."/>
            </Property>
            <Property Name="Status" Type="Resource.Status"/>
            <NavigationProperty Name="Redundancy" Type="Collection(Redundancy.Redundancy)">
                <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
                <Annotation Term="OData.Description" String="Redundancy information for the managers of this system"/>
                <Annotation Term="OData.AutoExpand"/>
            </NavigationProperty>
        </EntityType>

        <ComplexType Name="Actions">
            <Annotation Term="OData.AdditionalProperties" Bool="False"/>
            <Property Name="Oem" Type="Manager.1.0.0.OemActions"/>
        </ComplexType>

        <ComplexType Name="OemActions">
            <Annotation Term="OData.AdditionalProperties" Bool="True"/>
        </ComplexType>

        <ComplexType Name="Links" BaseType="Resource.Links">
            <NavigationProperty Name="ManagerForServers" Type="Collection(ComputerSystem.ComputerSystem)">
                <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
                <Annotation Term="OData.Description"
                            String="This property is an array of references to the systems that this manager has control over."/>
                <Annotation Term="OData.LongDescription"
                            String="This is a reference to a collection of ComputerSystem resources."/>
                <Annotation Term="OData.AutoExpandReferences"/>
            </NavigationProperty>
            <NavigationProperty Name="ManagerForChassis" Type="Collection(Chassis.Chassis)">
                <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
                <Annotation Term="OData.Description"
                            String="This property is an array of references to the chasis that this manager has control over."/>
                <Annotation Term="OData.LongDescription"
                            String="This is a reference to a collection of ListenerDestination resources."/>
                <Annotation Term="OData.AutoExpandReferences"/>
            </NavigationProperty>
        </ComplexType>

        <!-- This type defines the type of manager that given resource represents. -->
        <EnumType Name="ManagerType">
            <Member Name="ManagementController">
                <Annotation Term="OData.Description"
                            String="A controller used primarily to monitor or manage the operation of a device or system"/>
            </Member>
            <Member Name="EnclosureManager">
                <Annotation Term="OData.Description"
                            String="A controller which provides management functions for a chassis or group of devices or systems"/>
            </Member>
            <Member Name="BMC">
                <Annotation Term="OData.Description"
                            String="A controller which provides management functions for a single computer system"/>
            </Member>
            <Member Name="RackManager">
                <Annotation Term="OData.Description"
                            String="A controller which provides management functions for a whole or part of a rack"/>
            </Member>
            <Member Name="AuxiliaryController">
                <Annotation Term="OData.Description"
                            String="A controller which provides management functions for a particular subsystem or group of devices"/>
            </Member>
        </EnumType>

        <ComplexType Name="ManagerService" Abstract="true">
            <Annotation Term="OData.Description"
                        String="Used for describing services like Serial Console, Command Shell or Graphical Console"/>
            <Annotation Term="OData.AdditionalProperties" Bool="False"/>
            <Property Name="ServiceEnabled" Type="Edm.Boolean" Nullable="false">
                <Annotation Term="OData.Description" String="Indicates if the service is enabled for this manager."/>
                <Annotation Term="OData.LongDescription"
                            String="The value of this property shall contain the enabled status of the protocol used for the service.  The value shall be true if enabled and false if disabled."/>
            </Property>
            <Property Name="MaxConcurrentSessions" Type="Edm.Int64" Nullable="false">
                <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
                <Annotation Term="OData.Description"
                            String="Indicates the maximum number of service sessions, regardless of protocol, this manager is able to support."/>
                <Annotation Term="OData.LongDescription"
                            String="The value of this property shall contain the maximum number of concurrent service sessions supported by the implementation."/>
                <Annotation Term="Validation.Minimum" Int="0"/>
            </Property>
        </ComplexType>

        <!-- This schema defines Serial Console connection types allowed by the implementation. -->
        <EnumType Name="SerialConnectTypesSupported">
            <Member Name="SSH">
                <Annotation Term="OData.Description"
                            String="The controller supports a Serial Console connection using the SSH protocol"/>
            </Member>
            <Member Name="Telnet">
                <Annotation Term="OData.Description"
                            String="The controller supports a Serial Console connection using the Telnet protocol"/>
            </Member>
            <Member Name="IPMI">
                <Annotation Term="OData.Description"
                            String="The controller supports a Serial Console connection using the IPMI Serial-over-LAN (SOL) protocol"/>
            </Member>
            <Member Name="Oem">
                <Annotation Term="OData.Description"
                            String="The controller supports a Serial Console connection using an OEM-specific protocol"/>
            </Member>
        </EnumType>

        <!-- Need other enumerations because enumeration descriptions differ by type -->
        <EnumType Name="CommandConnectTypesSupported">
            <Member Name="SSH">
                <Annotation Term="OData.Description"
                            String="The controller supports a Command Shell connection using the SSH protocol"/>
            </Member>
            <Member Name="Telnet">
                <Annotation Term="OData.Description"
                            String="The controller supports a Command Shell connection using the SSH protocol"/>
            </Member>
            <Member Name="IPMI">
                <Annotation Term="OData.Description"
                            String="The controller supports a Command Shell connection using the SSH protocol"/>
            </Member>
            <Member Name="Oem">
                <Annotation Term="OData.Description"
                            String="The controller supports a Command Shell connection using an OEM-specific protocol"/>
            </Member>
        </EnumType>

        <EnumType Name="GraphicalConnectTypesSupported">
            <Member Name="KVMIP">
                <Annotation Term="OData.Description"
                            String="The controller supports a Graphical Console connection using a KVM-IP (redirection of Keyboard, Video, Mouse over IP) protocol"/>
            </Member>
            <Member Name="Oem">
                <Annotation Term="OData.Description"
                            String="The controller supports a Graphical Console connection using an OEM-specific protocol"/>
            </Member>
        </EnumType>

        <!-- This type contains information about the Serial Console service of a given manager. -->
        <ComplexType Name="SerialConsole" BaseType="Manager.1.0.0.ManagerService">
            <Annotation Term="OData.AdditionalProperties" Bool="False"/>
            <Property Name="ConnectTypesSupported" Type="Collection(Manager.1.0.0.SerialConnectTypesSupported)"
                      Nullable="false">
                <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
                <Annotation Term="OData.Description"
                            String="This object is used to enumerate the Serial Console connection types allowed by the implementation."/>
                <Annotation Term="OData.LongDescription"
                            String="The value of ConnectTypesSupported shall be an array of the enumerations provided here.  SSH shall be included if the Secure Shell (SSH) protocol is supported.  Telnet shall be included if the Telnet protocol is supported.  IPMI shall be included if the IPMI (Serial-over-LAN) protocol is supported."/>
            </Property>
        </ComplexType>

        <!-- This type contains information about the Command Shell service of a given manager. -->
        <ComplexType Name="CommandShell" BaseType="Manager.1.0.0.ManagerService">
            <Annotation Term="OData.AdditionalProperties" Bool="False"/>
            <Property Name="ConnectTypesSupported" Type="Collection(Manager.1.0.0.CommandConnectTypesSupported)"
                      Nullable="false">
                <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
                <Annotation Term="OData.Description"
                            String="This object is used to enumerate the Command Shell connection types allowed by the implementation."/>
                <Annotation Term="OData.LongDescription"
                            String="The value of ConnectTypesSupported shall be an array of the enumerations provided here.  SSH shall be included if the Secure Shell (SSH) protocol is supported.  Telnet shall be included if the Telnet protocol is supported.  IPMI shall be included if the IPMI (Serial-over-LAN) protocol is supported."/>
            </Property>
        </ComplexType>

        <!-- This type contains information about the Graphical Console (KVM-IP) service of a given manager. -->
        <ComplexType Name="GraphicalConsole" BaseType="Manager.1.0.0.ManagerService">
            <Annotation Term="OData.AdditionalProperties" Bool="False"/>
            <Property Name="ConnectTypesSupported" Type="Collection(Manager.1.0.0.GraphicalConnectTypesSupported)"
                      Nullable="false">
                <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
                <Annotation Term="OData.Description"
                            String="This object is used to enumerate the Graphical Console connection types allowed by the implementation."/>
                <Annotation Term="OData.LongDescription"
                            String="The value of ConnectTypesSupported shall be an array of the enumerations provided here.  RDP shall be included if the Remote Desktop (RDP) protocol is supported.  KVMIP shall be included if a vendor-define KVM-IP protocol is supported."/>
            </Property>
        </ComplexType>

    </Schema>
  </Schemas>
       """

RESOURCE = \
        {
                "@odata.type": "#Manager.1.0.0.Manager",
                "Status": {
                    "State": "Enabled",
                    "Health": "OK"
                },
                "EthernetInterfaces": {
                    "@odata.id": "/redfish/v1/Managers/RMC/EthernetInterfaces"
                },
                "Name": "Manager",
                "ManagerType": "RackManager",
                "Links": {
                    "ManagerForChassis": [
                        {
                            "@odata.id": "/redfish/v1/Chassis/Rack"
                        }
                    ],
                    "ManagerForChassis@odata.count": 1
                },
                "@odata.id": "/redfish/v1/Managers/RMC",
                "@odata.context": "/redfish/v1/$metadata#Managers/Members/$entity",
                "DateTime": "2016-11-16T18:41:14+02:00",
                "NetworkProtocol": {
                    "@odata.id": "/redfish/v1/Managers/RMC/NetworkProtocol"
                },
                "Actions": {
                    "#Manager.Reset": {
                        "target": "/redfish/v1/Managers/RMC/Actions/Manager.Reset"
                    },
                    "Oem": {
                        "Intel_RackScale": {
                            "#RMC.Update": {
                                "ComponentType@AllowableValues": [
                                    "rmm-base"
                                ],
                                "target": "/redfish/v1/Managers/RMC/Actions/Oem/RMC.Update",
                                "Image@AllowableValues": []
                            },
                            "#RMC.LoadFactoryDefault": {
                                "target": "/redfish/v1/Managers/RMC/Actions/Oem/RMC.LoadFactoryDefault"
                            }
                        }
                    }
                },
                "DateTimeLocalOffset": "+02:00",
                "Description": "RackScale RMC",
                "Model": "RackScale RMC 1.0",
                "ServiceEntryPointUUID": "7ac37ef0-ac09-11e6-bc77-1b32fb6a90e1",
                "Id": "1",
                "FirmwareVersion": "2.1.13.0",
                "UUID": "7ac52bec-ac09-11e6-9d27-db5afe21eb9a"
            }


class PatchRmmManagerTest(unittest.TestCase):
    def setUp(self):
        metadata_manager = MetadataManager(["qualifier"], )
        self.metadata_container = metadata_manager.read_metadata_from_strings("Unknown", METADATA)
        self.discovery_container = DiscoveryContainer()
        self.discovery_container.add_resource(ApiResource("/redfish/v1/Managers/RMC",
                                                          'netloc', RESOURCE,
                                                          "#Manager.1.0.0.Manager"))

    def test_patch(self):
        with mock.patch('cts_core.commons.api_caller.ApiCaller.__init__') as api_caller_init_mock:
            api_caller_init_mock.return_value = None

            with mock.patch('cts_core.validation.patch.metadata_patch_validator.MetadataPatchValidator._validate_patchable_property') as validate_property:
                validate_property.return_value = ValidationStatus.PASSED

                validator = MetadataPatchValidator(self.metadata_container, None, PatchingStrategy2_1())

                with StdoutCapture() as output:
                    self.assertEqual(ValidationStatus.PASSED, validator.validate(self.discovery_container))


