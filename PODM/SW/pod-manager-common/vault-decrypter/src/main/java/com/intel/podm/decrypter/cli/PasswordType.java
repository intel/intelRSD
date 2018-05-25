/*
 * Copyright (c) 2016-2018 Intel Corporation
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

package com.intel.podm.decrypter.cli;

public enum PasswordType {
    KEYSTORE("keystore", "VAULT::keystore::keystore-password::1"),
    KEYSTORE_KEY("key", "VAULT::keystore::key-password::1"),
    DATABASE("database", "VAULT::database::password::1");

    private final String name;
    private final String passPath;

    PasswordType(String name, String passPath) {
        this.name = name;
        this.passPath = passPath;
    }

    public String getName() {
        return name;
    }

    public String getPassPath() {
        return passPath;
    }

    public static PasswordType fromString(String text) {
        if (text != null) {
            for (PasswordType passwordType : PasswordType.values()) {
                if (text.equalsIgnoreCase(passwordType.getName())) {
                    return passwordType;
                }
            }
        }
        throw new IllegalArgumentException("Provided password type: [" + text + "] is not valid.");
    }
}
