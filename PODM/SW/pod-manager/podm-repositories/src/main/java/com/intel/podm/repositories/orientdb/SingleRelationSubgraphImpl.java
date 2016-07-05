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

package com.intel.podm.repositories.orientdb;

import com.intel.podm.repository.api.SingleRelationSubgraph;
import com.tinkerpop.blueprints.Edge;
import com.tinkerpop.blueprints.Vertex;

import java.util.Objects;

import static com.google.common.base.Preconditions.checkNotNull;
import static java.util.Objects.hash;

public class SingleRelationSubgraphImpl implements SingleRelationSubgraph {
    private final Vertex source;
    private final Vertex target;
    private final Edge edge;

    public SingleRelationSubgraphImpl(Vertex source, Vertex target, Edge edge) {
        this.source = checkNotNull(source, "Source vertex mustn't be null");
        this.target = checkNotNull(target, "Target vertex mustn't be null");
        this.edge = checkNotNull(edge, "Edge between two vertices mustn't be null");
    }

    @Override
    public Vertex getSource() {
        return source;
    }

    @Override
    public Vertex getTarget() {
        return target;
    }

    @Override
    public Edge getEdge() {
        return edge;
    }

    @Override
    public boolean equals(Object o) {
        if (!(o instanceof SingleRelationSubgraphImpl)) {
            return false;
        }

        SingleRelationSubgraphImpl that = (SingleRelationSubgraphImpl) o;
        return Objects.equals(getSource(), that.getSource())
                && Objects.equals(getTarget(), that.getTarget())
                && Objects.equals(getEdge(), that.getEdge());
    }

    @Override
    public int hashCode() {
        return hash(getSource(), getTarget(), getEdge());
    }
}
