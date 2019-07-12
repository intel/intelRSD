/*
 * Copyright (c) 2019 Intel Corporation
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

package com.intel.rsd;

import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.util.regex.Pattern;

import static com.intel.rsd.ResourcePatterns.ACTION_PATTERN;
import static com.intel.rsd.ResourcePatterns.CHASSIS_PATTERN;
import static com.intel.rsd.ResourcePatterns.COMPUTER_SYSTEM_ETHERNET_INTERFACE_PATTERN;
import static com.intel.rsd.ResourcePatterns.COMPUTER_SYSTEM_PATTERN;
import static com.intel.rsd.ResourcePatterns.ETHERNET_SWITCH_PORT_PATTERN;
import static org.testng.Assert.assertEquals;

public class ResourcePatternsTest {

    @DataProvider
    public static Object[][] patternsAndMatchers() {
        return new Object[][]{
            {COMPUTER_SYSTEM_PATTERN, true, "/redfish/v1/Systems/44"},
            {COMPUTER_SYSTEM_PATTERN, false, "/redfish/v1/Systems/1/Processor/23"},
            {CHASSIS_PATTERN, true, "/redfish/v1/Chassis/1"},
            {ETHERNET_SWITCH_PORT_PATTERN, true, "/redfish/v1/EthernetSwitches/547ca796-ee50-11e8-addc-6b3291d8f5b5_1/Ports/Ethernet23/4"},
            {COMPUTER_SYSTEM_ETHERNET_INTERFACE_PATTERN, true, "/redfish/v1/Systems/db046a90-d610-11e7-9661-419a3eff5c1a/EthernetInterfaces/87:5B:1B:11:28:F9"},
            {ACTION_PATTERN, true, "/redfish/v1/Systems/1/Actions/ComputerSystem.Reset"},
            {ACTION_PATTERN, true, "/redfish/v1/Systems/1/Actions/Oem/Intel.Oem.EraseOptaneDCPersistentMemory"}
        };
    }

    @Test(dataProvider = "patternsAndMatchers")
    public void testComputerSystemPattern(Pattern pattern, boolean matches, String path) {
        assertEquals(pattern.matcher(path).matches(), matches);
    }

}
