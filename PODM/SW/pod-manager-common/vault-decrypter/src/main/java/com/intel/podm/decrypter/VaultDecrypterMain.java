/*
 * Copyright (c) 2015 Intel Corporation
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

public final class VaultDecrypterMain {

    private static final String KEYSTORE_PASS_PATH = "VAULT::keystore::keystore-password::1";

    private VaultDecrypterMain() {
    }

    public static void main(String[] args) {
        String password = getPassword();
        System.out.println(password);
    }

    private static String getPassword() {
        PodmVaultDecrypter decrypter = new PodmVaultDecrypter();
        return decrypter.getPasswordUnder(KEYSTORE_PASS_PATH);
    }
}
