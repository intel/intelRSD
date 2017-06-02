/*
 * Copyright (c) 2015-2017 Intel Corporation
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

package com.intel.podm.decrypter;

import com.intel.podm.reader.VaultPropertiesReader;
import org.jboss.security.vault.SecurityVault;
import org.jboss.security.vault.SecurityVaultException;
import org.jboss.security.vault.SecurityVaultFactory;
import org.jboss.security.vault.SecurityVaultUtil;

import java.util.Map;

public class PodmVaultDecrypter {

    private static final String VAULT_CONFIG = "/var/lib/pod-manager/vault/vault.json";

    public String getPasswordUnder(String vaultPropertyPath) {

        try {
            Map<String, Object> options = new VaultPropertiesReader().getPropertiesMap(VAULT_CONFIG);

            SecurityVault vault = SecurityVaultFactory.get();
            vault.init(options);

            return SecurityVaultUtil.getValueAsString(vaultPropertyPath);
        } catch (SecurityVaultException e) {
            throw new RuntimeException("Unable to read password from vault", e);
        }
    }
}
