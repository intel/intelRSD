/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.business.redfish.services.detach;

import com.hazelcast.spring.context.SpringAware;
import com.intel.rsd.nodecomposer.business.BusinessApiException;
import com.intel.rsd.nodecomposer.business.redfish.services.actions.RestRequestInvoker;
import com.intel.rsd.nodecomposer.business.redfish.services.helpers.AttachableContext;
import com.intel.rsd.nodecomposer.business.services.redfish.odataid.ODataId;
import com.intel.rsd.nodecomposer.persistence.dao.GenericDao;
import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.Endpoint;
import com.intel.rsd.nodecomposer.persistence.redfish.Fabric;
import com.intel.rsd.nodecomposer.persistence.redfish.base.AttachableAsset;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ComposableAsset;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ConnectedEntity;
import com.intel.rsd.nodecomposer.persistence.redfish.base.DiscoverableEntity;
import com.intel.rsd.nodecomposer.utils.beans.NodeComposerBeanFactory;
import com.intel.rsd.nodecomposer.utils.transactions.RequiresNewTransactionWrapper;
import lombok.SneakyThrows;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.transaction.annotation.Propagation;
import org.springframework.transaction.annotation.Transactional;

import java.util.Optional;
import java.util.Set;

import static com.intel.rsd.collections.IterableHelper.optionalSingle;
import static com.intel.rsd.nodecomposer.business.redfish.services.helpers.AttachableContextBuilder.newAttachableContextBuilder;
import static com.intel.rsd.nodecomposer.utils.Collector.toSingle;

@SpringAware
@SuppressWarnings("checkstyle:ClassFanOutComplexity")
public abstract class Detacher<T extends DiscoverableEntity & AttachableAsset & ComposableAsset> {
    @Autowired
    private GenericDao genericDao;
    @Autowired
    private NodeComposerBeanFactory beanFactory;
    @Autowired
    private RequiresNewTransactionWrapper requiresNewTransactionWrapper;
    @Autowired
    private RestRequestInvoker restRequestInvoker;

    protected abstract void validate(ComposedNode composedNode, T resource) throws BusinessApiException;
    protected abstract void performResourceSpecificActions(ComposedNode composedNode, T resource) throws BusinessApiException;
    protected abstract Fabric retrieveFabricFromAsset(T asset);

    @Transactional(propagation = Propagation.MANDATORY, readOnly = true)
    public void detach(ComposedNode composedNode, T assetToDetach) throws BusinessApiException {
        validate(composedNode, assetToDetach);
        AttachableContext context = newAttachableContextBuilder(composedNode)
            .withFabric(retrieveFabricFromAsset(assetToDetach))
            .withAsset(assetToDetach).build();
        context = requiresNewTransactionWrapper.run(getConfigureFabricDetachTask(context));
        requiresNewTransactionWrapper.runInRetryableTransaction(getDetachNodeAssetsTask(context));
    }

    private DetachAssetOnFabricTask getConfigureFabricDetachTask(AttachableContext detachContext) {
        return beanFactory.create(DetachAssetOnFabricTask.class).init(this, detachContext);
    }

    private DetachNodeAssetsTask getDetachNodeAssetsTask(AttachableContext context) {
        return beanFactory.create(DetachNodeAssetsTask.class).init(this, context);
    }

    protected void detachRelatedItems(ComposedNode composedNode, T resource) {
        for (Endpoint endpoint : resource.getEndpoints()) {
            Set<ConnectedEntity> connectedEntities = endpoint.getConnectedEntities();
            ConnectedEntity connectedEntity = connectedEntities.stream()
                .filter(ce -> resource.equals(ce.getEntityLink()))
                .collect(toSingle());
            composedNode.detachAsset(endpoint);
            if (connectedEntities.size() == 1) {
                genericDao.remove(endpoint);
            } else {
                endpoint.unlinkConnectedEntities(connectedEntity);
            }
            genericDao.remove(connectedEntity);
        }
    }

    @SneakyThrows
    protected void removeResource(ODataId targetEndpointODataId) {
        // TODO: RSD-4999 - check connected entities before delete == 1
        restRequestInvoker.delete(targetEndpointODataId.toUri());
    }

    protected Optional<Endpoint> retrieveTargetEndpoint(AttachableAsset assetToDetach) {
        // TODO: RSD-4999 - find endpoint from external service
        return optionalSingle(assetToDetach.getEndpoints());
    }
}
