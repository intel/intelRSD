/*
 * Copyright (c) 2015-2018 Intel Corporation
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

package com.intel.podm.security.providers;

import com.intel.podm.common.logger.Logger;
import com.intel.podm.config.base.dto.SecurityConfig.CertificateType;

import javax.enterprise.context.ApplicationScoped;
import javax.inject.Inject;
import javax.naming.NamingException;
import javax.net.ssl.KeyManager;
import javax.net.ssl.KeyManagerFactory;
import javax.net.ssl.TrustManager;
import javax.net.ssl.TrustManagerFactory;
import javax.net.ssl.X509TrustManager;
import java.security.KeyStore;
import java.security.KeyStoreException;
import java.security.NoSuchAlgorithmException;
import java.security.UnrecoverableKeyException;
import java.security.cert.X509Certificate;

import static com.intel.podm.config.base.dto.SecurityConfig.CertificateType.CLIENT;
import static com.intel.podm.config.base.dto.SecurityConfig.CertificateType.SERVER;

@ApplicationScoped
@SuppressWarnings("checkstyle:ClassFanOutComplexity")
public class SslConnectionManagersProvider {
    @Inject
    private Logger logger;

    @Inject
    private KeyStoreProvider keyStoreProvider;

    public KeyManager[] getKeyManagersArray() {
        try {
            KeyStore keyStore = loadCertificate(CLIENT);
            KeyManagerFactory keyManagerFactory = KeyManagerFactory.getInstance(KeyManagerFactory.getDefaultAlgorithm());
            keyManagerFactory.init(keyStore, getPasswordForType(CLIENT).toCharArray());
            return keyManagerFactory.getKeyManagers();
        } catch (NoSuchAlgorithmException | UnrecoverableKeyException | KeyStoreException e) {
            logger.e("Key managers array has not been initialized due to error {}", e.getMessage());
            throw new RuntimeException("Error during key managers array initialize process", e);
        }
    }

    public TrustManager[] getTrustManagersArray() {
        try {
            KeyStore trustStore = loadCertificate(SERVER);
            TrustManagerFactory trustManagerFactory = TrustManagerFactory.getInstance(TrustManagerFactory.getDefaultAlgorithm());
            trustManagerFactory.init(trustStore);
            return trustManagerFactory.getTrustManagers();
        } catch (NoSuchAlgorithmException | KeyStoreException e) {
            logger.e("Trust managers array has not been initialized due to error {}", e.getMessage());
            throw new RuntimeException("Error during trust managers array initialize process", e);
        }
    }

    public TrustManager[] getTrustManagersWhichTrustAllIssuers() {
        return new TrustManager[]{
            new X509TrustManager() {
                @Override
                public X509Certificate[] getAcceptedIssuers() {
                    return new X509Certificate[0];
                }

                @Override
                public void checkClientTrusted(X509Certificate[] certs, String authType) {
                }

                @Override
                public void checkServerTrusted(X509Certificate[] certs, String authType) {
                }
            }
        };
    }

    private KeyStore loadCertificate(CertificateType certificateType) {
        String password = getPasswordForType(certificateType);
        return keyStoreProvider.loadCertificate(password, certificateType);
    }

    private String getPasswordForType(CertificateType certificateType) {
        try {
            return PasswordProvider.INSTANCE.getPassword(certificateType);
        } catch (NamingException e) {
            logger.e("Context not found {}", e.getMessage());
            throw new RuntimeException("Context not found. Keystore password could not be retrieved.", e);
        }
    }
}
