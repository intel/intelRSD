import unittest

from cts_core.discovery.api_resource import ApiResource

from cts_core.validation.validation_status import ValidationStatus
from cts_core.metadata.metadata_manager import MetadataManager
from cts_core.commons.preconditions import Preconditions, Requirement
from cts_core.discovery.discovery_container import DiscoveryContainer

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

class PreconditionsUnitTest(unittest.TestCase):

    def setUp(self):
        metadata_manager = MetadataManager(["qualifier"])
        self.metadata_container = metadata_manager.read_metadata_from_strings("Unknown", METADATA)

    def test_not_found_in_empty_discovery_container(self):
        discovery_container = DiscoveryContainer(metadata_container=self.metadata_container)
        requirements = [
            Requirement("Chassis.Chassis", min=1, max=1)
        ]
        preconditions = Preconditions(self.metadata_container, requirements)
        self.assertEqual(ValidationStatus.FAILED, preconditions.validate(discovery_container))

    def test_not_found_in_non_empty_discovery_container(self):
        discovery_container = DiscoveryContainer(metadata_container=self.metadata_container)
        discovery_container.add_resource(ApiResource("id_1", 'netloc', {'@odata.id': '123',
                                                              'Oem':
                                                                  {'Intel_RackScale':
                                                                       {'Location':
                                                                            {'Id': 1, 'ParentId': None}
                                                                        }
                                                                   }
                                                    }, None))
        requirements = [
            Requirement("Chassis.Chassis", min=1, max=1)
        ]
        preconditions = Preconditions(self.metadata_container, requirements)
        self.assertEqual(ValidationStatus.FAILED, preconditions.validate(discovery_container))

    def test_found_in_non_empty_discovery_container(self):
        discovery_container = DiscoveryContainer(metadata_container=self.metadata_container)
        discovery_container.add_resource(ApiResource("id_1", 'netloc', {'@odata.id': '123',
                                                              '@odata.type' : '#Chassis.v1_0_0.Chassis',
                                                              'Oem':
                                                                  {'Intel_RackScale':
                                                                       {'Location':
                                                                            {'Id': 1, 'ParentId': None}
                                                                        }
                                                                   }
                                                    }, None))
        requirements = [
            Requirement("Chassis.Chassis", min=1, max=1)
        ]
        preconditions = Preconditions(self.metadata_container, requirements)
        self.assertEqual(ValidationStatus.PASSED, preconditions.validate(discovery_container))


    def test_fail_when_less_than_min(self):
        discovery_container = DiscoveryContainer(metadata_container=self.metadata_container)
        discovery_container.add_resource(ApiResource("id_1", 'netloc', {'@odata.id': '123',
                                                              '@odata.type' : '#Chassis.v1_0_0.Chassis',
                                                              'Oem':
                                                                  {'Intel_RackScale':
                                                                       {'Location':
                                                                            {'Id': 1, 'ParentId': None}
                                                                        }
                                                                   }
                                                    }, None))
        requirements = [
            Requirement("Chassis.Chassis", min=2, max=1)
        ]
        preconditions = Preconditions(self.metadata_container, requirements)
        self.assertEqual(ValidationStatus.FAILED, preconditions.validate(discovery_container))


    def test_fail_when_more_than_max(self):
        discovery_container = DiscoveryContainer(metadata_container=self.metadata_container)
        discovery_container.add_resource(ApiResource("id_1", 'netloc', {'@odata.id': '123',
                                                              '@odata.type' : '#Chassis.v1_0_0.Chassis',
                                                              'Oem':
                                                                  {'Intel_RackScale':
                                                                       {'Location':
                                                                            {'Id': 1, 'ParentId': None}
                                                                        }
                                                                   }
                                                    }, None))
        requirements = [
            Requirement("Chassis.Chassis", min=0, max=0)
        ]
        preconditions = Preconditions(self.metadata_container, requirements)
        self.assertEqual(ValidationStatus.FAILED, preconditions.validate(discovery_container))
