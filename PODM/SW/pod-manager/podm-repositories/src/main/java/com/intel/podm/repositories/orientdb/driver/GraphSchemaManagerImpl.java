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
import com.intel.podm.repository.api.GraphSchemaManager;
import com.intel.podm.repository.api.GraphVertexType;
import com.orientechnologies.orient.core.metadata.schema.OClass;
import com.orientechnologies.orient.core.metadata.schema.OProperty;
import com.orientechnologies.orient.core.metadata.schema.OSchema;
import com.orientechnologies.orient.core.metadata.schema.OType;
import com.tinkerpop.blueprints.impls.orient.OrientExtendedGraph;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;

import static com.google.common.collect.Sets.difference;

@Dependent
public class GraphSchemaManagerImpl implements GraphSchemaManager {

    @Inject
    OrientExtendedGraph orientGraph;

    @Inject
    OrientGraphDataRemover orientGraphDataRemover;

    @Inject
    DatabaseCleanupConfig databaseCleanupConfig;

    @Override
    public void registerVertexType(GraphVertexType vertexType) {
        if (vertexType == null) {
            throw new IllegalArgumentException("Argument vertexType should not be null!");
        }

        OSchema schema = orientGraph.getRawGraph().getMetadata().getSchema();
        OClass vertexClass = schema.getClass(vertexType.getVertexTypeName());

        if (vertexClass == null) {
            createVertexClass(schema, vertexType);
        } else {
            updateOrientClass(vertexClass, vertexType);

            if (databaseCleanupConfig.isDbCleanupRequested()) {
                orientGraphDataRemover.truncateOrientVertexType(vertexType.getVertexTypeName());
            }
        }
    }

    @Override
    public void registerEdgeType(String edgeLabel) {
        OSchema schema = orientGraph.getRawGraph().getMetadata().getSchema();
        OClass edgeClass = schema.getClass(edgeLabel);
        if (edgeClass == null) {
            orientGraph.createEdgeType(edgeLabel);
        }
    }

    private void updateOrientClass(OClass vertexClass, GraphVertexType vertexType) {
        Map<String, Class> orientPropertiesMap = orientPropertiesToMap(vertexClass.declaredProperties());
        Map<String, Class> vertexPropertiesMap = vertexType.getVertexProperties();

        Iterable<Map.Entry<String, Class>> removedOrChangedProperties = difference(orientPropertiesMap.entrySet(), vertexPropertiesMap.entrySet());
        Iterable<Map.Entry<String, Class>> addedProperties = difference(vertexPropertiesMap.entrySet(), orientPropertiesMap.entrySet());

        removeMultipleProperties(vertexClass, removedOrChangedProperties);
        addMultipleProperties(vertexClass, addedProperties);
    }

    private void removeMultipleProperties(OClass vertexClass, Iterable<Map.Entry<String, Class>> removedProperties) {
        for (Map.Entry<String, Class> removedProperty : removedProperties) {
            vertexClass.dropProperty(removedProperty.getKey());
        }
    }

    private void addMultipleProperties(OClass vertexClass, Iterable<Map.Entry<String, Class>> newProperties) {
        for (Map.Entry<String, Class> newProperty : newProperties) {
            vertexClass.createProperty(newProperty.getKey(), OType.getTypeByClass(newProperty.getValue()));
        }
    }

    private void createVertexClass(OSchema schema, GraphVertexType vertexType) {
        OClass vertexSuperClass = schema.getClass("V");
        OClass vertexClass = schema.createClass(vertexType.getVertexTypeName(), vertexSuperClass);

        if (vertexType.getVertexProperties() != null) {
            addMultipleProperties(vertexClass, vertexType.getVertexProperties().entrySet());
        }
    }

    private Map<String, Class> orientPropertiesToMap(Collection<OProperty> properties) {
        HashMap<String, Class> propertiesMap = new HashMap<>();

        for (OProperty property : properties) {
            propertiesMap.put(property.getName(), property.getType().getDefaultJavaType());
        }

        return propertiesMap;
    }
}
