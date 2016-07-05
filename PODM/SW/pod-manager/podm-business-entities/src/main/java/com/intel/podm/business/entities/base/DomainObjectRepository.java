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

import com.google.common.collect.HashBasedTable;
import com.intel.podm.business.entities.DomainObjectNotFoundException;
import com.intel.podm.business.entities.hooks.OnDeleteHook;
import com.intel.podm.business.entities.ogm.OgmManager;
import com.intel.podm.common.enterprise.utils.beans.BeanFactory;
import com.intel.podm.common.logger.Logger;
import com.intel.podm.common.types.Id;
import com.intel.podm.repository.api.DatabaseDriver;
import com.intel.podm.repository.api.VertexNotFoundException;
import com.tinkerpop.blueprints.Vertex;

import javax.enterprise.inject.Instance;
import javax.inject.Inject;
import javax.transaction.TransactionScoped;
import javax.transaction.Transactional;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;

import static com.google.common.base.Preconditions.checkNotNull;
import static com.intel.podm.common.enterprise.utils.proxy.Unproxier.unproxy;
import static com.intel.podm.common.types.Id.id;
import static com.intel.podm.common.utils.IterableHelper.singleOrNull;
import static java.util.Optional.empty;
import static java.util.Optional.of;
import static javax.transaction.Transactional.TxType.MANDATORY;

/**
 * Allows to obtain or create instance of domain objects in state associated with current transaction
 */
@TransactionScoped
@Transactional(MANDATORY)
public class DomainObjectRepository implements Serializable {

    @Inject
    transient Logger logger;

    @Inject
    DatabaseDriver dbDriver;

    @Inject
    BeanFactory factory;

    @Inject
    OgmManager ogmManager;

    @Inject
    Instance<OnDeleteHook> hooks;

    private final HashBasedTable<Id, Class<? extends DomainObject>, DomainObject> cachedObjects = HashBasedTable.create();

    public <T extends DomainObject> T create(Class<T> domainObjectClass) {
        checkNotNull(domainObjectClass, "domainObjectClass should not be null");

        T domainObject = factory.create(domainObjectClass);
        domainObject.initializeAsNewInstance();
        return domainObject;
    }

    /**
     * Gets domain object instance by its vertex.
     * For given vertex it always returns the same reference.
     * @param vertex
     * @return Domain object instance optional
     */
    public <T extends DomainObject> Optional<T> tryGet(Vertex vertex) {
        checkNotNull(vertex, "Vertex should not be null");

        Class<T> clazz = (Class<T>) ogmManager.getDomainObjectClass(dbDriver.getVertexClass(vertex));
        Id id = id(dbDriver.getId(vertex.getId()));
        return tryGet(clazz, id);
    }

    /**
     * Gets domain object instance by its class and id.
     * For given domain object class and id it always returns the same reference.
     * @param domainObjectClass class of domain object
     * @param id Id of domain object
     * @return Domain object instance optional
     */
    public <T extends DomainObject> Optional<T> tryGet(Class<T> domainObjectClass, Id id) {
        checkNotNull(domainObjectClass, "domainObjectClass should not be null");
        checkNotNull(id, "id should not be null");

        if (cachedObjects.contains(id, domainObjectClass)) {
            return of((T) cachedObjects.get(id, domainObjectClass));
        }

        T domainObject = factory.create(domainObjectClass);
        try {
            domainObject.initializeById(id);
        } catch (VertexNotFoundException e) {
            return empty();
        }

        cachedObjects.put(id, domainObjectClass, domainObject);

        return of(domainObject);
    }

    /**
     * Gets domain object instance by its class and id.
     * For given domain object class and id it always returns the same reference.
     * @param domainObjectClass class of domain object
     * @param id Id of domain object
     * @return Domain object instance or throws DomainObjectNotFoundException if not found
     */
    public <T extends DomainObject> T get(Class<T> domainObjectClass, Id id) {
        Optional<T> domainObject = tryGet(domainObjectClass, id);
        return domainObject.orElseThrow(() -> new DomainObjectNotFoundException(domainObjectClass, id));
    }

    /**
     * Method gets all domain objects of given class
     *
     * @param domainObjectClass class of Domain Object
     * @return List of initialized Domain Objects
     */
    public <T extends DomainObject> List<T> getAll(Class<T> domainObjectClass) {
        checkNotNull(domainObjectClass, "domainObjectClass should not be null");

        List<Vertex> vertices = dbDriver.getVerticesOfClass(domainObjectClass.getSimpleName());
        return convertVerticesToDomainObjects(domainObjectClass, vertices);
    }

    /**
     * Finds single domain object by given property.
     *
     * @param domainObjectClass type of domain object
     * @param domainObjectProperty property to search by
     * @param propertyValue property value to search by
     * @return matching domain object or null if not found
     * @throws IllegalStateException if more then one match found
     */
    public <T extends DomainObject, U> T getSingleByProperty(
            Class<T> domainObjectClass, DomainObjectProperty<? extends U> domainObjectProperty, U propertyValue) {

        List<Vertex> vertices = getVerticesOfClassByProperty(domainObjectClass, domainObjectProperty, propertyValue);
        Vertex vertex = singleOrNull(vertices);
        if (vertex == null) {
            return null;
        }
        Long id = dbDriver.getId(vertex.getId());
        return get(domainObjectClass, id(id));
    }

    private <T extends DomainObject, U> List<Vertex> getVerticesOfClassByProperty(Class<T> domainObjectClass,
                                                                                  DomainObjectProperty<? extends U> domainObjectProperty,
                                                                                  U propertyValue) {
        checkNotNull(domainObjectClass, "domainObjectClass should not be null");
        checkNotNull(domainObjectProperty, "domainObjectProperty should not be null");
        checkNotNull(propertyValue, "propertyValue should not be null");

        Object value = domainObjectProperty.convertFromMappedType(propertyValue);
        return dbDriver.findByProperty(domainObjectClass.getSimpleName(), domainObjectProperty.getName(), value);
    }

    /**
     * Finds all domain objects by given property
     *
     * @param domainObjectClass type of domain object
     * @param domainObjectProperty property to search by
     * @param propertyValue property value to search by
     * @return matching domain objects or empty list if not found
     */
    public <T extends DomainObject, U> List<T> getAllByProperty(Class<T> domainObjectClass,
                                                                DomainObjectProperty<? extends U> domainObjectProperty,
                                                                U propertyValue) {

        List<Vertex> vertices = getVerticesOfClassByProperty(domainObjectClass, domainObjectProperty, propertyValue);
        return convertVerticesToDomainObjects(domainObjectClass, vertices);
    }

    /**
     * Finds all domain objects using boolean flag. DomainObject without this flag set are treated as having false flag value.
     *
     * @param domainObjectClass type of domain object
     * @param domainObjectFlag boolean property to search by
     * @param value boolean property value to search by
     * @return List of initialized Domain Objects
     */
    public <T extends DomainObject> List<T> getAllByFlag(Class<T> domainObjectClass, DomainObjectProperty<Boolean> domainObjectFlag, boolean value) {
        checkNotNull(domainObjectClass, "domainObjectClass should not be null");
        checkNotNull(domainObjectFlag, "domainObjectProperty should not be null");
        checkNotNull(value, "propertyValue should not be null");

        List<Vertex> vertices = dbDriver.findByFlag(domainObjectClass.getSimpleName(), domainObjectFlag.getName(), value);
        return convertVerticesToDomainObjects(domainObjectClass, vertices);
    }

    /**
     * Removes object of given domain object from repository (both from database and domain object cache)
     *
     * @param domainObject Domain Object to be deleted
     */
    public <T extends DomainObject> void delete(T domainObject) {
        checkNotNull(domainObject, "DomainObject should not be null");

        hooks.forEach(hook -> {
            if (hook.canExecute(domainObject)) {
                Class<OnDeleteHook> hookClass = unproxy(hook.getClass());
                logger.t("Executing {} hook for: {}", hookClass.getSimpleName(), domainObject.toString());
                hook.execute(domainObject);
            }
        });

        Class<? extends DomainObject> domainObjectClass = unproxy(domainObject.getClass());
        cachedObjects.remove(domainObject.getId(), domainObjectClass);

        Vertex vertex = domainObject.getVertex();
        dbDriver.deleteVertex(vertex);
    }

    private <T extends DomainObject> List<T> convertVerticesToDomainObjects(Class<T> domainObjectClass, List<Vertex> vertices) {
        List<T> domainObjectList = new ArrayList<>();
        for (Vertex vertex : vertices) {
            Long id = dbDriver.getId(vertex.getId());
            T domainObject = get(domainObjectClass, id(id));
            domainObjectList.add(domainObject);
        }

        return domainObjectList;
    }
}
