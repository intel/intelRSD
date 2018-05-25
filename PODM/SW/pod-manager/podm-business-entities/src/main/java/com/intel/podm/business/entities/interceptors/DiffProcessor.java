/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.business.entities.interceptors;

import com.intel.podm.business.entities.Diff;
import com.intel.podm.common.types.Pair;
import com.intel.podm.common.types.Status;
import com.intel.podm.common.types.events.EventType;

import javax.enterprise.context.ApplicationScoped;
import java.util.List;
import java.util.Objects;
import java.util.Optional;

import static com.google.common.collect.Lists.newArrayList;
import static com.intel.podm.common.types.Health.CRITICAL;
import static com.intel.podm.common.types.events.EventType.ALERT;
import static com.intel.podm.common.types.events.EventType.RESOURCE_UPDATED;
import static com.intel.podm.common.types.events.EventType.STATUS_CHANGE;

@ApplicationScoped
class DiffProcessor {
    public List<EventType> determineEventTypes(Diff diff) {
        List<EventType> events = newArrayList(RESOURCE_UPDATED);
        if (isEligibleForStatusChangeNotification(diff)) {
            events.add(STATUS_CHANGE);
        }
        if (isEligibleForAlertNotification(diff)) {
            events.add(ALERT);
        }
        return events;
    }

    private boolean isEligibleForAlertNotification(Diff diff) {
        Optional<Pair<Status, Status>> statusDiff = diff.getPairFor("status");
        if (!statusDiff.isPresent()) {
            return false;
        }

        Status previousStatus = statusDiff.get().first();
        Status currentStatus = statusDiff.get().second();

        return previousStatus != null
            && currentStatus != null
            && !Objects.equals(CRITICAL, previousStatus.getHealth())
            && Objects.equals(CRITICAL, currentStatus.getHealth());
    }

    private boolean isEligibleForStatusChangeNotification(Diff diff) {
        return diff.wasUpdatedFromOneValueToAnother("status");
    }
}
