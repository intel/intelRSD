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

package com.intel.rsd.resourcemanager.layers.unifier.patterns;

import lombok.val;
import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.util.Collection;
import java.util.List;
import java.util.stream.Collectors;

import static com.google.common.base.Joiner.on;
import static java.nio.charset.StandardCharsets.UTF_8;
import static org.testng.Assert.assertEquals;
import static org.testng.collections.Lists.newArrayList;

public class UnificationPatternFactoryTest {
    @Test(dataProvider = "testCreateTestCases")
    public void testCreate(String pattern) {
        val sut = new UnificationPatternFactory();
        val unificationPattern = sut.create(pattern);

        assertEquals(unificationPattern.toString(), pattern);
    }

    @Test
    public void testReadFromStream() throws IOException {
        val sut = new UnificationPatternFactory();

        Collection<String> expected = getAllPatterns();

        String file = on('\n').join(expected);
        ByteArrayInputStream stream = new ByteArrayInputStream(file.getBytes(UTF_8));

        List<String> actual = sut.readFromStream(stream)
            .stream()
            .map(Object::toString)
            .collect(Collectors.toList());

        assertEquals(actual, expected);
    }

    @DataProvider(name = "testCreateTestCases")
    public Object[][] testCreateTestCases() throws IOException {
        return getAllPatterns().stream().map(p -> new Object[]{p}).toArray(Object[][]::new);
    }

    public Collection<String> getAllPatterns() {
        return newArrayList(
            "/redfish -> /redfish",
            "/redfish/v1 -> /redfish/v1",
            "/redfish/v2 -> /redfish/v1",
            "/redfish/v1/Systems -> /redfish/v1/Systems",
            "/redfish/v1/Systems/{} -> /redfish/v1/Systems/{#/UUID}",
            "[/redfish/v1/Systems]/{} -> []/{#/UUID}",
            "[/redfish/v1/Systems/{}]/Processors -> []/Processors",
            "[/redfish/v1/Systems/{}]/Processors/{} -> []/Processors/{#/Socket}",
            "[/redfish/v1/Systems/{}/Processors]/{} -> []/{#/Socket}",
            "/redfish/v1/TaskService -> /redfish/v1/TaskService",
            "/redfish/v1/TaskService/Tasks -> /redfish/v1/TaskService/Tasks",
            "[/redfish/v1/TaskService/Tasks]/{} -> []/{/redfish/v1#/UUID #/Id}",
            "[/redfish/v1/TaskService/Tasks]/{} -> []/{/redfish/v1#/UUID '-' #/Id}",
            "[/redfish/v1/TaskService/Tasks]/{} -> []/{'ConstantPrefix' /redfish/v1#/UUID #/Id}",
            "[/redfish/v1/Fabrics/{}/Endpoints]/{} -> []/Endpoints/{$findId(#/Identifiers)}",
            "[/redfish/v1/Fabrics/{}/Endpoints]/{} -> []/Endpoints/{$findId()}"
        );
    }
}
