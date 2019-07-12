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

package com.intel.rsd.resourcemanager.layers.merger;

import com.fasterxml.jackson.databind.JsonNode;
import com.intel.rsd.resourcemanager.layers.Layer;
import com.intel.rsd.resourcemanager.layers.Response;
import com.intel.rsd.resourcemanager.layers.ServiceId;
import com.intel.rsd.resourcemanager.layers.merger.request.creators.RequestCreatorProvider;
import com.intel.rsd.resourcemanager.layers.merger.response.ResponseConverter;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;
import org.springframework.stereotype.Component;

import java.util.Map;
import java.util.Set;
import java.util.UUID;

import static java.util.Collections.singleton;

@Component
public class Merger extends Layer {

    private final ServiceId serviceId;

    private final RequestCreatorProvider requestCreatorProvider;

    private final ResponseConverter responseConverter;

    @Autowired
    public Merger(UUID serviceRootUuid, RequestCreatorProvider requestCreatorProvider, ResponseConverter responseConverter) {
        this.serviceId = new ServiceId(serviceRootUuid.toString());
        this.requestCreatorProvider = requestCreatorProvider;
        this.responseConverter = responseConverter;
    }

    @Override
    public Set<ServiceId> getServices() {
        return singleton(serviceId);
    }

    @Override
    protected Response invokeImpl(ServiceId serviceId, String path, HttpMethod method, HttpHeaders headers, JsonNode body,
                                  Map<String, String> requestParams) {

        val requestCreator = requestCreatorProvider.selectCreator(path, method);

        val request = requestCreator.create(path, method, headers, body, requestParams);

        return responseConverter.merge(
            request.execute(nextLayer)
        );
    }
}
