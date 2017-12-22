import os
import unittest

from mock import mock, MagicMock

from cts_core.commons.links_factory import Link
from cts_core.discovery.api_resource import ApiResource
from cts_core.discovery.discovery_container import DiscoveryContainer
from cts_core.metadata.metadata_manager import MetadataManager
from cts_core.validation.patch.metadata_patch_validator import MetadataPatchValidator
from cts_core.validation.patch.patching_strategy_2_2 import PatchingStrategy2_2
from cts_core.validation.validation_status import ValidationStatus
from cts_framework.commons.enums import RequestStatus
from unit_tests.helpers.stdout_capture import StdoutCapture


class AdditionalPropertiesPatchTest(unittest.TestCase):
    METADATA = """
        <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="MyNamespace.v1_0_0">
          <EntityType Name="MyEntity">
            <Annotation Term="OData.AdditionalProperties" Bool="true"/>
            <Property Name="Outer" Nullable="false" Type="OuterComplexType"/>
          </EntityType>
          <ComplexType Name="OuterComplexType">
            <Annotation Term="OData.AdditionalProperties" Bool="true"/>
            <Property Name="Name" Nullable="false" Type="Edm.String"/>
          </ComplexType>
          <ComplexType Name="InnerComplexType">
            <Annotation Term="OData.AdditionalProperties" Bool="false"/>
            <Property Name="Name" Nullable="false" Type="Edm.String">
                <Annotation Term="OData.Permissions" EnumMember="OData.Permission/ReadWrite"/>
                <Annotation Term="IntelRackScale.Nontrivial"
                            String="Ability to write this property value is implementation specific."
            </Property>
            <Property Name="NotRequired" type="Edm.String">
                <Annotation Term="OData.Permissions" EnumMember="OData.Permission/ReadWrite"/>
                <Annotation Term="IntelRackScale.Nontrivial"
                            String="Ability to write this property value is implementation specific."
            </Property>
          </ComplexType>
        </Schema>

        <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="IntelRackScale">
          <Term Name="Nontrivial" Type="Edm.String" AppliesTo="Property">
            <Annotation Term="OData.Description"
                        String="The term shall be applied to a property in order to specify nontrivial
                                preconditions, postconditions, side-effects or invariants that need to be satisfied."/>
          </Term>
        </Schema>
    """

    def setUp(self):
        self.discovery_container = DiscoveryContainer()



    def test_should_not_patch_if_property_annotation_is_non_trivial_and_alternative_implementation_not_known(self):
        """
        when additional property of type 'ComplexType' does have annotation Term Nontriavial should not be patched
        """

        metadata_manager = MetadataManager(["qualifier"])
        metadata_container = metadata_manager.read_metadata_from_strings(AdditionalPropertiesPatchTest.METADATA)


        entity_json = {
            "@odata.type": "MyNamespace.v1_0_0.MyEntity",
            "Outer": {
                "@odata.type": "MyNamespace.v1_0_0.OuterComplexType",
                "Name": "OuterName",
                "InnerToValidate": {
                    "@odata.type": "MyNamespace.v1_0_0.InnerComplexType",
                    "Name": "This name property is required"
                }
            }
        }

        self.discovery_container.add_resource(ApiResource("/redfish/v1/Managers/RMC", "netloc",
                                                          entity_json, "#MyNamespace.v1_0_0.MyEntity"))

        with mock.patch('cts_core.commons.api_caller.ApiCaller.__init__') as api_caller_init_mock:
            api_caller_init_mock.return_value = None
            validator = MetadataPatchValidator(metadata_container, None, PatchingStrategy2_2())

            # this ValidationStatus.BLOCKED should be not affect final ValidationStatus
            validator._verify_property = MagicMock(return_value=(ValidationStatus.BLOCKED, None))
            validator._restore_property = MagicMock(return_value=ValidationStatus.BLOCKED)
            validator._patch_property = MagicMock(return_value=(True, True, ValidationStatus.BLOCKED))

            with StdoutCapture() as out:
                self.assertEqual(ValidationStatus.PASSED_WITH_WARNINGS, validator.validate(self.discovery_container))

            self.assertIn('WARNING::Skipping non-trivial property Name;', out.raw)

    def test_patch_nontrivial_boot_property(self):
        metadata_manager = MetadataManager(["qualifier"])
        this_dir = os.path.dirname(os.path.realpath(__file__))
        metadata_dir = os.path.join(this_dir, 'metadata')
        self.assertTrue(metadata_manager.read_metadata_from_dir(metadata_dir))
        metadata_container = metadata_manager.metadata_container

        entity_json ={
            "@odata.context": "/redfish/v1/$metadata#ComputerSystem.ComputerSystem",
            "@odata.id": "/redfish/v1/Systems/34",
            "@odata.type": "#ComputerSystem.v1_3_0.ComputerSystem",
            "Id": "34",
            "Name": "Computer System",
            "Description": "Computer System description",
            "SystemType": "Physical",
            "AssetTag": "AZaz09[]!@#$%^&*()_+",
            "Manufacturer": None,
            "Model": None,
            "SKU": None,
            "SerialNumber": None,
            "PartNumber": None,
            "UUID": "1d7a1040-05e4-11e6-a108-54ab3a8ec80f",
            "HostName": None,
            "Status": {
                "State": "Enabled",
                "Health": "OK",
                "HealthRollup": "OK"
            },
            "IndicatorLED": None,
            "PowerState": "Off",
            "BiosVersion": "F20A3A03",
            "Boot": {
                "@odata.type": "#ComputerSystem.v1_1_0.Boot",
                "BootSourceOverrideEnabled": "Disabled",
                "BootSourceOverrideTarget": "None",
                "BootSourceOverrideTarget@Redfish.AllowableValues": [
                    "Hdd",
                    "Pxe",
                    "None",
                    "RemoteDrive"
                ],
                "BootSourceOverrideMode": "Legacy",
                "BootSourceOverrideMode@Redfish.AllowableValues": [
                    "Legacy",
                    "UEFI"
                ]
            },
            "ProcessorSummary": {
                "Count": 2,
                "Model": "Intel(R) Xeon(R)",
                "Status": {
                    "State": "Enabled",
                    "Health": "OK",
                    "HealthRollup": "OK"
                }
            },
            "MemorySummary": {
                "TotalSystemMemoryGiB": 30.52,
                "Status": {
                    "State": "Enabled",
                    "Health": "OK",
                    "HealthRollup": "OK"
                }
            },
            "Processors": {
                "@odata.id": "/redfish/v1/Systems/34/Processors"
            },
            "EthernetInterfaces": {
                "@odata.id": "/redfish/v1/Systems/34/EthernetInterfaces"
            },
            "NetworkInterfaces": {
                "@odata.id": "/redfish/v1/Systems/34/NetworkInterfaces"
            },
            "Storage": {
                "@odata.id": "/redfish/v1/Systems/34/Storage"
            },
            "Memory": {
                "@odata.id": "/redfish/v1/Systems/34/Memory"
            },
            "PCIeDevices": [],
            "PCIeFunctions": [],
            "TrustedModules": [],
            "Links": {
                "@odata.type": "#ComputerSystem.v1_2_0.Links",
                "Chassis": [
                    {
                        "@odata.id": "/redfish/v1/Chassis/38"
                    }
                ],
                "Endpoints": [],
                "ManagedBy": [
                    {
                        "@odata.id": "/redfish/v1/Managers/38"
                    }
                ],
                "Oem": {}
            },
            "Actions": {
                "#ComputerSystem.Reset": {
                    "target": "/redfish/v1/Systems/34/Actions/ComputerSystem.Reset",
                    "ResetType@Redfish.AllowableValues": [
                        "On",
                        "ForceOff",
                        "GracefulShutdown",
                        "GracefulRestart",
                        "ForceRestart"
                    ]
                },
                "Oem": {
                    "#Intel.Oem.ChangeTPMState": {
                        "target": "/redfish/v1/Systems/34/Actions/Oem/Intel.Oem.ChangeTPMState",
                        "InterfaceType@Redfish.AllowableValues": []
                    }
                }
            },
            "Oem": {
                "Intel_RackScale": {
                    "@odata.type": "#Intel.Oem.ComputerSystem",
                    "PciDevices": [],
                    "PCIeConnectionId": [],
                    "ProcessorSockets": 2,
                    "MemorySockets": 16,
                    "DiscoveryState": "Basic",
                    "UserModeEnabled": None,
                    "TrustedExecutionTechnologyEnabled": None,
                    "Metrics": {
                        "@odata.id": "/redfish/v1/Systems/34/Metrics"
                    }
                }
            }
        }

        self.discovery_container.add_resource(ApiResource("/redfish/v1/Systems/34", "https://localhost:8443",
                                                          entity_json, "#ComputerSystem.v1_3_0.ComputerSystem"))

        with mock.patch('cts_core.commons.api_caller.ApiCaller.__init__') as api_caller_init_mock:
            api_caller_init_mock.return_value = None
            validator = MetadataPatchValidator(metadata_container, None, PatchingStrategy2_2())

            # this ValidationStatus.BLOCKED should be not affect final ValidationStatus
            validator._verify_property = MagicMock(return_value=(ValidationStatus.PASSED, None))
            validator._restore_property = MagicMock(return_value=ValidationStatus.PASSED)
            validator._patch_property = MagicMock(return_value=(True, True, ValidationStatus.PASSED))

            with mock.patch('cts_core.commons.api_caller.ApiCaller.patch_resource') as api_caller_patch_resource:
                api_caller_patch_resource.return_value = (RequestStatus.SUCCESS, 200, None, None)
                with mock.patch('cts_core.commons.api_caller.ApiCaller.get_resource') as api_caller_get_resource:
                    api_caller_get_resource.return_value = (Link("/redfish/v1/Systems/34", "https://localhost:8443"),
                                                            RequestStatus.SUCCESS,
                                                            200,
                                                            entity_json,
                                                            None)

                    with StdoutCapture() as out:
                        self.assertEqual(ValidationStatus.PASSED, validator.validate(self.discovery_container))

                    self.assertNotIn('WARNING::Skipping non-trivial property', out.raw)

    def test_patch_nontrivial_roles(self):
        metadata_manager = MetadataManager(["qualifier"])
        this_dir = os.path.dirname(os.path.realpath(__file__))
        metadata_dir = os.path.join(this_dir, 'metadata')
        self.assertTrue(metadata_manager.read_metadata_from_dir(metadata_dir))
        metadata_container = metadata_manager.metadata_container

        role_administrator_json = {
            "@odata.type": "#Role.v1_0_0.Role",
            "Description": "Administrator User Role",
            "OemPrivileges": [
                "OemClearLog",
                "OemPowerControl"
            ],
            "@odata.id": "/redfish/v1/AccountService/Roles/Administrator",
            "@odata.context": "/redfish/v1/$metadata#AccountService/Roles/Administrator/$entity",
            "AssignedPrivileges": [
                "Login",
                "ConfigureManager",
                "ConfigureUsers",
                "ConfigureSelf",
                "ConfigureComponents"
            ],
            "Id": "Administrator",
            "IsPredefined": True,
            "Name": "User Role"
        }

        role_operator_json = {
            "@odata.type": "#Role.v1_0_0.Role",
            "Description": "Operator User Role",
            "OemPrivileges": [],
            "@odata.id": "/redfish/v1/AccountService/Roles/Operator",
            "@odata.context": "/redfish/v1/$metadata#AccountService/Roles/Operator/$entity",
            "AssignedPrivileges": [
                "Login",
                "ConfigureSelf",
                "ConfigureComponents"
            ],
            "Id": "Operator",
            "IsPredefined": True,
            "Name": "User Role"
        }

        role_readonly_json = {
            "@odata.type": "#Role.v1_0_0.Role",
            "Description": "ReadOnly User Role",
            "OemPrivileges": [],
            "@odata.id": "/redfish/v1/AccountService/Roles/ReadOnly",
            "@odata.context": "/redfish/v1/$metadata#AccountService/Roles/ReadOnly/$entity",
            "AssignedPrivileges": [
                "Login",
                "ConfigureSelf"
            ],
            "Id": "ReadOnly",
            "IsPredefined": True,
            "Name": "User Role"
        }

        role_custom_json = {
            "@odata.type": "#Role.v1_0_0.Role",
            "Description": "Custom User Role",
            "OemPrivileges": [],
            "@odata.id": "/redfish/v1/AccountService/Roles/Custom",
            "@odata.context": "/redfish/v1/$metadata#AccountService/Roles/ReadOnly/$entity",
            "AssignedPrivileges": [
                "Login",
                "ConfigureSelf"
            ],
            "Id": "ReadOnly",
            "IsPredefined": False,
            "Name": "Custom Role"
        }

        self.discovery_container.add_resource(ApiResource("/redfish/v1/AccountService/Roles/Administrator",
                                                          "https://localhost:8443",
                                                          role_administrator_json, "#Role.v1_0_0.Role"))

        self.discovery_container.add_resource(ApiResource("/redfish/v1/AccountService/Roles/Operator",
                                                          "https://localhost:8443",
                                                          role_operator_json, "#Role.v1_0_0.Role"))

        self.discovery_container.add_resource(ApiResource("/redfish/v1/AccountService/Roles/ReadOnly",
                                                          "https://localhost:8443",
                                                          role_readonly_json, "#Role.v1_0_0.Role"))

        self.discovery_container.add_resource(ApiResource("/redfish/v1/AccountService/Roles/Custom",
                                                          "https://localhost:8443",
                                                          role_custom_json, "#Role.v1_0_0.Custom"))

        with mock.patch('cts_core.commons.api_caller.ApiCaller.__init__') as api_caller_init_mock:
            api_caller_init_mock.return_value = None
            validator = MetadataPatchValidator(metadata_container, None, PatchingStrategy2_2())

            # this ValidationStatus.BLOCKED should be not affect final ValidationStatus
            validator._verify_property = MagicMock(return_value=(ValidationStatus.PASSED, None))
            validator._restore_property = MagicMock(return_value=ValidationStatus.PASSED)
            validator._patch_property = MagicMock(return_value=(True, True, ValidationStatus.PASSED))

            with mock.patch('cts_core.validation.patch.metadata_patch_validator.MetadataPatchValidator'
                            '._validate_resource') as validate_resource:
                with StdoutCapture() as out:
                    self.assertEqual(ValidationStatus.PASSED, validator.validate(self.discovery_container))

            self.assertEquals(1, validate_resource.call_count)  # only for 'custom' role
