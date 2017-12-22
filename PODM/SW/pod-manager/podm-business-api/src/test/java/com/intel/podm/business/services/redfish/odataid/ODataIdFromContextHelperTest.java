/*
 * Copyright (c) 2015-2017 Intel Corporation
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

import com.intel.podm.business.services.context.Context;
import com.intel.podm.business.services.context.ContextType;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.CHASSIS;
import static com.intel.podm.business.services.context.ContextType.COMPUTER_SYSTEM;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_PORT;
import static com.intel.podm.business.services.context.ContextType.ETHERNET_SWITCH_PORT_VLAN;
import static com.intel.podm.business.services.context.ContextType.PROCESSOR;
import static com.intel.podm.business.services.context.ContextType.REDUNDANCY;
import static com.intel.podm.business.services.context.ContextType.THERMAL;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.asOdataId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.oDataIdOfCollectionInContext;
import static com.intel.podm.business.services.redfish.odataid.ODataIdFromContextHelper.oDataIdOfServiceRoot;
import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromString;
import static com.intel.podm.common.types.Id.id;
import static org.testng.AssertJUnit.assertEquals;

@SuppressWarnings({"checkstyle:MagicNumber"})
public class ODataIdFromContextHelperTest {

    @Test
    public void testODataIdOfServiceRoot() throws Exception {
        String result = oDataIdOfServiceRoot().toString();
        assertEquals(result, "/redfish/v1");
    }

    @Test
    public void testODataIdOfComputerSystems() throws Exception {
        String result = oDataIdOfCollectionInContext(null, COMPUTER_SYSTEM).toString();

        assertEquals(result, "/redfish/v1/Systems");
    }

    @Test(dataProvider = "contextAndODataId")
    public void test(Context context, String oDataId) {
        ODataId actualODataId = asOdataId(context);
        ODataId expectedODataId = oDataId == null ? null : oDataIdFromString(oDataId);
        assertEquals(actualODataId, expectedODataId);
    }

    @DataProvider
    public Object[][] contextAndODataId() {
        return new Object[][]{
            {
                null,
                null
            },
            {
                contextOf(id(1), COMPUTER_SYSTEM),
                "/redfish/v1/Systems/1"
            },
            {
                contextOf(id(10), COMPUTER_SYSTEM).child(id(20), PROCESSOR),
                "/redfish/v1/Systems/10/Processors/20"
            },
            {
                contextOf(id(11), ETHERNET_SWITCH).child(id(22), ETHERNET_SWITCH_PORT).child(id(33), ETHERNET_SWITCH_PORT_VLAN),
                "/redfish/v1/EthernetSwitches/11/Ports/22/VLANs/33"
            },
            {
                contextOf(id(12), CHASSIS).child(id(""), THERMAL).child(id(34), REDUNDANCY),
                "/redfish/v1/Chassis/12/Thermal#/Redundancy/34"
            }
        };
    }

    @Test(dataProvider = "contextAndCollectionTypeAndODataId")
    public void test(Context context, ContextType collectionType, String oDataId) {
        ODataId actualODataId = oDataIdOfCollectionInContext(context, collectionType);
        ODataId expectedODataId = oDataIdFromString(oDataId);

        assertEquals(actualODataId, expectedODataId);
    }

    @DataProvider
    public Object[][] contextAndCollectionTypeAndODataId() {
        return new Object[][]{
            {
                null,
                COMPUTER_SYSTEM,
                "/redfish/v1/Systems"
            },
            {
                contextOf(id(1), COMPUTER_SYSTEM),
                PROCESSOR,
                "/redfish/v1/Systems/1/Processors"
            },
            {
                contextOf(id(11), ETHERNET_SWITCH).child(id(22), ETHERNET_SWITCH_PORT),
                ETHERNET_SWITCH_PORT_VLAN,
                "/redfish/v1/EthernetSwitches/11/Ports/22/VLANs"
            }
        };
    }

}
