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

package com.intel.podm.business.entities.interceptors;

import com.intel.podm.business.entities.Diff;
import com.intel.podm.business.entities.redfish.base.Entity;

public interface EntityEventObserver {

    /**
     * This method is invoked each time when EntityManager.persist(entity) is called.
     * PodM application creates entity in two steps:
     * 1. Empty entity (created by default constructor) is persisted
     * 2. State of entity is fulfilled by a mapper.
     * Because of that, passed entity instance doesn't have full state yet.
     */
    default void resourceAdded(Entity entity) { }

    /**
     * This method is invoked each time when existing entity will be updated and flush operation will be called.
     *
     * @param entity modified entity
     * @param diff information about detected changes
     */
    default void resourceUpdated(Entity entity, Diff diff) { }

    /**
     * This method is invoked each time after persistent collection updated.
     *
     * @param entity parent of persistence collection
     * @param collectionName name of collection
     */
    default void collectionResourceUpdated(Entity entity, String collectionName) { }

    /**
     * This method is invoked each time when EntityManager.remove(entity) method is called.
     *
     * @param entity instance of entity which has been removed. Any associations of this entity are already unlinked, so you cannot rely on them.
     */
    default void resourceRemoved(Entity entity) { }

    default void onBeforeTransactionCompletion() { }

    default void onTransactionCompletion() { }

    default void onTransactionFailed() { }
}
