/*
 * Copyright (c) 2019 Intel Corporation
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

package com.intel.rsd.podm.https;

import org.apache.catalina.connector.Connector;
import org.apache.tomcat.util.descriptor.web.FilterDef;
import org.apache.tomcat.util.descriptor.web.FilterMap;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.autoconfigure.condition.ConditionalOnProperty;
import org.springframework.boot.web.embedded.tomcat.TomcatContextCustomizer;
import org.springframework.boot.web.embedded.tomcat.TomcatServletWebServerFactory;
import org.springframework.boot.web.servlet.server.ServletWebServerFactory;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

@Configuration
@ConditionalOnProperty(prefix = "server.ssl.", name = {"enabled", "key-store", "key-password", "key-store-password"})
public class TomcatConfigForHttp {
    private final ServiceRootOnlyFilter serviceRootOnlyFilter;
    private final SslConfiguration sslConfiguration;

    @Autowired
    public TomcatConfigForHttp(ServiceRootOnlyFilter serviceRootOnlyFilter, SslConfiguration sslConfiguration) {
        this.serviceRootOnlyFilter = serviceRootOnlyFilter;
        this.sslConfiguration = sslConfiguration;
    }

    @Bean
    public ServletWebServerFactory servletContainer() {
        TomcatServletWebServerFactory tomcat = new TomcatServletWebServerFactory();
        tomcat.addAdditionalTomcatConnectors(getServiceRootConnector(sslConfiguration.getHttpPort()));
        tomcat.addContextCustomizers(customizerWithLimitedHttpSupport());
        return tomcat;
    }

    private Connector getServiceRootConnector(int httpPort) {
        Connector connector = new Connector(TomcatServletWebServerFactory.DEFAULT_PROTOCOL);
        connector.setPort(httpPort);
        return connector;
    }

    private TomcatContextCustomizer customizerWithLimitedHttpSupport() {
        TomcatContextCustomizer tomcatContextCustomizer = context -> {
            FilterDef filterDef = new FilterDef();
            filterDef.setFilterName(serviceRootOnlyFilter.getClass().getSimpleName());
            filterDef.setFilterClass(serviceRootOnlyFilter.getClass().getName());
            context.addFilterDef(filterDef);

            FilterMap filterMapping = new FilterMap();
            filterMapping.setFilterName(serviceRootOnlyFilter.getClass().getSimpleName());
            filterMapping.addURLPattern("/*");
            context.addFilterMap(filterMapping);
        };
        return tomcatContextCustomizer;
    }
}
