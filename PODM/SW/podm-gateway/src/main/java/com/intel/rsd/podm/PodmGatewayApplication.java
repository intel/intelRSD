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

package com.intel.rsd.podm;

import com.intel.rsd.podm.filters.AddRedfishMandatoryHeadersFilter;
import com.intel.rsd.podm.filters.ContentTypeFilter;
import com.intel.rsd.podm.filters.HttpMethodFilter;
import com.intel.rsd.podm.filters.OdataResourceProvider;
import com.intel.rsd.redfish.RedfishErrorController;
import lombok.SneakyThrows;
import org.apache.commons.io.IOUtils;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.cloud.netflix.zuul.EnableZuulProxy;
import org.springframework.cloud.netflix.zuul.filters.route.FallbackProvider;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Import;
import org.springframework.context.annotation.Scope;
import org.springframework.web.client.RestTemplate;
import org.springframework.web.cors.CorsConfiguration;
import org.springframework.web.cors.UrlBasedCorsConfigurationSource;
import org.springframework.web.filter.CorsFilter;

import java.io.InputStream;

import static java.nio.charset.Charset.defaultCharset;
import static org.springframework.beans.factory.config.ConfigurableBeanFactory.SCOPE_PROTOTYPE;

@EnableZuulProxy
@SpringBootApplication
@SuppressWarnings({"checkstyle:HideUtilityClassConstructor", "checkstyle:ClassFanOutComplexity"})
@Import(RedfishErrorController.class)
public class PodmGatewayApplication {

    public static void main(String[] args) {
        SpringApplication.run(PodmGatewayApplication.class, args);
    }

    @SuppressWarnings("unused")
    @Bean
    public ContentTypeFilter contentTypeFilter() {
        return new ContentTypeFilter();
    }

    @Bean
    public HttpMethodFilter httpMethodFilter() {
        return new HttpMethodFilter();
    }

    @Bean
    public AddRedfishMandatoryHeadersFilter addOdataHeaderFilter() {
        return new AddRedfishMandatoryHeadersFilter();
    }

    @Bean
    public CorsFilter corsFilter() {
        final UrlBasedCorsConfigurationSource source = new UrlBasedCorsConfigurationSource();
        final CorsConfiguration config = new CorsConfiguration();
        config.addAllowedOrigin("*");
        config.addAllowedHeader("*");
        config.addAllowedMethod("OPTIONS");
        config.addAllowedMethod("HEAD");
        config.addAllowedMethod("GET");
        config.addAllowedMethod("PUT");
        config.addAllowedMethod("POST");
        config.addAllowedMethod("DELETE");
        config.addAllowedMethod("PATCH");
        config.addExposedHeader("Location");
        source.registerCorsConfiguration("/**", config);
        return new CorsFilter(source);
    }

    @SuppressWarnings("unused")
    @Bean
    FallbackProvider gatewayFallbackProvider() {
        return new GatewayFallbackProvider();
    }


    @SuppressWarnings("unused")
    @Bean
    @Scope(SCOPE_PROTOTYPE)
    public RestTemplate restTemplate() {
        return new RestTemplate();
    }

    @Bean
    @SneakyThrows
    public OdataResourceProvider odataResourceProvider() {
        try (InputStream is = OdataResourceProvider.class.getResourceAsStream("/schemas/odata.json")) {
            if (is == null) {
                throw new RuntimeException("Cannot find /schemas/odata.json");
            }
            return new OdataResourceProvider(IOUtils.toString(is, defaultCharset()));
        }
    }
}
