/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.persistence.redfish.base;

import com.intel.rsd.nodecomposer.types.Health;
import com.intel.rsd.nodecomposer.types.State;
import org.apache.commons.lang3.builder.EqualsBuilder;

import static com.intel.rsd.nodecomposer.types.Health.CRITICAL;
import static com.intel.rsd.nodecomposer.types.Health.OK;
import static com.intel.rsd.nodecomposer.types.State.ENABLED;
import static com.intel.rsd.nodecomposer.types.State.STANDBY_OFFLINE;
import static com.intel.rsd.nodecomposer.types.State.UNAVAILABLE_OFFLINE;

public final class StatusControl extends EqualsBuilder {
    private final DiscoverableEntity entity;

    private StatusControl(DiscoverableEntity entity) {
        if (entity == null) {
            throw new IllegalArgumentException("DiscoverableEntity cannot be null");
        }
        this.entity = entity;
    }

    public static StatusControl statusOf(DiscoverableEntity entity) {
        return new StatusControl(entity);
    }

    public StatusControl isEnabled() {
        return hasState(ENABLED);
    }

    public StatusControl isStandbyOffline() {
        return hasState(STANDBY_OFFLINE);
    }

    public StatusControl isUnavailableOffline() {
        return hasState(UNAVAILABLE_OFFLINE);
    }

    public StatusControl isCritical() {
        return hasHealth(CRITICAL);
    }

    public StatusControl isHealthy() {
        return hasHealth(OK);
    }

    private StatusControl hasState(State state) {
        if (entity.getStatus() == null) {
            setEquals(false);
        } else {
            append(entity.getStatus().getState(), state);
        }
        return this;
    }

    private StatusControl hasHealth(Health health) {
        if (entity.getStatus() == null) {
            setEquals(false);
        } else {
            append(entity.getStatus().getHealth(), health);
        }
        return this;
    }

    public boolean verify() {
        return isEquals();
    }
}
