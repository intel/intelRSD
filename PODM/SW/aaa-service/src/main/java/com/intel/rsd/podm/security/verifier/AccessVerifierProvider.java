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

package com.intel.rsd.podm.security.verifier;

import com.intel.rsd.dto.accessverifier.VerifyAccessRequest;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.util.Set;

@Component
public class AccessVerifierProvider {

    private Set<AccessVerifier> accessVerifiers;

    @Autowired
    public AccessVerifierProvider(Set<AccessVerifier> accessVerifiers) {
        this.accessVerifiers = accessVerifiers;
    }

    public AccessVerifier provideFor(VerifyAccessRequest verifyAccessRequest) {
        for (AccessVerifier verifier : accessVerifiers) {
            if (verifier.isApplicableFor(verifyAccessRequest)) {
                return verifier;
            }
        }

        return request -> false;
    }
}
