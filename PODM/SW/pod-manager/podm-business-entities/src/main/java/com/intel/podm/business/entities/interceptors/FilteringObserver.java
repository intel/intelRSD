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

import javax.annotation.PostConstruct;
import javax.ejb.DependsOn;
import javax.ejb.Singleton;
import javax.ejb.Startup;
import javax.inject.Inject;

@Singleton
@Startup
@DependsOn("DatabaseSchemaUpdateFinalizer")
public class FilteringObserver implements EntityEventObserver {

    @Inject
    private CollectingObserver delegate;

    @Inject
    private EventSuppressions eventSuppressions;

    @PostConstruct
    public void init() {
        StaticDelegateInterceptor.setObserver(this);
    }

    @Override
    public void resourceAdded(Entity entity) {
        if (eventSuppressions.isSuppressed(entity)) {
            return;
        }
        delegate.resourceAdded(entity);
    }

    @Override
    public void resourceUpdated(Entity entity, Diff diff) {
        if (eventSuppressions.isSuppressed(entity)) {
            return;
        }
        delegate.resourceUpdated(entity, diff);
    }

    @Override
    public void collectionResourceUpdated(Entity entity, String collectionName) {
        if (eventSuppressions.isSuppressed(entity, collectionName)) {
            return;
        }
        delegate.collectionResourceUpdated(entity, collectionName);
    }

    @Override
    public void resourceRemoved(Entity entity) {
        if (eventSuppressions.isSuppressed(entity)) {
            return;
        }
        delegate.resourceRemoved(entity);
    }

    @Override
    public void onBeforeTransactionCompletion() {
        delegate.onBeforeTransactionCompletion();
    }

    @Override
    public void onTransactionCompletion() {
        delegate.onTransactionCompletion();
    }

    @Override
    public void onTransactionFailed() {
        delegate.onTransactionFailed();
    }
}
