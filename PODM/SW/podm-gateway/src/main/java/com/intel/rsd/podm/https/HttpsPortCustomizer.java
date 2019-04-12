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

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.autoconfigure.condition.ConditionalOnProperty;
import org.springframework.boot.web.server.ConfigurableWebServerFactory;
import org.springframework.boot.web.server.WebServerFactoryCustomizer;
import org.springframework.stereotype.Component;

@Component
@ConditionalOnProperty(prefix = "server.ssl.", name = {"enabled", "key-store", "key-password", "key-store-password"})
public class HttpsPortCustomizer implements WebServerFactoryCustomizer<ConfigurableWebServerFactory> {

    private final SslConfiguration sslConfiguration;

    @Autowired
    public HttpsPortCustomizer(SslConfiguration sslConfiguration) {
        this.sslConfiguration = sslConfiguration;
    }

    @Override
    public void customize(ConfigurableWebServerFactory factory) {
        factory.setPort(sslConfiguration.getHttpsPort());
    }
}
