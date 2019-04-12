/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.crawler;

import java.util.function.Supplier;

public final class ResourceSupplier implements Supplier<Resource> {
    private final Resource resource;
    private final String odataId;

    private ResourceSupplier(Resource resource) {
        this.resource = resource;
        this.odataId = resource.getOdataId();
    }

    private ResourceSupplier(String serviceSpecificOdataId) {
        this.resource = null;
        this.odataId = serviceSpecificOdataId;
    }

    public static ResourceSupplier asSupplier(Resource resource) {
        return new ResourceSupplier(resource);
    }

    public static ResourceSupplier asSupplier(String serviceSpecificOdataId) {
        return new ResourceSupplier(serviceSpecificOdataId);
    }

    @Override
    public Resource get() {
        return resource;
    }

    public String getOdataId() {
        return odataId;
    }
}
