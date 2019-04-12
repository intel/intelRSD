/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.resourcemanager.layers.merger.resources;

import org.springframework.http.HttpMethod;

import java.io.IOException;
import java.util.Collection;

import static java.util.Arrays.asList;
import static org.springframework.http.HttpMethod.GET;
import static org.springframework.http.HttpMethod.POST;

public class ModifiableCollectionResource extends CollectionResource {

    private final Collection<HttpMethod> supportedMethods;

    public ModifiableCollectionResource(String odataId, String collectionName) throws IOException {
        super(odataId, collectionName);
        this.supportedMethods = asList(GET, POST);
    }

    @Override
    public boolean isMethodSupported(HttpMethod httpMethod) {
        return supportedMethods.contains(httpMethod);
    }

}
