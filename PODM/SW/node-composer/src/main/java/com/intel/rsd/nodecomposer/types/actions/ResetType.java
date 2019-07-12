/*
 * Copyright (c) 2015-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.types.actions;

import com.intel.rsd.nodecomposer.types.EnumeratedType;

import static java.util.Arrays.stream;

public enum ResetType implements EnumeratedType {
    ON("On"),
    FORCE_OFF("ForceOff"),
    GRACEFUL_SHUTDOWN("GracefulShutdown"),
    GRACEFUL_RESTART("GracefulRestart"),
    FORCE_RESTART("ForceRestart"),
    NMI("Nmi"),
    FORCE_ON("ForceOn"),
    PUSH_POWER_BUTTON("PushPowerButton"),
    POWER_CYCLE("PowerCycle");

    private final String value;

    ResetType(String value) {
        this.value = value;
    }

    @Override
    public String getValue() {
        return value;
    }

    @Override
    public String toString() {
        return getValue();
    }

    public static boolean isAnyOfTypes(ResetType resetType, ResetType... states) {
        return resetType != null && stream(states).anyMatch(expectedResetType -> resetType == expectedResetType);
    }
}
