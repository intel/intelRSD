/*
 * Copyright (c) 2016-2017 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.intel.podm.business.services.redfish.odataid;

import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.net.URI;

import static com.intel.podm.business.services.redfish.odataid.ODataContextProvider.getContextFromODataType;
import static org.testng.Assert.assertEquals;

@SuppressWarnings({"checkstyle:MethodName", "checkstyle:MethodLength"})
public class ODataContextProviderTest {

    @DataProvider
    private Object[][] OdataTypeToContext() {
        return new Object[][]{
            {"#ServiceRoot.v1_1_1.ServiceRoot", "/redfish/v1/$metadata#ServiceRoot.ServiceRoot"},
            {"#ChassisCollection.ChassisCollection", "/redfish/v1/$metadata#ChassisCollection.ChassisCollection"},
            {"#Chassis.v1_3_0.Chassis", "/redfish/v1/$metadata#Chassis.Chassis"},
            {"#Power.v1_1_0.Power", "/redfish/v1/$metadata#Power.Power"},
            {"#Thermal.v1_1_0.Thermal", "/redfish/v1/$metadata#Thermal.Thermal"},
            {"#StorageServiceCollection.StorageServiceCollection", "/redfish/v1/$metadata#StorageServiceCollection.StorageServiceCollection"},
            {"#ComputerSystemCollection.ComputerSystemCollection", "/redfish/v1/$metadata#ComputerSystemCollection.ComputerSystemCollection"},
            {"#TaskService.v1_0_0.TaskService", "/redfish/v1/$metadata#TaskService.TaskService"},
            {"#TaskCollection.TaskCollection", "/redfish/v1/$metadata#TaskCollection.TaskCollection"},
            {"#ComputerSystem.v1_3_0.ComputerSystem", "/redfish/v1/$metadata#ComputerSystem.ComputerSystem"},
            {"#StorageService.v1_0_0.StorageService", "/redfish/v1/$metadata#StorageService.StorageService"},
            {"#RemoteTarget.v1_1_0.RemoteTarget", "/redfish/v1/$metadata#RemoteTarget.RemoteTarget"},
            {"#ManagerCollection.ManagerCollection", "/redfish/v1/$metadata#ManagerCollection.ManagerCollection"},
            {"#EthernetInterface.v1_1_0.EthernetInterface", "/redfish/v1/$metadata#EthernetInterface.EthernetInterface"},
            {"#VLanNetworkInterface.v1_0_0.VLanNetworkInterface", "/redfish/v1/$metadata#VLanNetworkInterface.VLanNetworkInterface"},
            {"#ManagerNetworkProtocol.v1_0_2.ManagerNetworkProtocol", "/redfish/v1/$metadata#ManagerNetworkProtocol.ManagerNetworkProtocol"},
            {"#MetricDefinitionCollection.MetricDefinitionCollection", "/redfish/v1/$metadata#MetricDefinitionCollection.MetricDefinitionCollection"},
            {"#EventDestinationCollection.EventDestinationCollection", "/redfish/v1/$metadata#EventDestinationCollection.EventDestinationCollection"},
            {"#EventDestination.v1_1_1.EventDestination", "/redfish/v1/$metadata#EventDestination.EventDestination"},
            {"#ComposedNodeCollection.ComposedNodeCollection", "/redfish/v1/$metadata#ComposedNodeCollection.ComposedNodeCollection"},
            {"#ComposedNode.v1_0_0.ComposedNode", "/redfish/v1/$metadata#ComposedNode.ComposedNode"}
        };
    }

    @Test(dataProvider = "OdataTypeToContext")
    public void checkOdataTypeToContext(String odataType, String expectedResult) {
        URI expectedContext = URI.create(expectedResult);
        URI actualContext = getContextFromODataType(odataType);

        assertEquals(actualContext, expectedContext);
    }
}
