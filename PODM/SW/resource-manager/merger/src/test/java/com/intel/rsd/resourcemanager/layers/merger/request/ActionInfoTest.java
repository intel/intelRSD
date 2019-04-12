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

package com.intel.rsd.resourcemanager.layers.merger.request;

import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.node.ArrayNode;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.io.IOException;
import java.util.stream.StreamSupport;

import static com.intel.rsd.json.JsonUtils.readJsonFromFile;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

public class ActionInfoTest {

    @Test(expectedExceptions = IllegalArgumentException.class)
    public void whenGivenPathIsNotActionPath_exceptionShouldBeThrown() {
        new ActionInfo("/redfish/v1");
    }

    @Test(dataProvider = "testResourcePathExtraction")
    public void testResourcePathExtraction(String path, String resourcePath) throws Exception {
        ActionInfo sut = new ActionInfo(path);
        assertEquals(sut.getResourcePath(), resourcePath);
    }

    @Test(dataProvider = "testActionNameExtraction")
    public void testActionNameExtraction(String path, String actionName) throws Exception {
        ActionInfo sut = new ActionInfo(path);
        assertEquals(sut.getActionName(), actionName);
    }

    @Test(dataProvider = "testIsDefined")
    public void testIsDefinedOn(String actionPath, JsonNode jsonNode) {
        ActionInfo sut = new ActionInfo(actionPath);
        assertTrue(sut.isSupportedBy(jsonNode));
    }

    @Test
    public void whenCorrectActionUriIsGiven_trueShouldBeReturned() {
        assertTrue(ActionInfo.isActionUri("/redfish/v1/Systems/e36f8ea4-7ad2-11e8-a7c2-b3837769af31/Actions/ComputerSystem.Reset"));
    }

    @Test
    public void whenInvalidActionUriIsGiven_falseShouldBeReturned() {
        assertFalse(ActionInfo.isActionUri("/redfish/v1/Systems/e36f8ea4-7ad2-11e8-a7c2-b3837769af31/Actions"));
    }

    @DataProvider(name = "testResourcePathExtraction")
    private Object[][] loadTestResourcePathExtractionData() throws IOException {
        return StreamSupport.stream(readJsonFromFile(this, "ActionInfo.json", ArrayNode.class).spliterator(), false)
            .map(node -> new Object[]{node.get("Path").asText(), node.get("ResourcePath").asText()})
            .toArray(Object[][]::new);
    }

    @DataProvider(name = "testActionNameExtraction")
    private Object[][] loadTestActionNameExtractionData() throws IOException {
        return StreamSupport.stream(readJsonFromFile(this, "ActionInfo.json", ArrayNode.class).spliterator(), false)
            .map(node -> new Object[]{node.get("Path").asText(), node.get("ActionName").asText()})
            .toArray(Object[][]::new);
    }

    @DataProvider(name = "testIsDefined")
    private Object[][] loadTestIsDefinedData() throws IOException {
        return StreamSupport.stream(readJsonFromFile(this, "ActionInfo.json", ArrayNode.class).spliterator(), false)
            .map(node -> new Object[]{node.get("Path").asText(), node.get("ContainingFragment")})
            .toArray(Object[][]::new);
    }
}
