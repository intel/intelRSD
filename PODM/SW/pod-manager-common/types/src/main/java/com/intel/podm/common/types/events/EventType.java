/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.common.types.events;

import com.intel.podm.common.types.EnumeratedType;

public enum EventType implements EnumeratedType {
    STATUS_CHANGE("StatusChange"),
    RESOURCE_UPDATED("ResourceUpdated"),
    RESOURCE_ADDED("ResourceAdded"),
    RESOURCE_REMOVED("ResourceRemoved"),
    ALERT("Alert");

    private final String value;

    EventType(String value) {
        this.value = value;
    }

    public static boolean isStringRedfishEventType(String s) {
        for (EventType eventType : values()) {
            if (eventType.getValue().equals(s)) {
                return true;
            }
        }
        return false;
    }

    @Override
    public String getValue() {
        return value;
    }
}
