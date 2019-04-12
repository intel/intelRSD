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

package com.intel.rsd.resourcemanager.layers.completer;

import com.fasterxml.jackson.databind.node.ObjectNode;
import lombok.val;
import org.testng.annotations.Test;

import java.util.Collections;
import java.util.HashSet;

import static com.intel.rsd.json.JsonUtils.readJsonFromFile;
import static org.assertj.core.api.AssertionsForClassTypes.assertThat;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

public class PodChassisResourceCompleterTest {
    @Test
    public void whenGivenObjectNodeRepresentingPodChassis_thenShouldBeAbleToComplete() throws Exception {
        val sut = new PodChassisResourceCompleter(new HashSet<>());
        boolean result = sut.canCompleteResource(readJsonFromFile(this, "PodChassis.json"));
        assertTrue(result);
    }

    @Test
    public void whenGivenObjectNodeRepresentingRegularChassis_thenShouldNotBeAbleToComplete() throws Exception {
        val sut = new PodChassisResourceCompleter(new HashSet<>());
        boolean result = sut.canCompleteResource(readJsonFromFile(this, "RegularChassis.json"));
        assertFalse(result);
    }

    @Test
    public void whenRegularChassisIsNotContainedByAnotherChassis_thenPodChassisShouldBeCompletedWithIt() throws Exception {
        val sut = new PodChassisResourceCompleter(new HashSet<>());
        val regularChassisNotContainedByAnotherChassis = readJsonFromFile(this, "RegularChassis.json");
        regularChassisNotContainedByAnotherChassis.with("Links").putNull("ContainedBy");
        sut.onResourcesCrawled(Collections.singleton(new ResourceImpl(regularChassisNotContainedByAnotherChassis)));

        ObjectNode result = sut.completeResource(readJsonFromFile(this, "PodChassis.json"));

        assertThat(result)
            .matches(
                actual -> actual.at("/Links/Contains/0/@odata.id").asText().equals(regularChassisNotContainedByAnotherChassis.get("@odata.id").asText())
            );
    }

    @Test
    public void whenRegularChassisIsContainedByAnotherChassis_thenPodChassisShouldNotBeCompletedWithIt() throws Exception {
        val sut = new PodChassisResourceCompleter(new HashSet<>());
        val regularChassisContainedByAnotherChassis = readJsonFromFile(this, "RegularChassis.json");
        regularChassisContainedByAnotherChassis.with("Links").put("ContainedBy", "/redfish/v1/Chassis/Drawer1");
        sut.onResourcesCrawled(Collections.singleton(new ResourceImpl(regularChassisContainedByAnotherChassis)));

        ObjectNode result = sut.completeResource(readJsonFromFile(this, "PodChassis.json"));

        assertThat(result)
            .matches(
                actual -> actual.at("/Links/Contains").isArray() && actual.at("/Links/Contains").size() == 0
            );
    }
}
