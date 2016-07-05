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

package com.intel.podm.business.entities.ogm;

import com.google.common.collect.ImmutableMap;
import com.intel.podm.business.entities.base.ClassWithOneProperty;
import com.intel.podm.business.entities.base.ClassWithThreeProperties;
import com.intel.podm.business.entities.base.ClassWithTwoProperties;
import com.intel.podm.business.entities.base.ClassWithoutProperties;
import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.repository.api.GraphSchemaManager;
import com.intel.podm.repository.api.GraphVertexType;
import org.testng.annotations.Test;

import java.util.Map;

import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.verify;
import static org.testng.Assert.assertEquals;

@Test(dependsOnGroups = {"DomainObjectInspector"})
public class OgmManagerTest {

    @Test
    public void whenAddingToMapping_ShouldCreateProperVertexType() {
        OgmManager sut = createOgmManager();
        sut.addToMapping(ClassWithoutProperties.class);

        GraphVertexType vertexType = new GraphVertexType(
                "ClassWithoutProperties",
                ImmutableMap.of("wasLastUpdatedByDiscovery", Boolean.class)
        );
        verify(sut.graphSchemaManager).registerVertexType(vertexType);
    }

    @Test
    public void whenAddingDomainObjectWithMultiplePropertiesToMapping_ShouldCreateVertexTypeWithProperProperties() {
        OgmManager sut = createOgmManager();
        Map<String, Class> expectedProperties = ImmutableMap.of(
                "size", Integer.class,
                "wasLastUpdatedByDiscovery", Boolean.class,
                "description", String.class
        );

        sut.addToMapping(ClassWithTwoProperties.class);

        GraphVertexType vertexType = new GraphVertexType("ClassWithTwoProperties", expectedProperties);
        verify(sut.graphSchemaManager).registerVertexType(vertexType);
    }

    @Test
    public void whenAddingMultipleDomainObjectsToMapping_ShouldCreateProperVertexTypes() {
        OgmManager sut = createOgmManager();
        sut.addToMapping(ClassWithOneProperty.class);
        sut.addToMapping(ClassWithoutProperties.class);

        GraphVertexType firstVertexType = new GraphVertexType(
                "ClassWithOneProperty",
                ImmutableMap.of("name", String.class, "wasLastUpdatedByDiscovery", Boolean.class)
        );
        verify(sut.graphSchemaManager).registerVertexType(firstVertexType);

        GraphVertexType secondVertexType = new GraphVertexType(
                "ClassWithoutProperties",
                ImmutableMap.of("wasLastUpdatedByDiscovery", Boolean.class)
        );
        verify(sut.graphSchemaManager).registerVertexType(secondVertexType);
    }

    @Test
    public void whenGettingMappedDomainObjectClassForVertexTypeName_ShouldReturnProperClass() {
        OgmManager sut = createOgmManager();
        sut.addToMapping(ClassWithOneProperty.class);

        Class<? extends DomainObject> actualClass = sut.getDomainObjectClass("ClassWithOneProperty");

        assertEquals(actualClass, ClassWithOneProperty.class);
    }

    @Test
    public void whenGettingMappedDomainObjectClassForTwoVertexTypeName_ShouldReturnProperClasses() {
        OgmManager sut = createOgmManager();
        sut.addToMapping(ClassWithOneProperty.class);
        sut.addToMapping(ClassWithThreeProperties.class);

        Class<? extends DomainObject> firstClass = sut.getDomainObjectClass("ClassWithOneProperty");
        Class<? extends DomainObject> secondClass = sut.getDomainObjectClass("ClassWithThreeProperties");

        assertEquals(firstClass, ClassWithOneProperty.class);
        assertEquals(secondClass, ClassWithThreeProperties.class);
    }

    @Test(expectedExceptions = NullPointerException.class)
    public void whenAddingNullClass_ShouldThrow() {
        OgmManager sut = createOgmManager();
        sut.addToMapping(null);
    }

    @Test(expectedExceptions = NullPointerException.class)
    public void whenGettingDomainObjectClassForNullVertexTypeName_ShouldThrow() {
        OgmManager sut = createOgmManager();
        sut.getDomainObjectClass(null);
    }

    @Test(expectedExceptions = IllegalArgumentException.class)
    public void whenGettingDomainObjectClassForUnmappedVertexTypeName_ShouldThrow() {
        OgmManager sut = createOgmManager();
        sut.getDomainObjectClass("UnknownVertexTypeName");
    }

    private OgmManager createOgmManager() {
        OgmManager sut = new OgmManager();
        sut.graphSchemaManager = mock(GraphSchemaManager.class);
        return sut;
    }
}
