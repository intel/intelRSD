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

package com.intel.podm.business.redfish.services;

import com.intel.podm.actions.ActionException;
import com.intel.podm.actions.ResetActionInvoker;
import com.intel.podm.business.EntityNotFoundException;
import com.intel.podm.business.entities.redfish.ComputerSystem;
import com.intel.podm.business.redfish.DomainObjectTreeTraverser;
import com.intel.podm.business.services.context.Context;
import com.intel.podm.common.types.actions.ResetType;
import com.intel.podm.config.base.Config;
import com.intel.podm.config.base.Holder;
import com.intel.podm.config.base.dto.DiscoveryConfig;
import com.intel.podm.discovery.external.deep.DeepDiscoveryHelper;

import javax.enterprise.context.RequestScoped;
import javax.inject.Inject;
import javax.transaction.Transactional;

import static java.lang.String.format;
import static javax.transaction.Transactional.TxType.REQUIRED;

@RequestScoped
@Transactional(REQUIRED)
public class ComputerSystemActionsService {

    @Inject
    private DomainObjectTreeTraverser traverser;

    @Inject
    private ResetActionInvoker invoker;

    @Inject @Config
    private Holder<DiscoveryConfig> config;

    @Inject
    private DeepDiscoveryHelper deepDiscoveryHelper;

    public void reset(Context context, ResetType resetType) throws EntityNotFoundException, ActionException, IllegalStateException {
        ComputerSystem computerSystem = (ComputerSystem) traverser.traverse(context);

        if (!computerSystem.getAllowableResetTypes().contains(resetType)) {
            String msg = format("Provided ResetType should be in allowable ResetType's %s", computerSystem.getAllowableResetTypes());
            throw new IllegalArgumentException(msg);
        }

        if (!computerSystem.isEnabledAndHealthy()) {
            throw new IllegalStateException("ComputerSystem should be enabled and healthy in order to invoke actions on it.");
        }

        invoker.reset(computerSystem, resetType);
    }

    public void startDeepDiscovery(Context context) throws EntityNotFoundException, ActionException {
        ComputerSystem computerSystem = (ComputerSystem) traverser.traverse(context);

        if (!computerSystem.isEnabledAndHealthy()) {
            throw new ActionException("ComputerSystem should be enabled and healthy in order to invoke actions on it.");
        }

        if (!config.get().isDeepDiscoveryEnabled()) {
            throw new ActionException("Deep discovery action cannot be triggered. Functionality has been disabled.");
        } else if (computerSystem.getComposedNode() != null) {
            throw new ActionException("Deep discovery action cannot be triggered. ComputerSystem is used by a Node.");
        } else if (computerSystem.isStorageServiceHost()) {
            throw new ActionException("Deep discovery action cannot be triggered. ComputerSystem is used as host for Storage Service");
        } else if (deepDiscoveryHelper.isDeepDiscoveryInProgress(computerSystem)) {
            throw new ActionException("Deep discovery action cannot be triggered. Deep discovery is already in progress.");
        } else {
            deepDiscoveryHelper.triggerDeepDiscovery(computerSystem);
        }
    }



}
