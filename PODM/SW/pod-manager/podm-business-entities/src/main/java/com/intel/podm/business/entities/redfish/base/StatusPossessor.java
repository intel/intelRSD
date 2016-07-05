/*
 * Copyright (c) 2016 Intel Corporation
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

package com.intel.podm.business.entities.redfish.base;

import com.intel.podm.business.entities.base.DomainObjectProperty;
import com.intel.podm.common.types.Status;

import java.util.Objects;

import static com.intel.podm.business.entities.base.DomainObjectProperties.statusProperty;
import static com.intel.podm.common.types.Health.OK;
import static com.intel.podm.common.types.State.ENABLED;

public interface StatusPossessor {
    DomainObjectProperty<Status> STATUS = statusProperty("status");

    Status getStatus();
    void setStatus(Status status);

    default boolean isEnabledAndHealthy() {
        return getStatus() != null && Objects.equals(getStatus().getHealth(), OK) && Objects.equals(getStatus().getState(), ENABLED);
    }
}
