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

package com.intel.podm.business.entities.base;

import com.intel.podm.business.entities.DomainObjectNotFoundException;
import com.intel.podm.common.enterprise.utils.beans.BeanFactory;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.Id;
import com.intel.podm.repository.api.DatabaseDriver;
import com.intel.podm.repository.api.VertexNotFoundException;
import com.tinkerpop.blueprints.Vertex;
import org.testng.annotations.Test;

import javax.enterprise.inject.Instance;
import java.util.List;
import java.util.Optional;

import static com.intel.podm.business.entities.base.DomainObjectProperties.stringProperty;
import static com.intel.podm.common.types.Id.id;
import static java.util.Arrays.asList;
import static java.util.Collections.emptyList;
import static org.mockito.Matchers.any;
import static org.mockito.Mockito.doAnswer;
import static org.mockito.Mockito.doThrow;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertEquals;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertNotNull;
import static org.testng.Assert.assertNull;
import static org.testng.AssertJUnit.assertTrue;

public class DomainObjectRepositoryTest {
    @Test
    public void whenCreatingNewInstance_ShouldReturnObjectCreatedByFactory() {
        DomainObjectRepository sut = createRepository();

        ClassWithOneProperty expectedDomainObject = mock(ClassWithOneProperty.class);
        when(sut.factory.create(ClassWithOneProperty.class)).thenReturn(expectedDomainObject);

        ClassWithOneProperty actualDomainObject = sut.create(ClassWithOneProperty.class);

        assertEquals(actualDomainObject, expectedDomainObject);
    }

    @Test
    public void whenCreatingNewInstance_ShouldReturnInitializedObject() {
        DomainObjectRepository sut = createRepository();

        ClassWithThreeProperties mockedDomainObject = mock(ClassWithThreeProperties.class);
        when(sut.factory.create(ClassWithThreeProperties.class)).thenReturn(mockedDomainObject);

        sut.create(ClassWithThreeProperties.class);

        verify(mockedDomainObject).initializeAsNewInstance();
    }

    @Test(expectedExceptions = NullPointerException.class, expectedExceptionsMessageRegExp = ".*domainObject.*")
    public void whenCreatingNewInstanceOfNullClass_ShouldThrow() {
        DomainObjectRepository sut = createRepository();
        sut.create(null);
    }

    @Test
    public void whenGettingInstanceOfExistingDomainObject_ShouldReturnObjectCreatedByFactory() {
        DomainObjectRepository sut = createRepository();

        ClassWithTwoProperties expectedDomainObject = mock(ClassWithTwoProperties.class);
        when(sut.factory.create(ClassWithTwoProperties.class)).thenReturn(expectedDomainObject);

        ClassWithTwoProperties actualDomainObject = sut.get(ClassWithTwoProperties.class, id(1));

        assertEquals(actualDomainObject, expectedDomainObject);
    }

    @Test
    public void whenGettingInstanceOfExistingDomainObject_ShouldReturnInitializedObject() {
        DomainObjectRepository sut = createRepository();

        ClassWithTwoProperties mockedDomainObject = mock(ClassWithTwoProperties.class);
        doAnswer(i -> when(mockedDomainObject.getId()).thenReturn(id(3)))
                .when(mockedDomainObject)
                .initializeById(id(3));

        when(sut.factory.create(ClassWithTwoProperties.class)).thenReturn(mockedDomainObject);

        ClassWithTwoProperties actualDomainObject = sut.get(ClassWithTwoProperties.class, id(3));

        assertEquals(actualDomainObject.getId(), id(3));
    }

    @Test
    public void whenGettingInstanceOfDomainObjectTwiceForTheSameId_ShouldReturnTheSameInstances() {
        DomainObjectRepository sut = createRepository();
        when(sut.factory.create(ClassWithoutProperties.class)).then(i -> mock(ClassWithoutProperties.class));

        ClassWithoutProperties firstGet = sut.get(ClassWithoutProperties.class, id(10));
        ClassWithoutProperties secondGet = sut.get(ClassWithoutProperties.class, id(10));

        assertTrue(firstGet == secondGet);
    }

    @Test
    public void whenGettingObjectsWithTheSameIdButDifferentType_ShouldReturnDifferentInstances() {
        DomainObjectRepository sut = createRepository();

        when(sut.factory.create(ClassWithoutProperties.class)).thenReturn(mock(ClassWithoutProperties.class));
        when(sut.factory.create(ClassWithTwoProperties.class)).thenReturn(mock(ClassWithTwoProperties.class));

        DomainObject firstGet = sut.get(ClassWithoutProperties.class, id(10));
        DomainObject secondGet = sut.get(ClassWithTwoProperties.class, id(10));

        assertFalse(firstGet == secondGet);
    }

    @Test
    public void whenGettingInstancesOfExistingDomainObjectsWithDifferentId_ShouldReturnDifferentInstances() {
        DomainObjectRepository sut = createRepository();
        when(sut.factory.create(ClassWithoutProperties.class)).then(i -> mock(ClassWithoutProperties.class));

        ClassWithoutProperties firstGet = sut.get(ClassWithoutProperties.class, id(10));
        ClassWithoutProperties secondGet = sut.get(ClassWithoutProperties.class, id(7));

        assertTrue(firstGet != secondGet);
    }

    @Test
    public void whenGettingDomainObjectsAlternatelyForDifferentId_ShouldReturnTheSameInstancesForTheSameId() {
        DomainObjectRepository sut = createRepository();
        ClassWithoutProperties obj1 = mock(ClassWithoutProperties.class);
        ClassWithoutProperties obj2 = mock(ClassWithoutProperties.class);

        when(sut.factory.create(ClassWithoutProperties.class))
                .thenReturn(obj1)
                .thenReturn(obj2);

        ClassWithoutProperties firstGetObj1 = sut.get(ClassWithoutProperties.class, id(10));
        ClassWithoutProperties firstGetObj2 = sut.get(ClassWithoutProperties.class, id(7));
        ClassWithoutProperties secondGetObj1 = sut.get(ClassWithoutProperties.class, id(10));
        ClassWithoutProperties secondGetObj2 = sut.get(ClassWithoutProperties.class, id(7));

        assertTrue(firstGetObj1 == secondGetObj1);
        assertTrue(firstGetObj2 == secondGetObj2);
    }

    @Test
    public void whenGettingDomainObjectsAlternatelyForDifferentTypes_ShouldReturnTheSameInstancesForTheSameType() {
        DomainObjectRepository sut = createRepository();

        when(sut.factory.create(ClassWithoutProperties.class)).thenAnswer(i -> mock(ClassWithoutProperties.class));
        when(sut.factory.create(ClassWithTwoProperties.class)).thenAnswer(i -> mock(ClassWithTwoProperties.class));

        Id anyId = id(66);
        ClassWithoutProperties firstGetObj1 = sut.get(ClassWithoutProperties.class, anyId);
        ClassWithTwoProperties firstGetObj2 = sut.get(ClassWithTwoProperties.class, anyId);
        ClassWithoutProperties secondGetObj1 = sut.get(ClassWithoutProperties.class, anyId);
        ClassWithTwoProperties secondGetObj2 = sut.get(ClassWithTwoProperties.class, anyId);

        assertTrue(firstGetObj1 == secondGetObj1);
        assertTrue(firstGetObj2 == secondGetObj2);
    }

    @Test(expectedExceptions = DomainObjectNotFoundException.class)
    public void whenGettingInstancesOfNonexistentDomainObject_ShouldThrow() {
        DomainObjectRepository sut = createRepository();
        ClassWithTwoProperties mockedDomainObject = mock(ClassWithTwoProperties.class);
        when(sut.factory.create(ClassWithTwoProperties.class)).thenReturn(mockedDomainObject);

        doThrow(VertexNotFoundException.class).when(mockedDomainObject).initializeById(id(1));

        sut.get(ClassWithTwoProperties.class, id(1));
    }

    @Test
    public void whenGettingInstancesOfNonexistentDomainObject_ShouldReturnNull() {
        DomainObjectRepository sut = createRepository();
        ClassWithTwoProperties mockedDomainObject = mock(ClassWithTwoProperties.class);
        when(sut.factory.create(ClassWithTwoProperties.class)).thenReturn(mockedDomainObject);

        doThrow(VertexNotFoundException.class).when(mockedDomainObject).initializeById(id(1));

        Optional<ClassWithTwoProperties> domainObject = sut.tryGet(ClassWithTwoProperties.class, id(1));
        assertFalse(domainObject.isPresent());
    }

    @Test
    public void whenGettingInstanceOfNonexistentDomainObjectTwice_ShouldReturnNullTwice() {
        DomainObjectRepository sut = createRepository();
        ClassWithTwoProperties mockedDomainObject = mock(ClassWithTwoProperties.class);
        when(sut.factory.create(ClassWithTwoProperties.class)).thenReturn(mockedDomainObject);

        doThrow(VertexNotFoundException.class).when(mockedDomainObject).initializeById(id(1));

        Optional<ClassWithTwoProperties> domainObject = sut.tryGet(ClassWithTwoProperties.class, id(1));
        assertFalse(domainObject.isPresent());

        Optional<ClassWithTwoProperties> domainObject2 = sut.tryGet(ClassWithTwoProperties.class, id(1));
        assertFalse(domainObject2.isPresent());
    }

    @Test(expectedExceptions = NullPointerException.class, expectedExceptionsMessageRegExp = ".*domainObjectClass.*")
    public void whenGettingInstanceOfNullClass_ShouldThrow() {
        DomainObjectRepository sut = createRepository();

        sut.get(null, id(33));
    }

    @Test(expectedExceptions = NullPointerException.class, expectedExceptionsMessageRegExp = ".*id.*")
    public void whenGettingInstanceOfNullId_ShouldThrow() {
        DomainObjectRepository sut = createRepository();
        sut.get(ClassWithTwoProperties.class, null);
    }

    @Test
    public void whenFindingByPropertyNoVertex_ShouldReturnNull() {
        DomainObjectRepository sut = createRepository();
        DomainObjectProperty property = stringProperty("any");
        Object propertyValue = mock(Object.class);
        when(sut.dbDriver.findByProperty(any(), any(), any())).thenReturn(emptyList());
        DomainObject single = sut.getSingleByProperty(DomainObject.class, property, propertyValue);
        assertNull(single);
    }

    @Test
    public void whenFindingByPropertyOneVertexFound_ShouldReturnDomainObject() {
        DomainObjectRepository sut = createRepository();
        DomainObjectProperty property = stringProperty("any");
        Object propertyValue = mock(Object.class);
        when(sut.dbDriver.findByProperty(any(), any(), any())).thenReturn(oneVertexList());
        when(sut.factory.create(any())).thenReturn(mock(DomainObject.class));
        DomainObject single = sut.getSingleByProperty(DomainObject.class, property, propertyValue);
        assertNotNull(single);
    }

    @Test(expectedExceptions = IllegalStateException.class)
    public void whenFindingByPropertyMultipleVertexFound_ShouldThrow() {
        DomainObjectRepository sut = createRepository();
        DomainObjectProperty property = stringProperty("any");
        Object propertyValue = mock(Object.class);
        when(sut.dbDriver.findByProperty(any(), any(), any())).thenReturn(multipleVertexList());
        sut.getSingleByProperty(DomainObject.class, property, propertyValue);
    }

    @Test(expectedExceptions = NullPointerException.class)
    public void whenFindingByPropertyOfNullClass_ShouldThrow() {
        DomainObjectRepository sut = createRepository();
        DomainObjectProperty property = stringProperty("any");
        Object propertyValue = mock(Object.class);
        sut.getSingleByProperty(null, property, propertyValue);
    }

    @Test(expectedExceptions = NullPointerException.class)
    public void whenFindingByPropertyOfNullProperty_ShouldThrow() {
        DomainObjectRepository sut = createRepository();
        Object propertyValue = mock(Object.class);
        sut.getSingleByProperty(DomainObject.class, null, propertyValue);
    }

    @Test(expectedExceptions = NullPointerException.class)
    public void whenFindingByPropertyOfNullPropertyValue_ShouldThrow() {
        DomainObjectRepository sut = createRepository();
        DomainObjectProperty property = stringProperty("any");
        sut.getSingleByProperty(DomainObject.class, property, null);
    }

    @Test
    public void whenDeletingDomainObject_ShouldInvokeDatabaseDriverDeleteVertex() {
        DomainObjectRepository sut = createRepository();
        ClassWithoutProperties classWithoutProperties = mock(ClassWithoutProperties.class);

        sut.delete(classWithoutProperties);

        verify(sut.dbDriver, times(1)).deleteVertex(any());
    }

    @Test(expectedExceptions = NullPointerException.class)
    public void whenDeletingNullDomainObject_ShouldThrow() {
        DomainObjectRepository sut = createRepository();
        sut.delete(null);
    }

    private DomainObjectRepository createRepository() {
        DomainObjectRepository sut = new DomainObjectRepository();
        sut.factory = mock(BeanFactory.class);
        sut.dbDriver = mock(DatabaseDriver.class);
        sut.hooks = mock(Instance.class);
        sut.logger = mock(Logger.class);
        return sut;
    }

    private List<Vertex> multipleVertexList() {
        return asList(mock(Vertex.class), mock(Vertex.class));
    }

    private List<Vertex> oneVertexList() {
        return asList(mock(Vertex.class));
    }
}
