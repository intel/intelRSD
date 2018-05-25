/*
 * Copyright (c) 2016-2018 Intel Corporation
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

public enum BaseModuleType implements EnumeratedType {
    RDIMM("RDIMM"),
    UDIMM("UDIMM"),
    SO_DIMM("SO_DIMM"),
    LRDIMM("LRDIMM"),
    MINI_RDIMM("Mini_RDIMM"),
    MINI_UDIMM("Mini_UDIMM"),
    RDIMM_SO_72B("SO_RDIMM_72b"),
    UDIMM_SO_72B("SO_UDIMM_72b"),
    DIMM_SO_16B("SO_DIMM_16b"),
    DIMM_SO_32B("SO_DIMM_32b");

    private final String value;

    BaseModuleType(String value) {
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
