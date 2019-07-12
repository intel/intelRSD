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

package com.intel.rsd.resourcemanager.layers;

import com.intel.rsd.crawler.Crawler;
import com.intel.rsd.crawler.Resource;
import com.intel.rsd.crawler.SerializableResource;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.springframework.http.HttpHeaders;

import java.util.concurrent.CompletableFuture;
import java.util.concurrent.Executor;
import java.util.concurrent.Executors;
import java.util.function.BiFunction;
import java.util.function.Function;
import java.util.function.Supplier;

import static java.util.concurrent.CompletableFuture.supplyAsync;
import static org.springframework.http.HttpMethod.GET;

@Slf4j
public class CrawlerFactory {
    private final Executor threadPool;

    public CrawlerFactory(int threadPoolSize) {
        this.threadPool = Executors.newFixedThreadPool(threadPoolSize);
    }

    public AccessLayerBuilder builder() {
        return new Builder();
    }

    public interface AccessLayerBuilder {
        ServiceIdBuilder useAccessLayer(Layer accessLayer);
    }

    public interface ServiceIdBuilder {
        ThreadPoolConfigurer forService(ServiceId serviceId);
    }

    public interface ThreadPoolConfigurer {
        Crawler buildCurrentThreadBasedCrawler();
        Crawler buildThreadPoolBasedCrawler();
    }

    private final class Builder implements AccessLayerBuilder, ServiceIdBuilder, ThreadPoolConfigurer {

        private ServiceId serviceId;
        private Layer accessLayer;

        private Builder() {
        }

        @Override
        public Builder useAccessLayer(Layer accessLayer) {
            this.accessLayer = accessLayer;
            return this;
        }

        @Override
        public Builder forService(ServiceId serviceId) {
            this.serviceId = serviceId;
            return this;
        }

        @Override
        public Crawler buildCurrentThreadBasedCrawler() {
            return new Crawler(getInCurrentThreadResourceGetter());
        }

        @Override
        public Crawler buildThreadPoolBasedCrawler() {
            return new Crawler(getResourceGetter());
        }

        private Function<String, CompletableFuture<Resource>> getInCurrentThreadResourceGetter() {
            return odataId -> CompletableFuture.completedFuture(null)
                .thenApply(ignored -> this.accessLayer.getNextLayer().invoke(serviceId, odataId, GET, getHttpHeadersWithNotStaleCache()))
                .handleAsync(responseHandler(odataId), Runnable::run);
        }

        private Function<String, CompletableFuture<Resource>> getResourceGetter() {
            return odataId -> supplyAsync(requestInvoker(odataId), threadPool).handleAsync(responseHandler(odataId), threadPool);
        }

        private Supplier<Response> requestInvoker(String odataId) {
            return () -> accessLayer.getNextLayer().invoke(serviceId, odataId, GET, getHttpHeadersWithNotStaleCache());
        }

        private BiFunction<Response, Throwable, Resource> responseHandler(String odataId) {
            return (response, ex) -> {
                if (ex != null) {
                    log.warn("Exception caught during crawling URI({}) on ServiceId({}), {}", odataId, serviceId, ex);
                    return null;
                }

                if (!response.getHttpStatus().is2xxSuccessful()) {
                    log.warn("Unexpected not 20x status({}) during crawling URI({}) on ServiceId({})", response.getHttpStatus(), odataId, serviceId);
                    return null;
                }

                if (response.getBody() == null) {
                    log.warn("Unexpected null body during crawling URI({}) on ServiceId({})", response.getHttpStatus(), odataId, serviceId);
                    return null;
                }
                return new SerializableResource(response.getBody());
            };
        }

        private HttpHeaders getHttpHeadersWithNotStaleCache() {
            val headers = new HttpHeaders();
            headers.setCacheControl("max-stale=0");
            return headers;
        }
    }
}
