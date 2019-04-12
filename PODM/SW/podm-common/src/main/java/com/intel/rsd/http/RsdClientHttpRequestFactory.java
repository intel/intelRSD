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

package com.intel.rsd.http;

import com.intel.rsd.http.config.KeyStoreConfig;
import com.intel.rsd.http.config.TrustStoreConfig;
import lombok.SneakyThrows;
import lombok.extern.slf4j.Slf4j;
import lombok.val;
import org.apache.http.client.config.RequestConfig;
import org.apache.http.conn.ssl.NoopHostnameVerifier;
import org.apache.http.conn.ssl.SSLConnectionSocketFactory;
import org.apache.http.conn.ssl.TrustAllStrategy;
import org.apache.http.impl.client.HttpClients;
import org.apache.http.ssl.SSLContextBuilder;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Bean;
import org.springframework.http.client.HttpComponentsClientHttpRequestFactory;

import java.io.FileInputStream;
import java.security.KeyStore;

@Slf4j
@SuppressWarnings("checkstyle:ClassFanOutComplexity")
public class RsdClientHttpRequestFactory {
    @Bean
    @SneakyThrows
    @RsdClient
    @SuppressWarnings("checkstyle:ParameterNumber")
    HttpComponentsClientHttpRequestFactory rsdClientHttpRequestFactory(@RsdClient SSLContextBuilder sslContextBuilder,
                                                                       @Value("${http.client.socket-timeout:10000}") int socketTimeout,
                                                                       @Value("${http.client.connect-timeout:2000}") int connectTimeout,
                                                                       @Value("${http.client.connect-request-timeout:1000}") int connectRequestTimeout,
                                                                       @Value("${http.client.max-conn-total:1500}") int maxConnTotal,
                                                                       @Value("${http.client.max-conn-per-route:100}") int maxConnPerRoute) {


        val sslContext = sslContextBuilder.build();
        val sslConnectionSocketFactory = new SSLConnectionSocketFactory(sslContext, NoopHostnameVerifier.INSTANCE);

        val requestConfig = RequestConfig.custom()
            .setSocketTimeout(socketTimeout)
            .setConnectTimeout(connectTimeout)
            .setConnectionRequestTimeout(connectRequestTimeout).build();

        val httpClient = HttpClients.custom()
            .setSSLSocketFactory(sslConnectionSocketFactory)
            .setMaxConnTotal(maxConnTotal)
            .setMaxConnPerRoute(maxConnPerRoute)
            .setDefaultRequestConfig(requestConfig)
            .build();

        return new HttpComponentsClientHttpRequestFactory(httpClient);
    }

    @Bean
    @RsdClient
    SSLContextBuilder rsdClientSslContextBuilder(TrustStoreConfig trustStoreConfig, KeyStoreConfig keyStoreConfig) {

        log.info("Initializing rsdClientSslContextBuilder with config(trustedCertificatesConfig: {}, clientCertificateConfig: {})",
            trustStoreConfig,
            keyStoreConfig
        );

        val sslContextBuilder = new SSLContextBuilder();

        configureTrustMaterial(sslContextBuilder, trustStoreConfig);
        configureKeyMaterial(sslContextBuilder, keyStoreConfig);

        return sslContextBuilder;
    }

    @SneakyThrows
    private void configureKeyMaterial(SSLContextBuilder sslContextBuilder, KeyStoreConfig keyStoreConfig) {
        if (clientCertificateConfigIsSet(keyStoreConfig)) {
            log.info("Client certificate configuration detected, initializing HTTP Client with JKS file: {}", keyStoreConfig.getPath());
            val keyStorePath = keyStoreConfig.getPath().getFile().toPath().toString();
            val keyPassword = keyStoreConfig.getPassword().toCharArray();
            val keyStore = getKeyStore(keyStorePath, keyPassword);
            sslContextBuilder.loadKeyMaterial(keyStore, keyPassword, (aliases, socket) -> keyStoreConfig.getAlias());
        }
    }

    @SneakyThrows
    private void configureTrustMaterial(SSLContextBuilder sslContextBuilder, TrustStoreConfig trustStoreConfig) {

        if (trustStoreConfig.getPath() != null && trustStoreConfig.getPassword() != null) {
            log.info("TrustStore configuration detected, initializing HTTP Client with {}", trustStoreConfig.getPath());
            val trustStorePath = trustStoreConfig.getPath().getFile().toPath().toString();
            val trustStorePasswordAsCharArray = trustStoreConfig.getPassword().toCharArray();
            val trustStore = getKeyStore(trustStorePath, trustStorePasswordAsCharArray);
            sslContextBuilder.loadTrustMaterial(trustStore, null);
        } else {
            log.info("Configuring RSD client which trust all certificates.");
            sslContextBuilder.loadTrustMaterial(null, TrustAllStrategy.INSTANCE);
        }
    }

    private boolean clientCertificateConfigIsSet(KeyStoreConfig keyStoreConfig) {
        return keyStoreConfig.getPath() != null
            && keyStoreConfig.getPassword() != null
            && keyStoreConfig.getAlias() != null;
    }

    @SneakyThrows
    private KeyStore getKeyStore(String path, char[] password) {
        val keyStore = KeyStore.getInstance(KeyStore.getDefaultType());
        keyStore.load(new FileInputStream(path), password);
        return keyStore;
    }
}
