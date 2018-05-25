/*
 * Copyright (c) 2015-2018 Intel Corporation
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

package com.intel.podm.discovery.external.finalizers;

import com.intel.podm.business.entities.redfish.base.ComposableAsset;

import javax.enterprise.context.Dependent;
import javax.inject.Inject;
import javax.transaction.Transactional;
import java.util.Collection;
import java.util.Set;

import static java.util.stream.Collectors.toSet;
import static javax.transaction.Transactional.TxType.MANDATORY;

@Dependent
public class ComposableAssetsDiscoveryListener {
    @Inject
    private ComposedNodeRecoveryService composeNodeRecoveryService;

    @Inject
    private ComposedNodeDisableService composedNodeDisableService;

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
