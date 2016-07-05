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

import com.google.common.collect.HashBiMap;
import com.intel.podm.business.entities.base.DomainObject;
import com.intel.podm.business.entities.base.DomainObjectInspector;
import com.intel.podm.repository.api.GraphSchemaManager;
import com.intel.podm.repository.api.GraphVertexType;

import javax.enterprise.context.ApplicationScoped;
import javax.inject.Inject;
import java.util.Map;

import static com.google.common.base.Preconditions.checkNotNull;

/**
 * OgmManager allows to manage mapping of Object Model (provided with DomainObject classes) to Graph Model.
 *  Be aware that DomainObject class proxied by container will not be handled in any special way.
*/
@ApplicationScoped
public class OgmManager {

    @Inject
    GraphSchemaManager graphSchemaManager;

    private final HashBiMap<String, Class<? extends DomainObject>> mappedDomainObjectClasses = HashBiMap.create();

    public void addToMapping(Class<? extends DomainObject> domainObjectClass) {
        checkNotNull(domainObjectClass);

        DomainObjectInspector inspector = DomainObjectInspector.inspect(domainObjectClass);
        Map<String, Class> properties = inspector.getProperties();
        String name = domainObjectClass.getSimpleName();

        GraphVertexType vertexType = new GraphVertexType(name, properties);
        graphSchemaManager.registerVertexType(vertexType);

        mappedDomainObjectClasses.put(name, domainObjectClass);
    }

    public Class<? extends DomainObject> getDomainObjectClass(String vertexClassName) {
        checkNotNull(vertexClassName);

        if (!mappedDomainObjectClasses.containsKey(vertexClassName)) {
            throw new IllegalArgumentException("There is no mapping for vertex type name: " + vertexClassName);
        }

        return mappedDomainObjectClasses.get(vertexClassName);
    }

    public String getVertexClassName(Class<? extends DomainObject> clazz) {
        checkNotNull(clazz);

        if (!mappedDomainObjectClasses.inverse().containsKey(clazz)) {
            throw new IllegalArgumentException("There is no mapping for class: " + clazz.getSimpleName());
        }

        return mappedDomainObjectClasses.inverse().get(clazz);
    }

    public void addLinkDefinition(String linkName) {
        graphSchemaManager.registerEdgeType(linkName);
    }
}
