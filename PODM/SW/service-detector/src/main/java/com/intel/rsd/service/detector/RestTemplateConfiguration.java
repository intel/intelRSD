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

package com.intel.rsd.service.detector;

import com.intel.rsd.http.config.KeyStoreConfig;
import lombok.SneakyThrows;
import lombok.val;
import org.apache.http.conn.ssl.NoopHostnameVerifier;
import org.apache.http.conn.ssl.TrustAllStrategy;
import org.apache.http.ssl.SSLContextBuilder;
import org.springframework.beans.factory.annotation.Qualifier;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.http.client.HttpComponentsClientHttpRequestFactory;
import org.springframework.web.client.RestTemplate;

import javax.net.ssl.SSLContext;
import java.util.function.Predicate;

import static org.apache.http.impl.client.HttpClients.custom;

@Configuration
public class RestTemplateConfiguration {
    private Predicate<KeyStoreConfig> pathIsDefined = config -> config.getPath() != null;
    private Predicate<KeyStoreConfig> aliasIsDefined = config -> config.getAlias() != null;
    private Predicate<KeyStoreConfig> passwordIsDefined = config -> config.getPassword() != null;
    private Predicate<KeyStoreConfig> keyStoreConfigIsProvided = pathIsDefined.and(aliasIsDefined).and(passwordIsDefined);

    @Bean
    @SneakyThrows
    @Qualifier("trustingAllSslContext")
    public SSLContext trustingAllSslContext(KeyStoreConfig keyStoreConfig) {
        val sslContextBuilder = new SSLContextBuilder();

        if (keyStoreConfigIsProvided.test(keyStoreConfig)) {
            val password = keyStoreConfig.getPassword().toCharArray();
            sslContextBuilder.loadKeyMaterial(
                keyStoreConfig.getPath().getFile().toPath().toFile(),
                password, password,
                (aliases, socket) -> keyStoreConfig.getAlias()
            );
        }

        return sslContextBuilder
            .loadTrustMaterial(null, TrustAllStrategy.INSTANCE)
            .build();
    }

    @Bean
    @Qualifier("trustingAllRestTemplate")
    public RestTemplate trustingAllRestTemplate(@Qualifier("trustingAllSslContext") SSLContext trustingSslContext) {
        val httpClient = custom()
            .setSSLContext(trustingSslContext)
            .setSSLHostnameVerifier(NoopHostnameVerifier.INSTANCE).build();

        val requestFactory = new HttpComponentsClientHttpRequestFactory(httpClient);
        return new RestTemplate(requestFactory);
    }

}
