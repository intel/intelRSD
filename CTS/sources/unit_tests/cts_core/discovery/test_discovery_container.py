"""
 * @section LICENSE
 *
 * @copyright
 * Copyright (c) 2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @section DESCRIPTION
"""
import re
import unittest

from cts_core.discovery.discovery_container import DiscoveryContainer, \
    equal, not_equal, less_or_equal, less, greater_or_equal, greater, first, last, order_by, \
    from_collection, urls
from cts_core.metadata.metadata_manager import MetadataManager
from cts_core.discovery.api_resource import ApiResource
from cts_core.commons.error import cts_error
from unit_tests.helpers.stdout_capture import StdoutCapture


class DiscoveryContainerTest(unittest.TestCase):
    TEST_METADATA = """
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
        
        <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="EthernetSwitchCollection">
          <EntityType Name="EthernetSwitchCollection" BaseType="Resource.v1_0_0.ResourceCollection">
            <NavigationProperty Name="Members" Type="Collection(EthernetSwitch.v1_0_0.EthernetSwitch)">
              <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
              <Annotation Term="OData.Description" String="Contains the members of this collection."/>
              <Annotation Term="OData.AutoExpandReferences"/>
            </NavigationProperty>
          </EntityType>
        </Schema>

        <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="EthernetSwitch.v1_0_0">
          <EntityType Name="EthernetSwitch" BaseType="Resource.v1_0_0.Resource">
            <NavigationProperty Name="Ports" Type="EthernetSwitchPortCollection.EthernetSwitchPortCollection"
                                ContainsTarget="true">
              <Annotation Term="OData.Description" String="Collection of switch ports"/>
              <Annotation Term="OData.LongDescription" String="Collection of switch ports"/>
              <Annotation Term="OData.AutoExpandReferences"/>
            </NavigationProperty>
          </EntityType>
        </Schema>

        <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="EthernetSwitchPortCollection">
          <EntityType Name="EthernetSwitchPortCollection" BaseType="Resource.v1_0_0.ResourceCollection">
            <NavigationProperty Name="Members" Type="Collection(EthernetSwitchPort.v1_0_0.EthernetSwitchPort)">
              <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
              <Annotation Term="OData.Description" String="Contains the members of this collection."/>
              <Annotation Term="OData.AutoExpandReferences"/>
            </NavigationProperty>
          </EntityType>
        </Schema>
        
        <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="EthernetSwitchPort">
          <EntityType Name="EthernetSwitchPort" BaseType="Resource.v1_0_0.Resource">
          </EntityType>
        </Schema>
    
        <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="EthernetSwitchPort.v1_0_0">
          <EntityType Name="EthernetSwitchPort" BaseType="EthernetSwitchPort.EthernetSwitchPort">
            <Property Name="PortId" Type="Edm.String">
              <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
              <Annotation Term="OData.Description" String="Switch port unique identifier."/>
              <Annotation Term="OData.LongDescription" String="Switch port unique identifier."/>
            </Property>
          </EntityType>
        </Schema>
        
    <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="TaskService">
      <EntityType Name="TaskService" BaseType="Resource.v1_0_0.Resource" Abstract="true">
        <Annotation Term="OData.Description"
                    String="The Task Service of a Redfish service allows scheduling and execution of long-duration processes.  It represents the properties for the Task Service itself and has links to the actual collection of Task resources."/>
        <Annotation Term="OData.LongDescription"
                    String="This resource shall be used to represent a task service for a Redfish implementation."/>
      </EntityType>
    </Schema>

    <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="TaskService.v1_0_0">

      <EntityType Name="TaskService" BaseType="TaskService.TaskService">
        <Property Name="LifeCycleEventOnTaskStateChange" Type="Edm.Boolean" Nullable="false">
          <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
          <Annotation Term="OData.Description" String="Send an Event upon Task State Change."/>
          <Annotation Term="OData.LongDescription"
                      String="The value of this property, if set to true, shall indicate that the service shall send a LifeCycle event to ListenerDestinations registered for such events upon change of task state."/>
        </Property>
        <NavigationProperty Name="Tasks" Type="TaskCollection.TaskCollection" Nullable="false">
          <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
          <Annotation Term="OData.Description" String="References to the Tasks collection."/>
          <Annotation Term="OData.LongDescription"
                      String="The value of this property shall be a link to a resource of type TaskCollection."/>
        <Annotation Term="OData.AutoExpandReferences"/>
        </NavigationProperty>
      </EntityType>
    </Schema>
    
    <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="TaskCollection">
      <EntityType Name="TaskCollection" BaseType="Resource.v1_0_0.ResourceCollection">
        <NavigationProperty Name="Members" Type="Collection(Task.Task)" Nullable="false">
          <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
          <Annotation Term="OData.Description" String="Contains the members of this collection."/>
          <Annotation Term="OData.AutoExpandReferences"/>
        </NavigationProperty>
      </EntityType>
    </Schema>


    <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="Task">
      <EntityType Name="Task" BaseType="Resource.v1_0_0.Resource" Abstract="true">
        <Annotation Term="OData.Description"
                    String="This resource contains information about a specific Task scheduled by or being executed by a Redfish service's Task Service."/>
        <Annotation Term="OData.LongDescription"
                    String="This resource shall be used to represent a task for a Redfish implementation."/>
      </EntityType>
    </Schema>

    <Schema xmlns="http://docs.oasis-open.org/odata/ns/edm" Namespace="Task.v1_0_0">
      <EntityType Name="Task" BaseType="Task.Task">
        <Property Name="Name" Type="Edm.String" Nullable="false">
          <Annotation Term="OData.Permissions" EnumMember="OData.Permission/Read"/>
          <Annotation Term="OData.Description" String="The Name"/>
        </Property>
      </EntityType>
    </Schema>
  </Schemas>
    """

    def test_empty_root_resources_when_location_not_specified(self):
        container = DiscoveryContainer()
        container.add_resource(ApiResource("id_1", 'netloc', {}, None))
        container.add_resource(ApiResource("id_1", 'netloc', {'some_property' : 'some_value'}, None))

        self.assertFalse(container.root_resources())


    def test_empty_root_resources_when_none_location_has_empty_parentid(self):
        container = DiscoveryContainer()
        container.add_resource(ApiResource("id_1", 'netloc', {'Oem' :
                                                        {'Intel_RackScale' :
                                                             {'Location' :
                                                                  {'Id' : 1, 'ParentId' : 2}
                                                             }
                                                        }
                                                   }, None))
        container.add_resource(ApiResource("id_2", 'netloc', {'Oem':
                                                        {'Intel_RackScale':
                                                             {'Location':
                                                                  {'Id': 3, 'ParentId': 4}
                                                              }
                                                         }
                                                    }, None))

        self.assertFalse(container.root_resources())

    def test_non_empty_root_resources(self):
        container = DiscoveryContainer()
        container.add_resource(ApiResource("id_1", 'netloc', {'@odata.id' : '123',
                                                    'Oem':
                                                        {'Intel_RackScale':
                                                             {'Location':
                                                                  {'Id': 1, 'ParentId': None}
                                                              }
                                                         }
                                                    }, None))
        container.add_resource(ApiResource("id_2", 'netloc', {'Oem':
                                                        {'Intel_RackScale':
                                                             {'Location':
                                                                  {'Id': 3, 'ParentId': 4}
                                                              }
                                                         }
                                                    }, None))

        root = container.root_resources()
        self.assertEqual(1, len(root))
        self.assertEqual('123', root[0].odata_id)

    def test_more_than_one_root_resource(self):
        container = DiscoveryContainer()
        container.add_resource(ApiResource("id_1", 'netloc', {'@odata.id': '123',
                                                    'Oem':
                                                        {'Intel_RackScale':
                                                             {'Location':
                                                                  {'Id': 1, 'ParentId': None}
                                                              }
                                                         }
                                                    }, None))
        container.add_resource(ApiResource("id_2", 'netloc', {'@odata.id': '456',
                                                    'Oem':
                                                        {'Intel_RackScale':
                                                             {'Location':
                                                                  {'Id': 3, 'ParentId': None}
                                                              }
                                                         }
                                                    }, None))

        root_ids = [r.odata_id for r in container.root_resources()]
        self.assertEqual(2, len(root_ids))
        self.assertIn('123', root_ids)
        self.assertIn('456', root_ids)

    def test_iter_all(self):
        metadata_manager = MetadataManager(["qualifier"])
        discovery_container = DiscoveryContainer(
            metadata_container=metadata_manager.read_metadata_from_strings("Unknown", self.TEST_METADATA))

        chassis1_2 = {
              "@odata.context": "/redfish/v1/$metadata#Chassis/Members/$entity",
              "@odata.id": "/redfish/v1/Chassis/Rack1",
              "@odata.type": "#Chassis.v1_2_0.Chassis",
              "ChassisType": "Rack",
            }
        discovery_container.add_resource(ApiResource("/redfish/v1/Chassis/Rack1", 'netloc',chassis1_2, "Chassis.v1_2_0.Chassis"))

        self.assertEqual(1, len(list(discovery_container.iter_all("Chassis.v1_2_0.Chassis"))))
        self.assertEqual(1, len(list(discovery_container.iter_all("Chassis.v1_0_0.Chassis"))))
        self.assertEqual(1, len(list(discovery_container.iter_all("Chassis.Chassis"))))
        self.assertEqual(1, len(list(discovery_container.iter_all("Resource.v1_0_0.Resource"))))
        self.assertEqual(1, len(list(discovery_container.iter_all("Resource.Resource"))))

        chassis1_0 = {
              "@odata.context": "/redfish/v1/$metadata#Chassis/Members/$entity",
              "@odata.id": "/redfish/v1/Chassis/Module1",
              "@odata.type": "#Chassis.v1_0_0.Chassis",
              "ChassisType": "Module",
            }
        discovery_container.add_resource(ApiResource("/redfish/v1/Chassis/Module1",
                                                     'netloc', chassis1_0,
                                                     "Chassis.v1_0_0.Chassis"))

        self.assertEqual(1, len(list(discovery_container.iter_all("Chassis.v1_2_0.Chassis"))))
        self.assertEqual(2, len(list(discovery_container.iter_all("Chassis.v1_0_0.Chassis"))))
        self.assertEqual(2, len(list(discovery_container.iter_all("Chassis.Chassis"))))
        self.assertEqual(2, len(list(discovery_container.iter_all("Resource.v1_0_0.Resource"))))
        self.assertEqual(2, len(list(discovery_container.iter_all("Resource.Resource"))))

    def test_generalized_odata_type(self):
        discovery_container = DiscoveryContainer(service_root="/redfish/v1")

        switch_collection = {
            "@odata.context": "/redfish/v1/$metadata#EthernetSwitchCollection.EthernetSwitchCollection",
            "@odata.id": "/redfish/v1/EthernetSwitches",
            "@odata.type": "#EthernetSwitchCollection.EthernetSwitchCollection",
            "Members": [
                {
                    "@odata.id": "/redfish/v1/EthernetSwitches/1"
                }
            ],
            "Members@odata.count": 1
        }

        switch = {
            "@odata.context": "/redfish/v1/$metadata#EthernetSwitch.EthernetSwitch",
            "@odata.id": "/redfish/v1/EthernetSwitches/1",
            "@odata.type": "#EthernetSwitch.v1_0_0.EthernetSwitch",
            "Ports": {
                "@odata.id": "redfish/v1/EthernetSwitches/1/Ports"
            }
        }

        switch_port_collection = {
            "@odata.context": "/redfish/v1/$metadata#EthernetSwitchPortCollection.EthernetSwitchPortCollection",
            "@odata.id": "/redfish/v1/EthernetSwitches/1/Ports",
            "@odata.type": "#EthernetSwitchPortCollection.EthernetSwitchPortCollection",
            "Members": [
                {
                    "@odata.id": "/redfish/v1/EthernetSwitches/1/Ports/xyz"
                },
                {
                    "@odata.id": "/redfish/v1/EthernetSwitches/1/Ports/22-grubykot-1"
                }
            ],
            "Members@odata.count": 2
        }

        port1 = {
            "@odata.context": "/redfish/v1/$metadata#EthernetSwitchPort.EthernetSwitchPort",
            "@odata.id": "/redfish/v1/EthernetSwitches/1/Ports/xyz",
            "@odata.type": "#EthernetSwitchPort.v1_0_0.EthernetSwitchPort",
            "PortId": "kot, ale czyj?"
        }

        port2 = {
            "@odata.context": "/redfish/v1/$metadata#EthernetSwitchPort.EthernetSwitchPort",
            "@odata.id": "/redfish/v1/EthernetSwitches/1/Ports/22-grubykot-1",
            "@odata.type": "#EthernetSwitchPort.v1_0_0.EthernetSwitchPort",
            "PortId": "prezes, ale jakie ma zwierze?"
        }

        task_service = {
            "@odata.context": "/redfish/v1/$metadata#TaskService.TaskService",
            "@odata.id": "/redfish/v1/TaskService",
            "@odata.type": "#TaskService.v1_0_0.TaskService",
            "Tasks": {
                "@odata.id": "/redfish/v1/TaskService/Tasks"
            }
        }

        task_collection = {
            "@odata.context": "/redfish/v1/$metadata#TaskCollection.TaskCollection",
            "@odata.id": "/redfish/v1/TaskService/Tasks",
            "@odata.type": "#TaskCollection.TaskCollection",
            "Members": [
                {
                    "@odata.id": "/redfish/v1/TaskService/Tasks/first_task"
                }
            ],
            "Members@odata.count": 0,
        }

        task = {
            "@odata.context": "/redfish/v1/$metadata#Task.Task",
            "@odata.id": "/redfish/v1/TaskService/Tasks/first_task",
            "@odata.type": "#Task.v1_0_0.Task",
            "Name": "Task"
        }

        discovery_container.add_resource(ApiResource("/redfish/v1/EthernetSwitches",
                                                     'netloc', switch_collection,
                                                     'EthernetSwitchCollection.v1_0_0.EthernetSwitchCollection'))
        discovery_container.register_url_pattern('/redfish/v1/EthernetSwitches',
                                                 "EthernetSwitchCollection.EthernetSwitchCollection")

        discovery_container.add_resource(ApiResource("/redfish/v1/EthernetSwitches/1",
                                                     'netloc', switch,
                                                     'EthernetSwitch.v1_0_0.EthernetSwitch'))
        discovery_container.register_url_pattern('/redfish/v1/EthernetSwitches/1',
                                                 'EthernetSwitch.v1_0_0.EthernetSwitch')

        discovery_container.add_resource(ApiResource("/redfish/v1/EthernetSwitches/1/Ports",
                                                     'netloc', switch_port_collection,
                                                     'EthernetSwitchPortCollection.EthernetSwitchPortCollection"'))
        discovery_container.register_url_pattern('/redfish/v1/EthernetSwitches/1/Ports',
                                                 'EthernetSwitchPortCollection.EthernetSwitchPortCollection"')

        discovery_container.add_resource(ApiResource("/redfish/v1/EthernetSwitches/1/Ports/xyz",
                                                     'netloc', port1,
                                                     'EthernetSwitchPort.v1_0_0.EthernetSwitchPort'))
        discovery_container.register_url_pattern('/redfish/v1/EthernetSwitches/1/Ports/xyz',
                                                 'EthernetSwitchPort.v1_0_0.EthernetSwitchPort')

        discovery_container.add_resource(ApiResource("/redfish/v1/EthernetSwitches/1/Ports/22-grubykot-1",
                                                     'netloc', port2,
                                                     'EthernetSwitchPort.v1_0_0.EthernetSwitchPort'))
        discovery_container.register_url_pattern('/redfish/v1/EthernetSwitches/1/Ports/22-grubykot-1',
                                                 'EthernetSwitchPort.v1_0_0.EthernetSwitchPort')

        discovery_container.add_resource(ApiResource("/redfish/v1/TaskService",
                                                      'netloc', task_service,
                                                      'TaskService.v1_0_0.TaskService'))
        discovery_container.register_url_pattern('/redfish/v1/TaskService',
                                                 'TaskService.v1_0_0.TaskService')

        discovery_container.add_resource(ApiResource("/redfish/v1/TaskService/Tasks",
                                                      'netloc', task_collection,
                                                      'TaskCollection.TaskCollection'))
        discovery_container.register_url_pattern('/redfish/v1/TaskService/Tasks',
                                                 'TaskCollection.TaskCollection')

        discovery_container.add_resource(ApiResource("/redfish/v1/TaskService/Tasks/first_task",
                                                      'netloc', task,
                                                      'Task.v1_0_0.Task'))
        discovery_container.register_url_pattern('/redfish/v1/TaskService/Tasks/first_task',
                                                 'Task.v1_0_0.Task')


        self.assertEqual(
            discovery_container.get_expected_odata_type_for_url("/redfish/v1/EthernetSwitches/x"),
            'EthernetSwitch.v1_0_0.EthernetSwitch')

        self.assertEqual(
            discovery_container.get_expected_odata_type_for_url("/redfish/v1/EthernetSwitches/1/Ports/bla/"),
            'EthernetSwitchPort.v1_0_0.EthernetSwitchPort')

        port1_url = '/redfish/v1/EthernetSwitches/1/Ports/xyz'
        port2_url = '/redfish/v1/EthernetSwitches/1/Ports/22-grubykot-1'
        task_service_url = "/redfish/v1/TaskService"
        task_collection_url = "/redfish/v1/TaskService/Tasks"
        task_url = "/redfish/v1/TaskService/Tasks/first_task"
        task_generalized_url = "/redfish/v1/TaskService/Tasks/*"

        self.assertEqual(task_service_url, discovery_container._generalize_url(task_service_url))
        self.assertEqual(task_collection_url, discovery_container._generalize_url(task_collection_url))
        self.assertEqual(task_generalized_url, discovery_container._generalize_url(task_url))

        # test_identical_error_id_for_related_odata_id(self):
        with StdoutCapture() as output:
            cts_error('{odata_id:id}', odata_id=port1_url)
        match = re.search("\[#Id=(?P<id>[0-9a-f]{32})\]", output.raw)
        self.assertIsNotNone(match)
        id_1 = match.group('id')

        with StdoutCapture() as output:
            cts_error('{odata_id:id}', odata_id=port2_url)
        match = re.search("\[#Id=(?P<id>[0-9a-f]{32})\]", output.raw)
        self.assertIsNotNone(match)
        id_2 = match.group('id')

        self.assertEqual(id_1, id_2)

        # test_different_error_ids_for_generalized_and_not_generalized_odata_id(self):
        with StdoutCapture() as output:
            cts_error('{odata_id:id}', odata_id=port1_url)
        match = re.search("\[#Id=(?P<id>[0-9a-f]{32})\]", output.raw)
        self.assertIsNotNone(match)
        id_for_generalized = match.group('id')

        with StdoutCapture() as output:
            cts_error('odata_id={odata_id}', odata_id=port2_url)
        match = re.search("\[#Id=(?P<id>[0-9a-f]{32})\]", output.raw)
        self.assertIsNotNone(match)
        id_for_not_generalized = match.group('id')

        self.assertNotEqual(id_for_generalized, id_for_not_generalized)


    def test_dataframes(self):
        metadata_container = MetadataManager(["qualifier"]).\
            read_metadata_from_strings("Unknown", self.TEST_METADATA)
        dc = DiscoveryContainer(service_root="/redfish/v1", metadata_container=metadata_container)
        chassis1_2_1 = {
              "@odata.context": "/redfish/v1/$metadata#Chassis/Members/$entity",
              "@odata.id": "/redfish/v1/Chassis/Rack1",
              "@odata.type": "#Chassis.v1_2_0.Chassis",
              "ChassisType": "Rack",
              "SomeValue": 1,
            }
        chassis1_2_2 = {
              "@odata.context": "/redfish/v1/$metadata#Chassis/Members/$entity",
              "@odata.id": "/redfish/v1/Chassis/Rack2",
              "@odata.type": "#Chassis.v1_2_0.Chassis",
              "ChassisType": "Rack",
              "SomeValue": 3,
            }
        chassis1_0_1 = {
              "@odata.context": "/redfish/v1/$metadata#Chassis/Members/$entity",
              "@odata.id": "/redfish/v1/Chassis/Module1",
              "@odata.type": "#Chassis.v1_0_0.Chassis",
              "ChassisType": "Module",
              "SomeValue": 2,
            }
        chassis1_0_2 = {
              "@odata.context": "/redfish/v1/$metadata#Chassis/Members/$entity",
              "@odata.id": "/redfish/v1/Chassis/Module2",
              "@odata.type": "#Chassis.v1_0_0.Chassis",
              "ChassisType": "Module",
              "SomeValue": 4,
            }

        dc.add_resource(ApiResource("/redfish/v1/Chassis/Rack1", 'netloc', chassis1_2_1,
                                    "Chassis.v1_2_0.Chassis"))
        dc.add_resource(ApiResource("/redfish/v1/Chassis/Rack2", 'netloc',chassis1_2_2, "Chassis.v1_2_0.Chassis"))
        dc.add_resource(ApiResource("/redfish/v1/Chassis/Module1", 'netloc',chassis1_0_1, "Chassis.v1_0_0.Chassis"))
        dc.add_resource(ApiResource("/redfish/v1/Chassis/Module2", 'netloc',chassis1_0_2, "Chassis.v1_0_0.Chassis"))

        def _some_values(resource_list):
            return [resource.body["SomeValue"] for _, resource in resource_list]

        self.assertEqual(2, dc.count_resources("Chassis.v1_2_0.Chassis"))
        self.assertEqual(2, dc.count_resources("Chassis.v1_2_0.Chassis", any_child_version=True))
        self.assertEqual(2, dc.count_resources("Chassis.v1_0_0.Chassis"))
        self.assertEqual(4, dc.count_resources("Chassis.v1_0_0.Chassis", any_child_version=True))
        self.assertEqual(0, dc.count_resources("Chassis.Chassis"))
        self.assertEqual(4, dc.count_resources("Chassis.Chassis", any_child_version=True))

        self.assertEqual([1,2,3,4], _some_values(dc.get_resources("Chassis.Chassis", any_child_version=True,
                                                                   constraints=[order_by("SomeValue")])))
        self.assertEqual(2, len(dc.get_resources("Chassis.v1_2_0.Chassis")))
        self.assertEqual(4, len(dc.get_resources("Chassis.Chassis", any_child_version=True)))
        self.assertEqual([1,2,3], _some_values(dc.get_resources("Chassis.Chassis", any_child_version=True,
                                                                                   constraints=[order_by("SomeValue"),
                                                                                                first(3)])))
        self.assertEqual([2,1], _some_values(dc.get_resources("Chassis.Chassis", any_child_version=True,
                                                               constraints=[order_by("SomeValue", ascending=False),
                                                                                              last(2)])))
        self.assertEqual([3,4], _some_values(dc.get_resources("Chassis.Chassis", any_child_version=True,
                                                               constraints=[order_by("SomeValue"), last(2)])))

        self.assertEqual(2, len(dc.get_resources("Chassis.Chassis", any_child_version=True,
                                                      constraints=[less("SomeValue", 3)])))

        self.assertEqual(3, len(dc.get_resources("Chassis.Chassis", any_child_version=True,
                                                      constraints=[less_or_equal("SomeValue", 3)])))

        self.assertEqual(1, len(dc.get_resources("Chassis.Chassis", any_child_version=True,
                                                      constraints=[equal("SomeValue", 3)])))

        self.assertEqual(3, len(dc.get_resources("Chassis.Chassis", any_child_version=True,
                                                      constraints=[not_equal("SomeValue", 3)])))

        self.assertEqual(0, len(dc.get_resources("Chassis.Chassis", any_child_version=False,
                                                 constraints=[order_by("SomeValue")])))

        self.assertEqual(0, len(dc.get_resources("Chassis.v1_2_0.Chassis", any_child_version=False,
                                                 constraints=[greater("SomeValue", 3)])))

        self.assertEqual(1, len(dc.get_resources("Chassis.v1_0_0.Chassis", any_child_version=True,
                                                 constraints=[greater("SomeValue", 3)])))

        self.assertEqual(2, len(dc.get_resources("Chassis.v1_0_0.Chassis", any_child_version=True,
                                                 constraints=[greater_or_equal("SomeValue", 3)])))

        self.assertEqual(4, len(dc.get_resources("Chassis.v1_0_0.Chassis", any_child_version=True,
                                                 constraints=[from_collection("/redfish/v1/Chassis")])))

        chassis1_0_2["@odata.id"] = "/redfish/v1/Chassis2/Module2"
        dc.add_resource(ApiResource("/redfish/v1/Chassis2/Module2", 'netloc', chassis1_0_2,
                                    "Chassis.v1_0_0.Chassis"))

        self.assertEqual(4, len(dc.get_resources("Chassis.v1_0_0.Chassis", any_child_version=True,
                                                 constraints=[from_collection("/redfish/v1/Chassis")])))

        self.assertEqual(1, len(dc.get_resources("Chassis.v1_0_0.Chassis", any_child_version=True,
                                                 constraints=[from_collection("/redfish/v1/Chassis2")])))

        self.assertEqual(2, len(dc.get_resources("Chassis.v1_0_0.Chassis", any_child_version=True,
                                                 constraints=[urls(["/redfish/v1/Chassis/Rack1",
                                                                         "/redfish/v1/Chassis/Module1"])])))
