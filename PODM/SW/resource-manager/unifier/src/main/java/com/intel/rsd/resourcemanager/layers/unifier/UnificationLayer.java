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

package com.intel.rsd.resourcemanager.layers.unifier;

import com.fasterxml.jackson.databind.JsonNode;
import com.intel.rsd.resourcemanager.layers.Layer;
import com.intel.rsd.resourcemanager.layers.CrawlerFactory;
import com.intel.rsd.resourcemanager.layers.Response;
import com.intel.rsd.resourcemanager.layers.ServiceId;
import com.intel.rsd.resourcemanager.layers.unifier.unification.GrandUnifier;
import com.intel.rsd.resourcemanager.layers.unifier.unification.UnifiedOdataIdResolver;
import com.intel.rsd.resourcemanager.layers.unifier.unification.UnifiedOdataIdResolverFactory;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpHeaders;
import org.springframework.http.HttpMethod;
import org.springframework.stereotype.Component;

import java.util.Collection;
import java.util.Map;
import java.util.Set;

import static java.util.stream.Collectors.toList;
import static org.springframework.http.HttpMethod.DELETE;
import static org.springframework.http.ResponseEntity.notFound;

@Component
@Slf4j
public class UnificationLayer extends Layer {
    private static final String LAYER_NAME = "Unifier";

    private final UnifiedOdataIdResolverFactory unifiedOdataIdResolverFactory;

    private CrawlerFactory crawlerFactoryFactory;
    private GrandUnifier unifier;

    @Autowired
    public UnificationLayer(UnifiedOdataIdResolverFactory unifiedOdataIdResolverFactory, CrawlerFactory crawlerFactoryFactory, GrandUnifier unifier) {
        this.unifiedOdataIdResolverFactory = unifiedOdataIdResolverFactory;
        this.crawlerFactoryFactory = crawlerFactoryFactory;
        this.unifier = unifier;
    }

    @Override
    public Set<ServiceId> getServices() {
        return nextLayer.getServices();
    }

    @Override
    public Collection<ServiceId> getServicesExposingPath(String path) {
        return getServices().stream()
            .filter(serviceId -> unifiedOdataIdResolverFactory.createUnifiedOdataIdResolver(serviceId.toString()).toServiceSpecific(path).isPresent())
            .collect(toList());
    }

    @Override
    public final String id() {
        return LAYER_NAME;
    }

    @Override
    protected Response invokeImpl(ServiceId serviceId, String path, HttpMethod method, HttpHeaders headers, JsonNode body,
                                  Map<String, String> requestParams) {
        val resolver = createLoadingResolver(serviceId);
        val serviceSpecificUri = resolver.toServiceSpecific(path);

        if (serviceSpecificUri.isPresent()) {
            val deunifiedBody = unifier.deunify(body, resolver);
            val nextLayerResponse = passToNextLayer(serviceId, serviceSpecificUri.get(), method, headers, deunifiedBody, requestParams);
            if (DELETE.equals(method) && nextLayerResponse.getHttpStatus().is2xxSuccessful()) {
                removeMappings(serviceSpecificUri.get(), path, resolver);
            }
            return unifier.unify(nextLayerResponse, resolver);
        } else {
            return new Response(notFound().build());
        }
    }

    private void removeMappings(String serviceSpecificUri, String unifiedUri, UnifiedOdataIdResolver resolver) {
        log.debug("Removing mapping for {}, unified {}", serviceSpecificUri, unifiedUri);
        resolver.getUnificationMappings().getMapping().remove(serviceSpecificUri);
        resolver.getUnificationMappings().getReverseMapping().remove(unifiedUri);
    }

    UnifiedOdataIdResolver createLoadingResolver(ServiceId serviceId) {
        val crawler = crawlerFactoryFactory.builder().useAccessLayer(this).forService(serviceId).buildCurrentThreadBasedCrawler();
        return unifiedOdataIdResolverFactory.createLoadingResolver(serviceId.toString(), crawler);
    }
}
