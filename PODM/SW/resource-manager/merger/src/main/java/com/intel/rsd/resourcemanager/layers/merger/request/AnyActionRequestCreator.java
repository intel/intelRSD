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

package com.intel.rsd.resourcemanager.layers.merger.request;

import com.fasterxml.jackson.databind.JsonNode;
import com.intel.rsd.resourcemanager.layers.Response;
import com.intel.rsd.resourcemanager.layers.ServiceId;
import com.intel.rsd.resourcemanager.layers.merger.request.creators.SelectableRequestCreator;
import lombok.extern.slf4j.Slf4j;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;
import org.springframework.stereotype.Component;

import java.util.Collection;
import java.util.Map;

import static com.intel.rsd.resourcemanager.layers.merger.request.ActionInfo.isActionUri;
import static java.util.stream.Collectors.toList;
import static org.springframework.http.HttpMethod.GET;
import static org.springframework.http.HttpMethod.POST;

@Component
@Slf4j
public class AnyActionRequestCreator implements SelectableRequestCreator {

    @Override
    public boolean canBeSelected(HttpMethod httpMethod, String path) {
        return POST.equals(httpMethod) && isActionUri(path);
    }

    @Override
    public ExecutableRequest create(String path, HttpMethod method, HttpHeaders headers, JsonNode body, Map<String, String> requestParams) {
        ActionInfo actionInfo = new ActionInfo(path);
        return layer -> {
            Collection<ServiceId> relatedServices = layer.getServicesExposingPath(path).stream()
                .filter(serviceId -> isSupportedBy(actionInfo, serviceId, layer.invoke(serviceId, actionInfo.getResourcePath(), GET)))
                .collect(toList());

            return relatedServices.stream().map(id -> layer.invoke(id, path, method, headers, body, requestParams)).toArray(Response[]::new);
        };
    }

    private boolean isSupportedBy(ActionInfo actionInfo, ServiceId serviceId, Response response) {
        if (!response.getHttpStatus().is2xxSuccessful()) {
            log.error("Cannot GET resource related with action({}). Requested action cannot be invoked against service ({})", actionInfo, serviceId);
            return false;
        } else {
            return actionInfo.isSupportedBy(response.getBody());
        }
    }
}
