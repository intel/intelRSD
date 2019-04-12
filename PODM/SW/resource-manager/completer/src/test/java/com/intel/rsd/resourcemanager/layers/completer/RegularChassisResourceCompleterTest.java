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

import com.fasterxml.jackson.databind.node.JsonNodeFactory;
import com.fasterxml.jackson.databind.node.ObjectNode;
import lombok.val;
import org.testng.annotations.Test;

import java.util.Collections;

import static com.intel.rsd.json.JsonUtils.readJsonFromFile;
import static org.assertj.core.api.AssertionsForClassTypes.assertThat;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

public class RegularChassisResourceCompleterTest {
    @Test
    public void whenGivenObjectNodeRepresentingRegularChassis_thenShouldBeAbleToComplete() throws Exception {
        val sut = new RegularChassisResourceCompleter();
        boolean result = sut.canCompleteResource(readJsonFromFile(this, "RegularChassis.json"));
        assertTrue(result);
    }

    @Test
    public void whenGivenObjectNodeRepresentingPodChassis_thenShouldNotBeAbleToComplete() throws Exception {
        val sut = new RegularChassisResourceCompleter();
        boolean result = sut.canCompleteResource(readJsonFromFile(this, "PodChassis.json"));
        assertFalse(result);
    }

    @Test
    public void whenRegularChassisIsNotContainedByAnotherChassis_thenRegularChassisShouldBeCompletedWithPodChassis() throws Exception {
        val sut = new RegularChassisResourceCompleter();
        val regularChassisNotContainedByAnotherChassis = readJsonFromFile(this, "RegularChassis.json");
        regularChassisNotContainedByAnotherChassis.with("Links").putNull("ContainedBy");
        sut.onResourcesCrawled(Collections.singleton(new ResourceImpl(regularChassisNotContainedByAnotherChassis)));

        ObjectNode result = sut.completeResource(readJsonFromFile(this, "RegularChassis.json"));

        assertThat(result)
            .matches(
                actual -> actual.at("/Links/ContainedBy/@odata.id").asText().equals("/redfish/v1/Chassis/pod")
            );
    }

    @Test
    public void whenRegularChassisIsContainedByAnotherChassis_thenRegularChassisShouldNotBeCompletedWithPodChassis() throws Exception {
        val sut = new RegularChassisResourceCompleter();
        val regularChassisNotContainedByAnotherChassis = readJsonFromFile(this, "RegularChassis.json");
        regularChassisNotContainedByAnotherChassis.with("Links").put("ContainedBy", "/redfish/v1/Chassis/abc");
        sut.onResourcesCrawled(Collections.singleton(new ResourceImpl(regularChassisNotContainedByAnotherChassis)));

        ObjectNode result = sut.completeResource(readJsonFromFile(this, "RegularChassis.json"));

        assertThat(result)
            .matches(
                actual -> !actual.at("/Links/ContainedBy['@odata.id']").asText().equals("/redfish/v1/Chassis/pod")
            );
    }

    @Test
    public void whenRegularChassisHasNullParentId_thenParentIdShouldBePod() throws Exception {
        val sut = new RegularChassisResourceCompleter();
        val regularChassisNotContainedByAnotherChassis = readJsonFromFile(this, "RegularChassis.json");
        regularChassisNotContainedByAnotherChassis
            .with("Oem")
            .with("Intel_RackScale")
            .with("Location")
            .set("ParentId", JsonNodeFactory.instance.nullNode());
        sut.onResourcesCrawled(Collections.singleton(new ResourceImpl(regularChassisNotContainedByAnotherChassis)));

        ObjectNode result = sut.completeResource(regularChassisNotContainedByAnotherChassis);

        assertThat(result)
            .matches(
                actual -> actual.at("/Oem/Intel_RackScale/Location/ParentId").asText().equals("pod")
            );
    }

    @Test
    public void whenRegularChassisHasNotNullParentId_thenParentIdShouldNotBeChanged() throws Exception {
        val sut = new RegularChassisResourceCompleter();
        val regularChassisNotContainedByAnotherChassis = readJsonFromFile(this, "RegularChassis.json");
        val parentId = JsonNodeFactory.instance.textNode("Chassis1");
        regularChassisNotContainedByAnotherChassis
            .with("Oem")
            .with("Intel_RackScale")
            .with("Location")
            .set("ParentId", parentId);
        sut.onResourcesCrawled(Collections.singleton(new ResourceImpl(regularChassisNotContainedByAnotherChassis)));

        ObjectNode result = sut.completeResource(regularChassisNotContainedByAnotherChassis);

        assertThat(result)
            .matches(
                actual -> actual.at("/Oem/Intel_RackScale/Location/ParentId").equals(parentId)
            );
    }
}
