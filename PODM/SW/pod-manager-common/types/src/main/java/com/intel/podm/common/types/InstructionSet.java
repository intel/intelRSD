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

package com.intel.podm.common.types;

public enum InstructionSet implements EnumeratedType {
    X86("x86"),
    X86_64("x86-64"),
    IA_64("IA-64"),
    ARM_A32("ARM-A32"),
    ARM_A64("ARM-A64"),
    MIPS32("MIPS32"),
    MIPS64("MIPS64"),
    OEM("OEM");

    private final String value;

    InstructionSet(final String value) {
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
