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

import com.tinkerpop.blueprints.Edge;

import java.util.Objects;

import static com.intel.podm.business.entities.base.DomainObjectLink.CONTAINED_BY;
import static java.util.stream.StreamSupport.stream;

public class DomainObjectsRelationsChecker {

    private final DomainObject domainObject;

    protected DomainObjectsRelationsChecker(DomainObject domainObject) {
        this.domainObject = domainObject;
    }

    public boolean containedBy(DomainObject other) {
        Iterable<Edge> edges = domainObject.getVertex().getEdges(CONTAINED_BY.getDirection(), CONTAINED_BY.getLabel());
        return stream(edges.spliterator(), false)
                .map(edge -> edge.getVertex(CONTAINED_BY.getDirection().opposite()))
                .anyMatch(vertex -> Objects.equals(vertex, other.getVertex()));
    }

    public static DomainObjectsRelationsChecker is(DomainObject domainObject) {
        return new DomainObjectsRelationsChecker(domainObject);
    }
}
