/*
 * Copyright (c) 2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.discovery.external;

import com.intel.rsd.nodecomposer.business.redfish.services.IscsiOobChapAuthenticationUpdater;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Qualifier;
import org.springframework.core.task.TaskExecutor;
import org.springframework.stereotype.Component;
import org.springframework.transaction.event.TransactionalEventListener;

import static com.intel.rsd.nodecomposer.utils.beans.JndiNames.ASYNC_TASKS_EXECUTOR;
import static org.springframework.transaction.event.TransactionPhase.AFTER_COMMIT;

@Slf4j
@Component
public class DiscoveryEventsObserver {
    private final IscsiOobChapAuthenticationUpdater iscsiOobChapAuthenticationUpdater;
    private final ResourceManagerScanner resourceManagerScanner;
    private final TaskExecutor asyncTaskExecutor;

    public DiscoveryEventsObserver(IscsiOobChapAuthenticationUpdater iscsiOobChapAuthenticationUpdater,
                                   ResourceManagerScanner resourceManagerScanner,
                                   @Qualifier(ASYNC_TASKS_EXECUTOR) TaskExecutor asyncTaskExecutor) {
        this.iscsiOobChapAuthenticationUpdater = iscsiOobChapAuthenticationUpdater;
        this.resourceManagerScanner = resourceManagerScanner;
        this.asyncTaskExecutor = asyncTaskExecutor;
    }

    @TransactionalEventListener(phase = AFTER_COMMIT)
    public void onDiscoveryEvent(DiscoveryEvent event) {
        switch (event.getEventType()) {
            case RETRY_REQUESTED:
                // TODO use discovery queue on leader
                asyncTaskExecutor.execute(resourceManagerScanner);
                break;
            case FINISHED:
                iscsiOobChapAuthenticationUpdater.checkIscsiOobAuthentication();
                break;
            default:
                log.warn("Unhandled event: {}", event);
                break;
        }
    }
}
