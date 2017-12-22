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

package com.intel.podm.discovery.external;

import java.util.UUID;

import static com.intel.podm.discovery.external.ExternalServiceMonitoringEvent.MonitoringState.STARTED;
import static com.intel.podm.discovery.external.ExternalServiceMonitoringEvent.MonitoringState.STOPPED;

public final class ExternalServiceMonitoringEvent {
    private final UUID uuid;
    private final MonitoringState monitoringState;

    private ExternalServiceMonitoringEvent(UUID externalServiceUuid, MonitoringState monitoringState) {
        this.uuid = externalServiceUuid;
        this.monitoringState = monitoringState;
    }

    static ExternalServiceMonitoringEvent externalServiceMonitoringStartedEvent(UUID externalServiceUuid) {
        return new ExternalServiceMonitoringEvent(externalServiceUuid, STARTED);
    }

    static ExternalServiceMonitoringEvent externalServiceMonitoringStoppedEvent(UUID externalServiceUuid) {
        return new ExternalServiceMonitoringEvent(externalServiceUuid, STOPPED);
    }

    public UUID getExternalServiceUuid() {
        return uuid;
    }

    public MonitoringState getMonitoringState() {
        return monitoringState;
    }

    public enum MonitoringState {
        STARTED,
        STOPPED
    }
}
