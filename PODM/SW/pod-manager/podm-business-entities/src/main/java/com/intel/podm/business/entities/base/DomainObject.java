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
import com.intel.podm.common.types.Id;
import com.intel.podm.repository.api.DatabaseDriver;
import com.tinkerpop.blueprints.Vertex;

import javax.inject.Inject;
import java.util.List;

import static com.intel.podm.business.entities.base.DomainObjectProperties.booleanProperty;
import static com.intel.podm.common.enterprise.utils.proxy.Unproxier.unproxy;
import static com.intel.podm.common.types.Id.id;

/**
 * Domain object base class. Not to be instantiated (not abstract because of JEE limitations)
 */
@AbstractDomainObject
public class DomainObject {
    public static final DomainObjectProperty<Boolean> WAS_LAST_UPDATED_BY_DISCOVERY = booleanProperty("wasLastUpdatedByDiscovery");

    @Inject
    protected DatabaseDriver dbDriver;

    protected Vertex vertex;

    @Inject
    private DomainObjectLinkSupporter linkSupporter;

    @Inject
    private GenericDao genericDao;

    public Boolean getWasLastUpdatedByDiscovery() {
        return getProperty(WAS_LAST_UPDATED_BY_DISCOVERY);
    }

    public void setWasLastUpdatedByDiscovery(Boolean lastUpdatedByDiscovery) {
        setProperty(WAS_LAST_UPDATED_BY_DISCOVERY, lastUpdatedByDiscovery);
    }

    void initializeAsNewInstance() {
        vertex = dbDriver.addVertex(getVertexClassName());
    }

    void initializeById(Id id) {
        vertex = dbDriver.getVertex(getVertexClassName(), id.getValue());
    }

    public Id getId() {
        return id(dbDriver.getId(vertex.getId()));
    }

    protected <T extends DomainObject> T addDomainObject(Link link, Class<T> domainObjectClass) {
        T domainObject = genericDao.create(domainObjectClass);

        link(link, domainObject);
        return domainObject;
    }

    protected void setLink(Link link, DomainObject target) {
        for (DomainObject domainObject : getLinked(link)) {
            unlink(link, domainObject);
        }

        link(link, target);
    }

    protected DomainObjectsRelation link(Link link, DomainObject target) {
        return linkSupporter.link(this, link, target);
    }

    protected void unlink(Link link, DomainObject target) {
        linkSupporter.unlink(this, link, target);
    }

    protected <T extends DomainObject> List<T> getLinked(Link link) {
        return linkSupporter.getLinked(this, link);
    }

    protected <T extends DomainObject> List<T> getLinked(Link link, Class<T> domainObjectClass) {
        return linkSupporter.getLinked(this, link, domainObjectClass);
    }

    protected <S extends T, T> S getProperty(DomainObjectProperty<? extends T> property) {
        Object value = vertex.getProperty(property.getName());
        return (S) property.convertToMappedType(value);
    }

    /**
     * Method stores a single domain object property.
     * If the value passed is null, the property is removed.
     *
     * @param property domain object property definition
     * @param value value to be set for the property
     * @param <T> type of value passed
     */
    protected <T> void setProperty(DomainObjectProperty<? extends T> property, T value) {
        if (value != null) {
            Object convertedValue = property.convertFromMappedType(value);
            vertex.setProperty(property.getName(), convertedValue);
        } else {
            removeProperty(property);
        }
    }

    protected <T> T removeProperty(DomainObjectProperty<T> property) {
        return vertex.removeProperty(property.getName());
    }

    protected Vertex getVertex() {
        return vertex;
    }

    protected String getVertexClassName() {
        return unproxy(this.getClass()).getSimpleName();
    }

    @Override
    public String toString() {
        return "DomainObject{" + "vertex=" + vertex + '}';
    }
}
