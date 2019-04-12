/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.resourcemanager.layers.unifier.patterns.matching;

import com.intel.rsd.resourcemanager.layers.unifier.patterns.UnificationPatternFactory;
import lombok.val;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.io.IOException;

import static org.testng.Assert.assertEquals;

public class MatchingPatternTest {
    @Test(dataProvider = "matching")
    public void test(String matchingPattern, String odataId, boolean matching) {
        val factory = new UnificationPatternFactory();
        val pattern = factory.createMatchingPattern(matchingPattern);

        assertEquals(pattern.matches(odataId), matching);
    }

    @DataProvider(name = "matching")
    public Object[][] matching() throws IOException {
        return new Object[][]{
            {"/redfish/v1", "/redfish/v1", true},
            {"/redfish/v1", "/redfish", false},
            {"/redfish/v1/Systems", "/redfish/v1/Systems/", true},
            {"/redfish/{}/Systems", "/redfish/v1/Systems", true},
            {"/redfish/v1/Systems/{}", "/redfish/v1/Systems/1", true},
            {"[/redfish]/v1", "/redfish/v1", true},
            {"[/redfish/{}]/Systems", "/redfish/v1/Systems", true},
            {"[/redfish/{}]/Systems", "/redfish/v2/Systems", true},
            {"[/redfish/{}]/Systems", "/redfish/v2/Systems/Processors", false},
            {"[/redfish/v1/Systems/{}]/Actions/ComputerSystem.Reset", "/redfish/v1/Systems/1/Actions/ComputerSystem.Reset", true},
        };
    }
}
