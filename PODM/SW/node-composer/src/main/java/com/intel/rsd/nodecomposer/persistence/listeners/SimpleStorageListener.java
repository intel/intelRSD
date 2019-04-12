/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.persistence.listeners;

import com.intel.rsd.nodecomposer.persistence.handlers.OnDeleteSimpleStorageHandler;
import com.intel.rsd.nodecomposer.persistence.redfish.SimpleStorage;
import org.springframework.context.annotation.Scope;
import org.springframework.stereotype.Component;

import javax.persistence.PreRemove;

import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@Component
@Scope(SCOPE_PROTOTYPE)
public class SimpleStorageListener extends EntityListener {
    @PreRemove
    public void preRemove(SimpleStorage simpleStorage) {
        OnDeleteSimpleStorageHandler onDeleteSimpleStorageHandler = create(OnDeleteSimpleStorageHandler.class);
        onDeleteSimpleStorageHandler.preRemove(simpleStorage);
    }
}
