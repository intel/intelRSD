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

package com.intel.podm.client.reader;

import com.intel.podm.client.api.ExternalServiceApiReaderException;
import com.intel.podm.client.api.WebClient;
import com.intel.podm.client.api.reader.ExternalServiceReader;
import com.intel.podm.client.api.resources.redfish.RackscaleServiceRootResource;

public final class ExternalServiceReaderImpl implements ExternalServiceReader {
    private final WebClient webClient;

    public ExternalServiceReaderImpl(WebClient webClient) {
        this.webClient = webClient;
    }

    @Override
    public RackscaleServiceRootResource getServiceRoot() throws ExternalServiceApiReaderException {
        return (RackscaleServiceRootResource) webClient.get(webClient.getBaseUri());
    }

    @Override
    public void close() {
        webClient.close();
    }
}
