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

package com.intel.rsd.nodecomposer.types;

/**
 * Types of storage's drive.
 */
public enum MediaType implements EnumeratedType {
    HDD("HDD", 1),
    SSD("SSD", 2);

    private final String value;
    private final Integer priority;

    MediaType(String value, Integer priority) {
        this.value = value;
        this.priority = priority;
    }

    @Override
    public String getValue() {
        return value;
    }

    @Override
    public Integer getPriority() {
        return priority;
    }

    @Override
    public String toString() {
        return getValue();
    }
}
