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

import com.intel.podm.repositories.orientdb.SingleRelationSubgraphImpl;
import com.intel.podm.repositories.orientdb.graph.annotation.PodGraph;
import com.intel.podm.repository.api.DatabaseDriver;
import com.intel.podm.repository.api.SingleRelationSubgraph;
import com.intel.podm.repository.api.VertexNotFoundException;
import com.orientechnologies.orient.core.id.ORecordId;
import com.tinkerpop.blueprints.Edge;
import com.tinkerpop.blueprints.Vertex;
import com.tinkerpop.blueprints.impls.orient.OrientGraph;
import org.ops4j.orient.adapter.api.OrientDatabaseConnection;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;
import java.util.Optional;

import static com.tinkerpop.blueprints.Direction.IN;
import static com.tinkerpop.blueprints.Direction.OUT;
import static java.util.stream.Collectors.toList;
import static java.util.stream.StreamSupport.stream;

/**
 * Driver for OrientDb database
 */
@Dependent
public class OrientDbDriver implements DatabaseDriver, Serializable {

    private static final String CLASS_PREFIX = "class:";
    private static final String CLASS_FIELD_NAME = "@class";

    @Inject
    @PodGraph(transactional = true)
    private OrientDatabaseConnection conn;

    @Override
    public Vertex addVertex(String className) {
        OrientGraph graph = conn.ograph();
        return graph.addVertex(CLASS_PREFIX + className);
    }

    private Vertex getVertex(Object recordId) {
        return conn.ograph().getVertex(recordId);
    }

    @Override
    public Vertex getVertex(String className, Long id) {
        long clusterPosition = id - 1;
        Vertex vertex = getVertex(new ORecordId(getClusterId(className, clusterPosition), clusterPosition));
        if (vertex == null) {
            throw new VertexNotFoundException(className, id);
        }
        return vertex;
    }

    private int getClusterId(String className, long clusterPosition) {
        if (clusterPosition < 0) {
            return -1;
        }

        OrientGraph graph = conn.ograph();
        int[] clusterIds = graph.getVertexType(className).getClusterIds();
        if (clusterIds.length > 1) {
            throw new RuntimeException("Ambiguous clusters for " + className);
        }

        return clusterIds[0];
    }

    @Override
    public Long getId(Object recordId) {
        return ((ORecordId) recordId).getClusterPosition() + 1;
    }

    @Override
    public String getVertexClass(Vertex vertex) {
        return vertex.getProperty(CLASS_FIELD_NAME);
    }

    @Override
    public List<Vertex> getVerticesOfClass(String className) {
        OrientGraph graph = conn.ograph();
        final List<Vertex> vertices = new ArrayList<>();
        graph.getVerticesOfClass(className).forEach(vertices::add);
        return vertices;
    }

    @Override
    public List<Vertex> findByProperty(String className, String propertyName, Object propertyValue) {
        OrientGraph graph = conn.ograph();

        return stream(graph.getVerticesOfClass(className).spliterator(), false)
                .filter(vertex -> Objects.equals(vertex.getProperty(propertyName), propertyValue))
                .collect(toList());
    }

    @Override
    public List<Vertex> findByFlag(String className, String propertyName, boolean value) {
        List<Vertex> vertices = getVerticesOfClass(className);
        List<Vertex> flagged = new ArrayList<>();
        for (Vertex vertex : vertices) {
            Boolean flag = vertex.getProperty(propertyName);
            boolean flagValue = (flag != null) ? flag : false;
            if (flagValue == value) {
                flagged.add(vertex);
            }
        }
        return flagged;
    }

    @Override
    public void deleteVertex(Vertex vertex) {
        conn.ograph().removeVertex(vertex);
    }

    @Override
    public SingleRelationSubgraph linkVertices(Vertex sourceVertex, Vertex targetVertex, String label) {
        for (Vertex outVertex : sourceVertex.getVertices(OUT, label)) {
            if (Objects.equals(outVertex.getId(), targetVertex.getId())) {
                Optional<Edge> edge = getEdge(sourceVertex, targetVertex, label);
                if (edge.isPresent()) {
                    return new SingleRelationSubgraphImpl(sourceVertex, targetVertex, edge.get());
                }

                throw new IllegalStateException("Couldn't find existing edge with label + " + label + " between " + sourceVertex + " and " + targetVertex);
            }
        }

        Edge edge = sourceVertex.addEdge(label, targetVertex);
        return new SingleRelationSubgraphImpl(sourceVertex, targetVertex, edge);
    }

    @Override
    public void unlinkVertices(Vertex sourceVertex, Vertex targetVertex, String label) {
        stream(sourceVertex.getVertices(OUT, label).spliterator(), false)
                .filter(outVertex -> Objects.equals(outVertex.getId(), targetVertex.getId()))
                .map(outVertex -> getEdge(sourceVertex, outVertex, label))
                .filter(Optional::isPresent)
                .forEach(edge -> deleteEdge(edge.get()));
    }

    private Optional<Edge> getEdge(Vertex sourceVertex, Vertex targetVertex, String label) {
        return stream(sourceVertex.getEdges(OUT, label).spliterator(), false)
                .filter(edge -> Objects.equals(edge.getVertex(IN).getId(), targetVertex.getId()))
                .findFirst();
    }

    @Override
    public void deleteEdge(Edge edge) {
        conn.ograph().removeEdge(edge);
    }
}
