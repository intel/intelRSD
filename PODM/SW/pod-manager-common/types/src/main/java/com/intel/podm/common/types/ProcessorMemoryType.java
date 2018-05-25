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

package com.intel.podm.common.types;

public enum ProcessorMemoryType implements EnumeratedType {
    UNKNOWN("Unknown"),
    L1CACHE("L1Cache"),
    L2CACHE("L2Cache"),
    L3CACHE("L3Cache"),
    HBM2("HBM2"),
    FLASH("Flash"),
    DDR("DDR"),
    DDR3("DDR3"),
    DDR4("DDR4"),
    GDDR5("GDDR5");

    private final String value;

    ProcessorMemoryType(String value) {
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
}
