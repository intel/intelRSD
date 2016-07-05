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

import com.intel.podm.business.entities.dao.GenericDao;
import com.intel.podm.business.entities.ogm.OgmManager;
import com.intel.podm.repository.api.DatabaseDriver;
import com.intel.podm.repository.api.SingleRelationSubgraph;
import com.tinkerpop.blueprints.Vertex;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import java.util.ArrayList;
import java.util.List;

import static com.intel.podm.common.types.Id.id;
import static com.tinkerpop.blueprints.Direction.IN;
import static java.util.Collections.unmodifiableList;

@Dependent
final class DomainObjectLinkSupporter {

    @Inject
    private DatabaseDriver dbDriver;

    @Inject
    private GenericDao genericDao;

    @Inject
    private OgmManager ogmManager;

    protected DomainObjectsRelation convertFromVertexRelation(SingleRelationSubgraph singleRelationSubgraph) {
        DomainObject source = genericDao.tryFind(singleRelationSubgraph.getSource()).orElse(null);
        DomainObject target = genericDao.tryFind(singleRelationSubgraph.getTarget()).orElse(null);
        return new DomainObjectsRelation(source, target, () -> singleRelationSubgraph.getEdge().getLabel());
    }

    protected DomainObjectsRelation link(DomainObject source, Link link, DomainObject target) {
        if (link.getDirection() == IN) {
            return convertFromVertexRelation(dbDriver.linkVertices(target.getVertex(), source.getVertex(), link.getLabel()));
        } else {
            return convertFromVertexRelation(dbDriver.linkVertices(source.getVertex(), target.getVertex(), link.getLabel()));
        }
    }

    protected void unlink(DomainObject source, Link link, DomainObject target) {
        if (link.getDirection() == IN) {
            dbDriver.unlinkVertices(target.getVertex(), source.getVertex(), link.getLabel());
        } else {
            dbDriver.unlinkVertices(source.getVertex(), target.getVertex(), link.getLabel());
        }
    }

    protected <T extends DomainObject> List<T> getLinked(DomainObject source, Link link) {
        List<T> linkedDomainObjects = new ArrayList<>();
        source.getVertex().getVertices(link.getDirection(), link.getLabel()).forEach(vertex -> appendDomainObject(linkedDomainObjects, vertex));
        return unmodifiableList(linkedDomainObjects);
    }

    protected <T extends DomainObject> List<T> getLinked(DomainObject source, Link link, Class<T> domainObjectClass) {
        List<T> linkedDomainObjects = new ArrayList<>();
        String vertexClassName = ogmManager.getVertexClassName(domainObjectClass);

        source.getVertex().getVertices(link.getDirection(), link.getLabel()).forEach(vertex -> {
            if (dbDriver.getVertexClass(vertex).equals(vertexClassName)) {
                appendDomainObject(linkedDomainObjects, vertex);
            }
        });

        return unmodifiableList(linkedDomainObjects);
    }

    private <T extends DomainObject> void appendDomainObject(List<T> domainObjects, Vertex vertex) {
        Class<T> domainObjectClass = (Class<T>) ogmManager.getDomainObjectClass(dbDriver.getVertexClass(vertex));
        domainObjects.add(genericDao.find(domainObjectClass, id(dbDriver.getId(vertex.getId()))));
    }
}
