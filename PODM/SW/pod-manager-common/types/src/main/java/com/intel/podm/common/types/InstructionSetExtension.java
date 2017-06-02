/*
 * Copyright (c) 2015-2017 Intel Corporation
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

/**
 * Enumeration of extended features supported by processors
 */
public enum InstructionSetExtension implements EnumeratedType {
    X86_SSE("x86:SSE"),
    X86_SSE2("x86:SSE2"),
    X86_SSE3("x86:SSE3"),
    X86_SSE4("x86:SSE4"),
    X86_SSE5("x86:SSE5"),
    X86_AVX("x86:AVX"),
    X86_AVX2("x86:AVX2"),
    X86_VT_D("x86:VT-d"),
    X86_VT_X("x86:VT-x"),
    X86_AES("x86:AES"),
    X86_MMX("x86:MMX"),
    X86_EM64T("x86:EM64T");

    private final String value;

    InstructionSetExtension(String value) {
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
