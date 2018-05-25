import unittest
from cts_core.discovery.discovery_container import DiscoveryContainer
from cts_core.metadata.metadata_manager import MetadataManager
from cts_core.validation.hierarchy.hierarchy_validator import HierarchyValidator
from cts_core.metadata.metadata_constants import MetadataConstants2_1
from unit_tests.helpers.resource_builder import ResourceBuilder
from cts_core.validation.validation_status import ValidationStatus
from unit_tests.helpers.stdout_capture import StdoutCapture

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
          </Schemas>
       """


class HierarchyValidatorTest(unittest.TestCase):
    def setUp(self):
        metadata_manager = MetadataManager(["qualifier"])
        self.metadata_container = metadata_manager.read_metadata_from_strings("Unknown", METADATA)
        self.discovery_container = DiscoveryContainer(metadata_container=self.metadata_container)
        self.validator = HierarchyValidator(self.discovery_container, self.metadata_container, MetadataConstants2_1)
        self.builder = ResourceBuilder(self.discovery_container)

    def test_invalid_hierarchy_without_root_resource(self):
        self.builder.build("/redfish/v1/Chassis/Rack",
                           "#Chassis.v1_2_0.Chassis",
                           "/redfish/v1/$metadata#Chassis/Members/$entity",
                           ChassisType='Rack'
                           ).with_location(id='Rack', parent_id='SomeNode').commit()

        with StdoutCapture() as output:
            self.assertEqual(ValidationStatus.FAILED, self.validator.validate_path_using_contains_property())

        self.assertIn('but none has NULL ParentId', '\n'.join(output))

    def test_hierarchy_ok_when_one_node_with_none_parent_id(self):
        self.builder.build("/redfish/v1/Chassis/Rack",
                           "#Chassis.v1_2_0.Chassis",
                           "/redfish/v1/$metadata#Chassis/Members/$entity",
                           ChassisType='Rack'
                           ).with_location(id='Rack', parent_id=None).commit()
        self.assertEqual(ValidationStatus.PASSED, self.validator.validate_path_using_contains_property())

    def test_failed_when_more_than_one_root(self):
        chassis_1 = self.builder.build("/redfish/v1/Chassis/Rack",
                                       "#Chassis.v1_2_0.Chassis",
                                       "/redfish/v1/$metadata#Chassis/Members/$entity",
                                       ChassisType='Rack'
                                       ).with_location(id='Rack', parent_id=None).commit()
        self.builder.clone("/redfish/v1/Chassis/Rack2").commit()

        with StdoutCapture() as output:
            self.assertEqual(ValidationStatus.FAILED, self.validator.validate_path_using_contains_property())

        self.assertIn('More than one root resource (with NULL ParentId). Expected only one', '\n'.join(output))

    def test_failed_when_parent_does_not_exist(self):
        chassis_1 = self.builder.build("/redfish/v1/Chassis/Rack",
                                       "#Chassis.v1_2_0.Chassis",
                                       "/redfish/v1/$metadata#Chassis/Members/$entity",
                                       ChassisType='Rack'
                                       ).with_location(id='Rack', parent_id=None).commit()
        chassis_1.clone("/redfish/v1/Chassis/Drawer",
                        ChassisType='Drawer').with_location(id='Drawer', parent_id='ParentDoesNotExist').commit()

        with StdoutCapture() as output:
            self.assertEqual(ValidationStatus.FAILED, self.validator.validate_path_using_contains_property())

        self.assertIn('Not all location-aware resources visited while traversing tree using \'Link->Contain', '\n'.join(output))

    def test_failed_when_resource_has_no_location_but_subresource_has_parent_location_set(self):
        parent = self.builder.build("/redfish/v1/Chassis/Rack",
                                       "#Chassis.v1_2_0.Chassis",
                                       "/redfish/v1/$metadata#Chassis/Members/$entity",
                                       ChassisType='Rack'
                                       ).contains("/redfish/v1/Chassis/Drawer").commit()

        child = self.builder.build("/redfish/v1/Chassis/Drawer",
                                       "#Chassis.v1_2_0.Chassis",
                                       "/redfish/v1/$metadata#Chassis/Members/$entity",
                                       ChassisType='Rack'
                                       ).with_location(id='Chassis', parent_id='Rack')\
                                        .contained_by("/redfish/v1/Chassis/Rack").commit()

        with StdoutCapture() as output:
            self.assertEqual(ValidationStatus.FAILED, self.validator.validate_path_using_contains_property())

        self.assertIn('Resources with Location in Oem property has been found, but none has NULL ParentId', '\n'.join(output))

    def test_hierarchy_ok_when_correct_contains_and_contained_by(self):
        parent = self.builder.build("/redfish/v1/Chassis/Rack",
                                       "#Chassis.v1_2_0.Chassis",
                                       "/redfish/v1/$metadata#Chassis/Members/$entity",
                                       ChassisType='Rack'
                                       ).with_location(id='Rack', parent_id=None)\
                                        .contains("/redfish/v1/Chassis/Drawer").commit()

        child = self.builder.build("/redfish/v1/Chassis/Drawer",
                                       "#Chassis.v1_2_0.Chassis",
                                       "/redfish/v1/$metadata#Chassis/Members/$entity",
                                       ChassisType='Rack'
                                       ).with_location(id='Chassis', parent_id='Rack')\
                                        .contained_by("/redfish/v1/Chassis/Rack").commit()

        self.assertEqual(ValidationStatus.PASSED, self.validator.validate_path_using_contains_property())


    def test_failed_when_invalid_contained_by(self):
        parent = self.builder.build("/redfish/v1/Chassis/Rack",
                                    "#Chassis.v1_2_0.Chassis",
                                    "/redfish/v1/$metadata#Chassis/Members/$entity",
                                    ChassisType='Rack'
                                    ).with_location(id='Rack', parent_id=None) \
                                     .contains("/redfish/v1/Chassis/Drawer").commit()

        child = self.builder.build("/redfish/v1/Chassis/Drawer",
                                   "#Chassis.v1_2_0.Chassis",
                                   "/redfish/v1/$metadata#Chassis/Members/$entity",
                                   ChassisType='Rack'
                                   ).with_location(id='Chassis', parent_id='Rack') \
                                    .contained_by("InvalidContainedBy").commit()

        with StdoutCapture() as output:
            self.assertEqual(ValidationStatus.FAILED, self.validator.validate_path_using_contains_property())

        self.assertIn('but it\'s parent is url=/redfish/v1/Chassis/Rack', '\n'.join(output))


    def test_failed_when_contained_does_not_exist(self):
        self.builder.build("/redfish/v1/Chassis/Rack",
                           "#Chassis.v1_2_0.Chassis",
                           "/redfish/v1/$metadata#Chassis/Members/$entity",
                           ChassisType='Rack'
                           ).with_location(id='Rack', parent_id=None) \
                            .contains("/redfish/v1/Chassis/Drawer").commit()

        with StdoutCapture() as output:
            self.assertEqual(ValidationStatus.FAILED, self.validator.validate_path_using_contains_property())

        self.assertIn('contains reference to nonexistent resource url=/redfish/v1/Chassis/Drawer',
                      '\n'.join(output))

    def test_failed_when_circular_dependency(self):
        parent = self.builder.build("/redfish/v1/Chassis/Rack",
                                    "#Chassis.v1_2_0.Chassis",
                                    "/redfish/v1/$metadata#Chassis/Members/$entity",
                                    ChassisType='Rack'
                                    ).with_location(id='Rack', parent_id=None) \
            .contains("/redfish/v1/Chassis/Drawer") \
            .contained_by("/redfish/v1/Chassis/Drawer") \
            .commit()

        child = self.builder.build("/redfish/v1/Chassis/Drawer",
                                   "#Chassis.v1_2_0.Chassis",
                                   "/redfish/v1/$metadata#Chassis/Members/$entity",
                                   ChassisType='Rack'
                                   ).with_location(id='Chassis', parent_id='Rack') \
            .contained_by("/redfish/v1/Chassis/Rack") \
            .contains("/redfish/v1/Chassis/Rack") \
            .commit()

        with StdoutCapture() as output:
            self.assertEqual(ValidationStatus.FAILED, self.validator.validate_path_using_contains_property())

        self.assertIn('but child resource has invalid Location->ParentId. Is: None. Expected: Chassis', '\n'.join(output))


    def test_failed_when_id_not_unique_within_scope(self):
        parent = self.builder.build("/redfish/v1/Chassis/Rack",
                                    "#Chassis.v1_2_0.Chassis",
                                    "/redfish/v1/$metadata#Chassis/Members/$entity",
                                    ChassisType='Rack'
                                    ).with_location(id='Rack', parent_id=None) \
            .contains("/redfish/v1/Chassis/Drawer1", "/redfish/v1/Chassis/Drawer2") \
            .commit()

        child1 = self.builder.build("/redfish/v1/Chassis/Drawer1",
                                    "#Chassis.v1_2_0.Chassis",
                                    "/redfish/v1/$metadata#Chassis/Members/$entity",
                                    ChassisType='Rack'
                                    )\
            .with_location(id='LocationA', parent_id='Rack') \
            .contained_by("/redfish/v1/Chassis/Rack") \
            .commit()

        child2 = self.builder.build("/redfish/v1/Chassis/Drawer2",
                                    "#Chassis.v1_2_0.Chassis",
                                    "/redfish/v1/$metadata#Chassis/Members/$entity",
                                    ChassisType='Rack'
                                    )\
            .with_location(id='LocationA', parent_id='Rack') \
            .contained_by("/redfish/v1/Chassis/Rack") \
            .commit()

        with StdoutCapture() as output:
            self.assertEqual(ValidationStatus.FAILED, self.validator.validate_path_using_contains_property())

        self.assertIn('has location id LocationA that conflicts with resources', '\n'.join(output))

    def test_failed_when_some_resources_have_location_some_not(self):
        parent = self.builder.build("/redfish/v1/Chassis/Rack",
                                    "#Chassis.v1_2_0.Chassis",
                                    "/redfish/v1/$metadata#Chassis/Members/$entity",
                                    ChassisType='Rack'
                                    ) \
            .contains("/redfish/v1/Chassis/Drawer1", "/redfish/v1/Chassis/Drawer2") \
            .commit()

        child1 = self.builder.build("/redfish/v1/Chassis/Drawer1",
                                    "#Chassis.v1_2_0.Chassis",
                                    "/redfish/v1/$metadata#Chassis/Members/$entity",
                                    ChassisType='Rack'
                                    )\
            .with_location(id='LocationA', parent_id='Rack') \
            .contained_by("/redfish/v1/Chassis/Rack") \
            .commit()

        with StdoutCapture() as output:
            self.assertEqual(ValidationStatus.FAILED, self.validator.validate_all_or_none_chassis_have_location())

        self.assertIn('Some chassis have Oem/Intel_RackScale/Location some does not', '\n'.join(output))

    def test_failed_when_none_resources_have_location(self):
        parent = self.builder.build("/redfish/v1/Chassis/Rack",
                                    "#Chassis.v1_2_0.Chassis",
                                    "/redfish/v1/$metadata#Chassis/Members/$entity",
                                    ChassisType='Rack'
                                    ) \
            .contains("/redfish/v1/Chassis/Drawer1", "/redfish/v1/Chassis/Drawer2") \
            .commit()

        child1 = self.builder.build("/redfish/v1/Chassis/Drawer1",
                                    "#Chassis.v1_2_0.Chassis",
                                    "/redfish/v1/$metadata#Chassis/Members/$entity",
                                    ChassisType='Rack'
                                    ) \
            .contained_by("/redfish/v1/Chassis/Rack") \
            .commit()

        with StdoutCapture() as output:
            self.assertEqual(ValidationStatus.FAILED, self.validator.validate_all_or_none_chassis_have_location())
