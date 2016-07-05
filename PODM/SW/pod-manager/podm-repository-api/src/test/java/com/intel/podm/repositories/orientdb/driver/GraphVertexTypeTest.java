/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.repositories.orientdb.driver;

import com.intel.podm.repository.api.GraphVertexType;
import org.testng.annotations.Test;

import java.util.HashMap;
import java.util.Map;

import static java.util.Collections.emptyMap;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

public class GraphVertexTypeTest {

    @Test(expectedExceptions = IllegalArgumentException.class, expectedExceptionsMessageRegExp = ".*vertexTypeName.*")
    public void whenCreatingGraphVertexTypeWithNullName_ShouldThrow() {
        new GraphVertexType(null, emptyMap());
    }

    @Test(expectedExceptions = IllegalArgumentException.class, expectedExceptionsMessageRegExp = ".*vertexTypeName.*empty.*")
    public void whenCreatingGraphVertexTypeWithEmptyName_ShouldThrow() {
        new GraphVertexType("", emptyMap());
    }

    @Test(expectedExceptions = IllegalArgumentException.class, expectedExceptionsMessageRegExp = ".*vertexProperties.*")
    public void whenCreatingGraphVertexTypeWithNullPropertyMap_ShouldThrow() {
        new GraphVertexType("any name", null);
    }

    @Test
    public void whenComparingGraphVertexTypeToItself_ShouldReturnTrue() {
        GraphVertexType one = new GraphVertexType("test", emptyMap());
        assertTrue(one.equals(one));
    }

    @Test
    public void whenComparingTwoGraphVertexTypesWithSameNameAndProperties_ShouldReturnTrue() {
        GraphVertexType one = new GraphVertexType("test", emptyMap());
        GraphVertexType two = new GraphVertexType("test", emptyMap());
        assertTrue(one.equals(two));
    }

    @Test
    public void whenComparingTwoGraphVertexTypesWithSameNameButDifferentProperties_ShouldReturnFalse() {
        HashMap<String, Class> properties = new HashMap<>();
        properties.put("anything", String.class);
        GraphVertexType one = new GraphVertexType("test", properties);
        GraphVertexType two = new GraphVertexType("test", emptyMap());
        assertFalse(one.equals(two));
    }

    @Test
    public void whenComparingTwoGraphVertexTypesWithDifferentNameAndTheSameProperties_ShouldReturnFalse() {
        GraphVertexType one = new GraphVertexType("test", emptyMap());
        GraphVertexType two = new GraphVertexType("test-another", emptyMap());
        assertFalse(one.equals(two));
    }

    @Test
    public void whenComparingTwoGraphVertexTypesWithDifferentNamesAndDifferentProperties_ShouldReturnFalse() {
        HashMap<String, Class> properties = new HashMap<>();
        properties.put("anything", String.class);
        GraphVertexType one = new GraphVertexType("test", properties);
        GraphVertexType two = new GraphVertexType("test-another", emptyMap());
        assertFalse(one.equals(two));
    }

    @Test
    public  void whenComparingTwoGraphVertexTypesOfDifferentClassButSameNameAndProperties_ShouldReturnFalse() {
        GraphVertexType one = new GraphVertexType("test", emptyMap());
        TestGraphVertexType two = new TestGraphVertexType("test", emptyMap());
        assertFalse(one.equals(two));
    }

    public class TestGraphVertexType {
        private final String vertexTypeName;
        private final Map<String, Class> vertexProperties;

        public TestGraphVertexType(String vertexTypeName, Map<String, Class> vertexProperties) {
            this.vertexTypeName = vertexTypeName;
            this.vertexProperties = vertexProperties;
        }
    }
}
