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

package com.intel.rsd.nodecomposer.externalservices;

import com.intel.rsd.nodecomposer.externalservices.resources.ExternalServiceResource;

import java.net.URI;
import java.util.Optional;

public interface WebClient extends AutoCloseable {
    URI getBaseUri();

    @Override
    void close();

    ExternalServiceResource get(URI uri) throws WebClientRequestException;
    <T> URI post(URI requestUri, T obj) throws WebClientRequestException;
    <T> void postNotMonitored(String requestUri, T obj) throws WebClientRequestException;
    <T> Optional<ExternalServiceResource> patch(URI requestUri, T obj) throws WebClientRequestException;
    void delete(URI requestUri) throws WebClientRequestException;

    /**
     * Performs PATCH request using obj on requestUri and returns patched resource (performs additional GET on it if necessary)
     */
    @SuppressWarnings("unchecked")
    default <T extends ExternalServiceResource> T patchAndRetrieve(URI requestUri, Object obj) throws WebClientRequestException {
        return (T) patch(requestUri, obj).orElse(get(requestUri));
    }
}
