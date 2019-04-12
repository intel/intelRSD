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

public enum AttachableType implements EnumeratedType {
    ENDPOINT(Types.ENDPOINT),
    VOLUME(Types.VOLUME),
    PROCESSOR(Types.PROCESSOR),
    DRIVE(Types.DRIVE);

    private final String value;

    AttachableType(String value) {
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

    public static class Types {
        public static final String ENDPOINT = "Endpoint";
        public static final String VOLUME = "Volume";
        public static final String PROCESSOR = "Processor";
        public static final String DRIVE = "Drive";
    }
}
