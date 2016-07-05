/*
 * Copyright (c) 2016 Intel Corporation
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

package com.intel.podm.client.resources.redfish.properties;

import com.fasterxml.jackson.annotation.JsonAnyGetter;
import com.fasterxml.jackson.annotation.JsonAnySetter;

import java.util.HashMap;
import java.util.Map;
import java.util.Optional;

public class OemResource {
    Map<String, OemVendorImpl> vendors = new HashMap<>();

    @JsonAnySetter
    private void addVendor(String name, OemVendorImpl oem) {
        vendors.put(name, oem);
    }

    @JsonAnyGetter
    private Map<String, OemVendorImpl> getVendors() {
        return vendors;
    }

    public <T extends OemVendorImpl> Optional<T> getOfType(Class<T> oemClass) {
        return vendors.values()
                .stream()
                .filter(oemClass::isInstance)
                .map(oemClass::cast)
                .findFirst();
    }

    public <T extends OemVendorImpl> T getOfType(Class<T> oemClass, T defaultValue) {
        return getOfType(oemClass).orElse(defaultValue);
    }

    public OemVendorImpl getOem(String vendorName) {
        return vendors.get(vendorName);
    }

}
