/*
 * Copyright (c) 2017 Intel Corporation
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
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import static com.intel.podm.business.services.context.Context.contextOf;
import static com.intel.podm.business.services.context.ContextType.CHASSIS;
import static com.intel.podm.business.services.context.ContextType.POWER;
import static com.intel.podm.business.services.context.ContextType.POWER_CONTROL;
import static com.intel.podm.business.services.context.ContextType.REDUNDANCY;
import static com.intel.podm.business.services.context.ContextType.THERMAL;
import static com.intel.podm.business.services.context.ContextType.THERMAL_TEMPERATURE;
import static com.intel.podm.common.types.Id.id;
import static com.intel.podm.business.services.redfish.odataid.ODataIdAsJsonPointerHelper.asJsonPointerOdataId;
import static com.intel.podm.business.services.redfish.odataid.ODataIdHelper.oDataIdFromString;
import static org.testng.AssertJUnit.assertEquals;

@SuppressWarnings({"checkstyle:MagicNumber"})
public class ODataIdAsJsonPointerHelperTest {

    @Test(dataProvider = "contextAndODataIdAsJsonPointer")
    public void testOdataIdFromContext(Context context, String oDataId, String jsonPointer) {
        ODataId actualODataId = asJsonPointerOdataId(context, jsonPointer);
        ODataId expectedODataId = oDataId == null ? null : oDataIdFromString(oDataId);
        assertEquals(actualODataId, expectedODataId);
    }

    @DataProvider
    public Object[][] contextAndODataIdAsJsonPointer() {
        return new Object[][]{
            {
                contextOf(id(12), CHASSIS).child(id(""), THERMAL).child(id(34), REDUNDANCY),
                "/redfish/v1/Chassis/12/Thermal#/Redundancy/1",
                "1"
            },
            {
                contextOf(id(12), CHASSIS).child(id(""), THERMAL).child(id(34), THERMAL_TEMPERATURE),
                "/redfish/v1/Chassis/12/Thermal#/Temperatures/9",
                "9"
            },
            {
                contextOf(id(12), CHASSIS).child(id(""), POWER).child(id(34), POWER_CONTROL),
                "/redfish/v1/Chassis/12/Power#/PowerControl/4",
                "4"
            }
        };
    }
}
