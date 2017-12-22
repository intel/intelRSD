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
import com.intel.podm.common.logger.Logger;
import org.hibernate.CallbackException;
import org.hibernate.EmptyInterceptor;
import org.hibernate.Transaction;
import org.hibernate.collection.spi.PersistentCollection;
import org.hibernate.type.Type;

import java.io.Serializable;

import static com.intel.podm.common.logger.LoggerFactory.getLogger;
import static org.hibernate.resource.transaction.spi.TransactionStatus.COMMITTED;

public class StaticDelegateInterceptor extends EmptyInterceptor {
    private static final boolean ENTITY_HAS_NOT_BEEN_CHANGED_BY_THIS_METHOD = false;
    private static final Logger LOGGER = getLogger(StaticDelegateInterceptor.class);

    private static final long serialVersionUID = 8871422760076723622L;

    private static EntityEventObserver observer;

    public static void setStaticDelegateInterceptorObserver(EntityEventObserver observer) {
        StaticDelegateInterceptor.observer = observer;
    }

    @Override
    public void afterTransactionCompletion(Transaction tx) {
        if (tx.getStatus() == COMMITTED) {
            StaticDelegateInterceptor.observer.onTransactionCompletion();
        } else {
            StaticDelegateInterceptor.observer.onTransactionFailed();
        }
    }

    @Override
    public void beforeTransactionCompletion(Transaction tx) {
        StaticDelegateInterceptor.observer.onBeforeTransactionCompletion();
    }

    @Override
    public boolean onSave(Object entity, Serializable id, Object[] state, String[] propertyNames, Type[] types) {
        StaticDelegateInterceptor.observer.resourceAdded((Entity) entity);
        return ENTITY_HAS_NOT_BEEN_CHANGED_BY_THIS_METHOD;
    }

    @Override
    @SuppressWarnings("checkstyle:ParameterNumber")
    public boolean onFlushDirty(Object entity, Serializable id, Object[] currentState, Object[] previousState, String[] propertyNames, Type[] types) {
        Diff diff = new Diff(previousState, currentState, propertyNames);
        StaticDelegateInterceptor.observer.resourceUpdated((Entity) entity, diff);
        return ENTITY_HAS_NOT_BEEN_CHANGED_BY_THIS_METHOD;
    }

    @Override
    public void onDelete(Object entity, Serializable id, Object[] state, String[] propertyNames, Type[] types) {
        StaticDelegateInterceptor.observer.resourceRemoved((Entity) entity);
    }

    @Override
    public void onCollectionUpdate(Object collection, Serializable key) throws CallbackException {
        if (!(collection instanceof PersistentCollection)) {
            LOGGER.e("Unsupported collection type: {}", collection.getClass());
            return;
        }
        Entity owner = (Entity) ((PersistentCollection) collection).getOwner();
        String role = ((PersistentCollection) collection).getRole();
        StaticDelegateInterceptor.observer.collectionResourceUpdated(owner, role);
    }
}
