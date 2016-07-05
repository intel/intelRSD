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

import com.intel.podm.config.DatabaseCleanupConfig;
import com.intel.podm.repository.api.GraphVertexType;
import com.orientechnologies.orient.core.db.document.ODatabaseDocumentTx;
import com.orientechnologies.orient.core.metadata.OMetadataDefault;
import com.orientechnologies.orient.core.metadata.schema.OClass;
import com.orientechnologies.orient.core.metadata.schema.OProperty;
import com.orientechnologies.orient.core.metadata.schema.OSchema;
import com.orientechnologies.orient.core.metadata.schema.OSchemaProxy;
import com.orientechnologies.orient.core.metadata.schema.OType;
import com.tinkerpop.blueprints.impls.orient.OrientExtendedGraph;
import com.tinkerpop.blueprints.impls.orient.OrientVertexType;
import org.testng.annotations.Test;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import static java.util.Collections.emptyMap;
import static org.mockito.Matchers.any;
import static org.mockito.Matchers.eq;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

public class GraphSchemaManagerImplTest {

    @Test(expectedExceptions = IllegalArgumentException.class, expectedExceptionsMessageRegExp = ".*vertexType.*")
    public void whenRegisteringVertexTypeAndPassingNull_ShouldThrow() {
        GraphSchemaManagerImpl graphSchemaManager = createGraphSchemaManager();
        graphSchemaManager.registerVertexType(null);
    }

    @Test
    public void whenRegisteringVertexType_ShouldCallCreateClassFromOSchema() {
        GraphSchemaManagerImpl graphSchemaManager = createGraphSchemaManager();
        GraphVertexType vertexType = new GraphVertexType("vertexTypeName", emptyMap());
        OSchema schema = getSchemaFor(graphSchemaManager);
        OClass superClass = getVertexSuperClass(schema);

        graphSchemaManager.registerVertexType(vertexType);
        verify(schema).createClass(eq(vertexType.getVertexTypeName()), eq(superClass));
    }

    @Test
    public void whenRegisteringVertexTypeAndDbCleanupRequested_ShouldCallTruncate() {
        GraphSchemaManagerImpl graphSchemaManager = createGraphSchemaManager();
        when(graphSchemaManager.databaseCleanupConfig.isDbCleanupRequested()).thenReturn(true);

        String vertexTypeName = "vertexTypeName";
        GraphVertexType vertexType = new GraphVertexType(vertexTypeName, emptyMap());

        OSchema schema = getSchemaFor(graphSchemaManager);
        OClass oClass = mockOClass(graphSchemaManager, vertexType.getVertexTypeName());
        when(schema.getClass(vertexType.getVertexTypeName())).thenReturn(oClass);

        graphSchemaManager.registerVertexType(vertexType);
        verify(graphSchemaManager.orientGraphDataRemover, times(1)).truncateOrientVertexType(vertexTypeName);
    }

    @Test
    public void whenRegisteringVertexTypeAndDbCleanupNotRequested_ShouldNotCallTruncate() {
        GraphSchemaManagerImpl graphSchemaManager = createGraphSchemaManager();
        when(graphSchemaManager.databaseCleanupConfig.isDbCleanupRequested()).thenReturn(false);

        String vertexTypeName = "vertexTypeName";
        GraphVertexType vertexType = new GraphVertexType(vertexTypeName, emptyMap());

        OSchema schema = getSchemaFor(graphSchemaManager);
        OClass oClass = mockOClass(graphSchemaManager, vertexType.getVertexTypeName());
        when(schema.getClass(vertexType.getVertexTypeName())).thenReturn(oClass);

        graphSchemaManager.registerVertexType(vertexType);
        verify(graphSchemaManager.orientGraphDataRemover, times(0)).truncateOrientVertexType(vertexTypeName);
    }

    @Test
    public void whenRegisteringTheSameVertexTypeTwice_ShouldCreateVertexTypeOnlyOnce() {
        GraphSchemaManagerImpl graphSchemaManager = createGraphSchemaManager();
        GraphVertexType vertexType = new GraphVertexType("vertexTypeName", emptyMap());
        OSchema schema = getSchemaFor(graphSchemaManager);

        graphSchemaManager.registerVertexType(vertexType);
        when(schema.getClass(vertexType.getVertexTypeName())).thenReturn(mock(OClass.class));
        graphSchemaManager.registerVertexType(vertexType);

        OClass superClass = getVertexSuperClass(schema);
        verify(schema, times(1)).createClass(eq(vertexType.getVertexTypeName()), eq(superClass));
    }

    @Test
    public void whenRegisteringTwoDifferentVertexTypesTwice_ShouldCallCreateClassFromOSchemaOnceForEach() {
        GraphSchemaManagerImpl graphSchemaManager = createGraphSchemaManager();
        GraphVertexType vertexType = new GraphVertexType("vertexTypeName", emptyMap());
        GraphVertexType anotherVertexType = new GraphVertexType("anotherVertexTypeName", emptyMap());
        OSchema schema = getSchemaFor(graphSchemaManager);
        OClass superClass = getVertexSuperClass(schema);

        graphSchemaManager.registerVertexType(vertexType);
        graphSchemaManager.registerVertexType(anotherVertexType);

        verify(schema, times(1)).createClass(eq(vertexType.getVertexTypeName()), eq(superClass));
        verify(schema, times(1)).createClass(eq(anotherVertexType.getVertexTypeName()), eq(superClass));
    }

    @Test
    public void whenRegisteringVertexWithoutProperties_ShouldNotCallCreatePropertyFromOrientGraphVertexType() {
        GraphSchemaManagerImpl graphSchemaManager = createGraphSchemaManager();

        GraphVertexType vertexType = new GraphVertexType("vertexTypeName", emptyMap());
        OClass mockedVertexClass = mockOClass(graphSchemaManager, vertexType.getVertexTypeName());

        graphSchemaManager.registerVertexType(vertexType);

        verify(mockedVertexClass, never()).createProperty(any(), any());
    }

    @Test
    public void whenRegisteringVertexWithSingleProperty_ShouldCallCreatePropertyFromOrientGraphVertexTypeOnce() {
        GraphSchemaManagerImpl graphSchemaManager = createGraphSchemaManager();

        Map<String, Class> properties = new HashMap<>();
        properties.put("test1", String.class);
        GraphVertexType vertexType = new GraphVertexType("vertexTypeName", properties);

        OClass mockedVertex = mockOClass(graphSchemaManager, vertexType.getVertexTypeName());
        graphSchemaManager.registerVertexType(vertexType);

        verify(mockedVertex, times(1)).createProperty("test1", OType.STRING);
    }

    @Test
    public void whenRegisteringVertexWithMoreThanOneProperty_ShouldCallCreatePropertyFromOrientGraphVertexTypeForEachProperty() {
        GraphSchemaManagerImpl graphSchemaManager = createGraphSchemaManager();

        Map<String, Class> properties = new HashMap<>();
        properties.put("test1", String.class);
        properties.put("test2", String.class);
        GraphVertexType vertexType = new GraphVertexType("vertexTypeName", properties);

        OClass mockedVertex = mockOClass(graphSchemaManager, vertexType.getVertexTypeName());

        graphSchemaManager.registerVertexType(vertexType);

        verify(mockedVertex, times(2)).createProperty(any(), any());
    }

    @Test
    public void whenRegisteringVertexWithMultipleProperties_ShouldCreateAllPropertiesWithProperNameAndType() {
        GraphSchemaManagerImpl graphSchemaManager = createGraphSchemaManager();

        Map<String, Class> properties = new HashMap<String, Class>() { {
            put("test1", String.class);
            put("40", Long.class);
            put("1", Integer.class);
        } };
        GraphVertexType vertexType = new GraphVertexType("vertexTypeName", properties);

        OClass mockedVertex = mockOClass(graphSchemaManager, vertexType.getVertexTypeName());

        graphSchemaManager.registerVertexType(vertexType);

        verify(mockedVertex).createProperty("test1", OType.STRING);
        verify(mockedVertex).createProperty("40", OType.LONG);
        verify(mockedVertex).createProperty("1", OType.INTEGER);
    }

    @Test
    public void whenRegisteringVertexType_ShouldCheckIfVertexTypeIsDefinedInOrientGraph() {
        GraphSchemaManagerImpl graphSchemaManager = createGraphSchemaManager();
        GraphVertexType vertexType = new GraphVertexType("vertexTypeName", emptyMap());

        graphSchemaManager.registerVertexType(vertexType);
        verify(getSchemaFor(graphSchemaManager), times(1)).getClass(vertexType.getVertexTypeName());
    }

    @Test
    public void whenRegisteringVertexTypeWithoutProperties_ShouldNotCreateOrientVertexTypeIfTypeAlreadyExists() {
        GraphSchemaManagerImpl graphSchemaManager = createGraphSchemaManager();
        GraphVertexType vertexType = new GraphVertexType("vertexTypeName", emptyMap());
        OSchema schema = getSchemaFor(graphSchemaManager);
        OClass superClass = getVertexSuperClass(schema);

        OClass oClass = mockOClass(graphSchemaManager, vertexType.getVertexTypeName());
        when(schema.getClass(vertexType.getVertexTypeName())).thenReturn(oClass);

        graphSchemaManager.registerVertexType(vertexType);

        verify(schema, never()).createClass(vertexType.getVertexTypeName(), superClass);
    }

    @Test
    public void whenRegisteringExistingVertexTypeWithProperties_ShouldCreateAllMissingProperties() {
        OClass mockedVertexTypeLessProperties = mock(OClass.class);

        List<OProperty> lessOProperties = new ArrayList<>();
        lessOProperties.add(createOProperty("test1", String.class));
        lessOProperties.add(createOProperty("1", Integer.class));

        when(mockedVertexTypeLessProperties.declaredProperties()).thenReturn(lessOProperties);

        Map<String, Class> properties = new HashMap<String, Class>() { {
            put("test1", String.class);
            put("40", Long.class);
            put("1", Integer.class);
        } };
        GraphVertexType vertexType = new GraphVertexType("vertexTypeName", properties);

        GraphSchemaManagerImpl graphSchemaManager = createGraphSchemaManager();

        when(getSchemaFor(graphSchemaManager).getClass(vertexType.getVertexTypeName())).thenReturn(mockedVertexTypeLessProperties);

        graphSchemaManager.registerVertexType(vertexType);

        verify(mockedVertexTypeLessProperties).createProperty("40", OType.LONG);
    }

    @Test
    public void whenRegisteringExistingVertexTypeWithProperties_ShouldDropAllObsoleteProperties() {
        List<OProperty> moreOProperties = new ArrayList<>();
        moreOProperties.add(createOProperty("test1", String.class));
        moreOProperties.add(createOProperty("40", Long.class));
        moreOProperties.add(createOProperty("1", Integer.class));

        OClass mockedVertexTypeMoreProperties = mock(OClass.class);
        when(mockedVertexTypeMoreProperties.declaredProperties()).thenReturn(moreOProperties);

        Map<String, Class> properties = new HashMap<String, Class>() { {
            put("test1", String.class);
            put("1", Integer.class);
        } };
        GraphVertexType vertexType = new GraphVertexType("vertexTypeName", properties);

        GraphSchemaManagerImpl graphSchemaManager = createGraphSchemaManager();
        when(getSchemaFor(graphSchemaManager).getClass(vertexType.getVertexTypeName())).thenReturn(mockedVertexTypeMoreProperties);

        graphSchemaManager.registerVertexType(vertexType);

        verify(mockedVertexTypeMoreProperties).dropProperty("40");
    }

    @Test
    public void whenRegisteringExistingVertexTypeWithPropertyWithExistingNameAndDifferentType_ShouldDropAndCreateProperty() {
        GraphSchemaManagerImpl graphSchemaManager = createGraphSchemaManager();

        Map<String, Class> properties = new HashMap<String, Class>() { {
            put("test1", String.class);
        } };
        GraphVertexType vertexType = new GraphVertexType("vertexTypeName", properties);

        OrientVertexType vertexTypeWithDifferentPropertyType = mock(OrientVertexType.class);

        List<OProperty> oPropertyWithDifferentPropertyType = new ArrayList<>();
        oPropertyWithDifferentPropertyType.add(createOProperty("test1", Integer.class));

        when(vertexTypeWithDifferentPropertyType.declaredProperties()).thenReturn(oPropertyWithDifferentPropertyType);

        when(getSchemaFor(graphSchemaManager).getClass(vertexType.getVertexTypeName())).thenReturn(vertexTypeWithDifferentPropertyType);

        graphSchemaManager.registerVertexType(vertexType);

        verify(vertexTypeWithDifferentPropertyType).createProperty("test1", OType.STRING);
        verify(vertexTypeWithDifferentPropertyType).dropProperty("test1");
    }

    private GraphSchemaManagerImpl createGraphSchemaManager() {
        GraphSchemaManagerImpl graphSchemaManager = new GraphSchemaManagerImpl();
        graphSchemaManager.orientGraph = mock(OrientExtendedGraph.class);
        when(graphSchemaManager.orientGraph.getRawGraph()).thenReturn(mock(ODatabaseDocumentTx.class));
        when(graphSchemaManager.orientGraph.getRawGraph().getMetadata()).thenReturn(mock(OMetadataDefault.class));
        when(graphSchemaManager.orientGraph.getRawGraph().getMetadata().getSchema()).thenReturn(mock(OSchemaProxy.class));
        when(graphSchemaManager.orientGraph.getRawGraph().getMetadata().getSchema().getClass("V")).thenReturn(mock(OClass.class));
        graphSchemaManager.databaseCleanupConfig = mock(DatabaseCleanupConfig.class);
        graphSchemaManager.orientGraphDataRemover = mock(OrientGraphDataRemover.class);
        return graphSchemaManager;
    }

    private OClass mockOClass(GraphSchemaManagerImpl graphSchemaManager, String vertexTypeName) {
        OClass mockedOClass = mock(OClass.class);
        OSchema schema = getSchemaFor(graphSchemaManager);
        OClass superClass = getVertexSuperClass(schema);
        when(schema.createClass(vertexTypeName, superClass))
                .then(a -> {
                    when(schema.getClass(vertexTypeName)).thenReturn(mockedOClass);
                    return mockedOClass;
                });

        return mockedOClass;
    }

    private OProperty createOProperty(String name, Class clazz) {
        OProperty oProperty = mock(OProperty.class);
        OType oType = OType.getTypeByClass(clazz);

        when(oProperty.getName()).thenReturn(name);
        when(oProperty.getType()).thenReturn(oType);

        return oProperty;
    }

    private OSchema getSchemaFor(GraphSchemaManagerImpl manager) {
        return manager.orientGraph.getRawGraph().getMetadata().getSchema();
    }

    private OClass getVertexSuperClass(OSchema schema) {
        return schema.getClass("V");
    }
}
