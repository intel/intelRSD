/*
 * Copyright (c) 2016-2017 Intel Corporation
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

package com.intel.podm.discovery;

import com.intel.podm.business.entities.events.ExternalServiceDeletionEvent;
import com.intel.podm.common.synchronization.SerialExecutorRegistry;

import javax.enterprise.context.Dependent;
import javax.enterprise.event.Observes;
import javax.inject.Inject;

import static javax.enterprise.event.TransactionPhase.AFTER_SUCCESS;

@Dependent
public class ExternalServiceDeletionObserver {

    @Inject
    private SerialExecutorRegistry serialExecutorRegistry;

    public void onExternalServiceDeletion(@Observes(during = AFTER_SUCCESS) ExternalServiceDeletionEvent externalServiceDeletionEvent) {
        serialExecutorRegistry.unregisterExecutor(externalServiceDeletionEvent.getUuid());
    }
}

