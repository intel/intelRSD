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

package com.intel.rsd.nodecomposer.discovery.external.finalizers;

import com.intel.rsd.nodecomposer.persistence.redfish.ComposedNode;
import com.intel.rsd.nodecomposer.persistence.redfish.base.ComposableAsset;

import javax.transaction.Transactional;
import java.util.Collection;

import static javax.transaction.Transactional.TxType.MANDATORY;

public abstract class Recoupler<T extends ComposableAsset> {
    protected abstract void reattach(ComposedNode node, Collection<T> assets);
    protected abstract boolean verify(ComposedNode node);

    @Transactional(MANDATORY)
    public boolean recouple(ComposedNode node, Collection<T> assets) {
        reattach(node, assets);
        return verify(node);
    }
}
