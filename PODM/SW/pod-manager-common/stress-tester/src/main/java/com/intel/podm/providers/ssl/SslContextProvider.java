/*
 * Copyright (c) 2016-2017 Intel Corporation
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

package com.intel.podm.providers.ssl;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import javax.net.ssl.SSLContext;
import javax.net.ssl.TrustManager;
import javax.net.ssl.X509TrustManager;
import java.security.KeyManagementException;
import java.security.NoSuchAlgorithmException;
import java.security.cert.X509Certificate;

import static javax.net.ssl.SSLContext.getInstance;

public class SslContextProvider {

    private static final Logger LOGGER = LoggerFactory.getLogger(SslContextProvider.class);
    private static final String SSL_PROTOCOL_TO_BE_USED = "TLSv1.2";

    public SSLContext getContext() {
        try {
            TrustManager[] trustManagers = getTrustManagers();
            SSLContext context = getInstance(SSL_PROTOCOL_TO_BE_USED);
            context.init(null, trustManagers, null);
            return context;
        } catch (NoSuchAlgorithmException | KeyManagementException e) {
            LOGGER.error("Keystore has not been initialized {}", e.getMessage());
            throw new RuntimeException("Keystore has not been initialized", e);
        }
    }

    private TrustManager[] getTrustManagers() {
        return new TrustManager[]{
                new X509TrustManager() {
                    public X509Certificate[] getAcceptedIssuers() {
                        return new X509Certificate[0];
                    }

                    public void checkClientTrusted(X509Certificate[] certs, String authType) {
                    }

                    public void checkServerTrusted(X509Certificate[] certs, String authType) {
                    }
                }
        };
    }
}
