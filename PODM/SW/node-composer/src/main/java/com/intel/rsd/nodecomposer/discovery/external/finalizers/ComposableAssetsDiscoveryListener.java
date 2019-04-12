/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.discovery.external.finalizers;

import com.intel.rsd.nodecomposer.persistence.redfish.base.ComposableAsset;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.transaction.Transactional;
import java.util.Collection;
import java.util.Set;

import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.MANDATORY;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_SINGLETON;

@Component
@Scope(SCOPE_SINGLETON)
public class ComposableAssetsDiscoveryListener {
    private final ComposedNodeRecoveryService composeNodeRecoveryService;
    private final ComposedNodeDisableService composedNodeDisableService;

    @Autowired
    public ComposableAssetsDiscoveryListener(ComposedNodeRecoveryService composeNodeRecoveryService, ComposedNodeDisableService composedNodeDisableService) {
        this.composeNodeRecoveryService = composeNodeRecoveryService;
        this.composedNodeDisableService = composedNodeDisableService;
    }

    @Transactional(MANDATORY)
    public void updateRelatedComposedNodes(Collection<ComposableAsset> composableAssets) {
        Set<ComposableAsset> availableAssets = getAvailableAssets(composableAssets);
        composeNodeRecoveryService.recoverFromAssets(availableAssets);

        Set<ComposableAsset> degradedAssets = getDegradedAssets(composableAssets);
        composedNodeDisableService.disableFromAssets(degradedAssets);
    }

    private Set<ComposableAsset> getAvailableAssets(Collection<ComposableAsset> assets) {
        return assets.stream().filter(ComposableAsset::isAvailable).collect(toSet());
    }

    private Set<ComposableAsset> getDegradedAssets(Collection<ComposableAsset> assets) {
        return assets.stream().filter(ComposableAsset::isDegraded).collect(toSet());
    }
}
