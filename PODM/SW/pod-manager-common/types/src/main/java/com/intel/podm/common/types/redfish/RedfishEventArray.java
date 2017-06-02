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

package com.intel.podm.common.types.redfish;

import com.intel.podm.common.types.events.EventType;

import java.net.URI;
import java.util.Collection;

import static java.util.Collections.emptyList;

public interface RedfishEventArray {
    default String getOdataType() {
        return null;
    }

    default URI getOdataContext() {
        return null;
    }

    default String getOdataId() {
        return null;
    }

    default String getId() {
        return null;
    }

    default String getName() {
        return null;
    }

    default String getDescription() {
        return null;
    }

    default Object getOem() {
        return null;
    }

    default Collection<? extends Event> getEvents() {
        return emptyList();
    }

    interface Event {
        default EventType getEventType() {
            return null;
        }

        default String getEventId() {
            return null;
        }

        default String getEventTimestamp() {
            return null;
        }

        default String getSeverity() {
            return null;
        }

        default String getMessage() {
            return null;
        }

        default String getMessageId() {
            return null;
        }

        default String getContext() {
            return null;
        }

        default URI getOriginOfCondition() {
            return null;
        }

        default Collection<String> getMessageArgs() {
            return emptyList();
        }

    }
}
