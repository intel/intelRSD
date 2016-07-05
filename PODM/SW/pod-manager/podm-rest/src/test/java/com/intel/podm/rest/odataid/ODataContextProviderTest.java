/*
 * Copyright (c) 2016 Intel Corporation
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

package com.intel.podm.rest.odataid;

import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.net.URI;

import static org.testng.Assert.assertEquals;

public class ODataContextProviderTest {

    @DataProvider
    private Object[][] OdataIdToContext() {
        return new Object[][] {
                {"/redfish/v1", "/redfish/v1/$metadata#ServiceRoot"},
                {"/redfish/v1/Chassis", "/redfish/v1/$metadata#Chassis"},
                {"/redfish/v1/Services", "/redfish/v1/$metadata#Services"},
                {"/redfish/v1/Systems", "/redfish/v1/$metadata#Systems"},
                {"/redfish/v1/Managers", "/redfish/v1/$metadata#Managers"},
                {"/redfish/v1/EventService", "/redfish/v1/$metadata#EventService"},
                {"/redfish/v1/Nodes", "/redfish/v1/$metadata#Nodes"},
                {"/redfish/v1/EthernetSwitches", "/redfish/v1/$metadata#EthernetSwitches"},
                {"/redfish/v1/Chassis/1", "/redfish/v1/$metadata#Chassis/Members/$entity"},
                {"/redfish/v1/Chassis/2", "/redfish/v1/$metadata#Chassis/Members/$entity"},
                {"/redfish/v1/Services/1", "/redfish/v1/$metadata#Services/Members/$entity"},
                {"/redfish/v1/Services/2", "/redfish/v1/$metadata#Services/Members/$entity"},
                {"/redfish/v1/EventService/Subscriptions", "/redfish/v1/$metadata#EventService/Subscriptions/$entity"},
                {"/redfish/v1/EventService/Subscriptions/1", "/redfish/v1/$metadata#EventService/Subscriptions/Members/$entity"},
                {"/redfish/v1/Systems/SDFAEWERSE", "/redfish/v1/$metadata#Systems/Members/$entity"},
                {"/redfish/v1/Systems/2/Adapters/4/Devices",
                        "/redfish/v1/$metadata#Systems/Members/2/Oem/Intel_RackScale/Adapters/Members/4/Devices/$entity"},
                {"/redfish/v1/Systems/2/Adapters/4/Devices/1",
                        "/redfish/v1/$metadata#Systems/Members/2/Oem/Intel_RackScale/Adapters/Members/4/Devices/Members/$entity"},
                {"/redfish/v1/Managers/BMC/NetworkProtocol", "/redfish/v1/$metadata#Managers/Members/BMC/NetworkProtocol/$entity"},
                {"/redfish/v1/Services/1/Targets", "/redfish/v1/$metadata#Services/Members/1/RemoteTargets/$entity"},
                {"/redfish/v1/Services/2/Targets/2", "/redfish/v1/$metadata#Services/Members/2/RemoteTargets/Members/$entity"}
        };
    }

    @Test(dataProvider = "OdataIdToContext")
    public void checkBasicOneParameterCollectionContext(String odataId, String expectedResult) {
        ODataId oDataId = ODataId.oDataId(odataId);
        URI expectedContext = URI.create(expectedResult);

        URI actualContext = ODataContextProvider.getContextFromId(oDataId);

        assertEquals(actualContext, expectedContext);
    }

}
