/*
 * Copyright (c) 2018 Intel Corporation
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

package com.intel.podm.common.types;

public class Chap {
    private Ref<String> username;
    private Ref<String> secret;
    private Ref<String> mutualUsername;
    private Ref<String> mutualSecret;

    public Ref<String> getUsername() {
        return username;
    }

    public void setUsername(Ref<String> username) {
        this.username = username;
    }

    public Ref<String> getSecret() {
        return secret;
    }

    public void setSecret(Ref<String> secret) {
        this.secret = secret;
    }

    public Ref<String> getMutualUsername() {
        return mutualUsername;
    }

    public void setMutualUsername(Ref<String> mutualUsername) {
        this.mutualUsername = mutualUsername;
    }

    public Ref<String> getMutualSecret() {
        return mutualSecret;
    }

    public void setMutualSecret(Ref<String> mutualSecret) {
        this.mutualSecret = mutualSecret;
    }
}
