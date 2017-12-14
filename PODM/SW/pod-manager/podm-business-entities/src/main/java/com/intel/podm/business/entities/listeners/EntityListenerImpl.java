/*
 * Copyright (c) 2017 Intel Corporation
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

package com.intel.podm.business.entities.listeners;

import com.intel.podm.business.entities.redfish.base.Entity;
import com.intel.podm.business.entities.types.EntityAdded;

import javax.enterprise.context.ApplicationScoped;
import javax.enterprise.util.AnnotationLiteral;
import javax.persistence.PostPersist;

@ApplicationScoped
public class EntityListenerImpl extends EntityListener {
    private static final AnnotationLiteral<EntityAdded> ENTITY_ADDED = new AnnotationLiteral<EntityAdded>() {
        private static final long serialVersionUID = 1013570273485238779L;
    };

    @PostPersist
    public void postPersist(Entity entity) {
        beanManager.fireEvent(entity, ENTITY_ADDED);
    }
}
