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

public enum ChassisType implements EnumeratedType {
    RACK("Rack"),
    BLADE("Blade"),
    ENCLOSURE("Enclosure"),
    STANDALONE("StandAlone"),
    RACK_MOUNT("RackMount"),
    CARD("Card"),
    CARTRIDGE("Cartridge"),
    ROW("Row"),
    POD("Pod"),
    EXPANSION("Expansion"),
    SIDECAR("Sidecar"),
    ZONE("Zone"),
    SLED("Sled"),
    SHELF("Shelf"),
    DRAWER("Drawer"),
    MODULE("Module"),
    COMPONENT("Component"),
    IP_BASED_DRIVE("IPBasedDrive"),
    RACK_GROUP("RackGroup"),
    OTHER("Other");

    private final String value;

    ChassisType(String value) {
        this.value = value;
    }

    public String getValue() {
        return value;
    }

    @Override
    public String toString() {
        return getValue();
    }
}
