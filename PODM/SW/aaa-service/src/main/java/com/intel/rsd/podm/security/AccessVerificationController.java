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

package com.intel.rsd.podm.security;

import com.intel.rsd.podm.security.verifier.AccessVerifier;
import com.intel.rsd.podm.security.verifier.AccessVerifierProvider;
import com.intel.rsd.podm.security.whitelist.WhiteListedAccessRequestsRegistry;
import com.intel.rsd.podm.services.AccessVerificationException;
import com.intel.rsd.dto.accessverifier.VerifyAccessRequest;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import static org.springframework.http.ResponseEntity.ok;

@RestController
@RequestMapping({"/"})
public class AccessVerificationController {

    private AccessVerifierProvider verifierProvider;

    private WhiteListedAccessRequestsRegistry whiteListedAccessRequestsRegistry;

    @Autowired
    public AccessVerificationController(AccessVerifierProvider verifierProvider, WhiteListedAccessRequestsRegistry whiteListedAccessRequestsRegistry) {
        this.verifierProvider = verifierProvider;
        this.whiteListedAccessRequestsRegistry = whiteListedAccessRequestsRegistry;
    }

    @PostMapping(value = "/redfish/v1/Actions/Intel.Oem.VerifyAccess")
    public ResponseEntity verifyAccess(@RequestBody VerifyAccessRequest verifyAccessRequest) {
        if (whiteListedAccessRequestsRegistry.isWhitelisted(verifyAccessRequest)) {
            return ok().build();
        } else {
            AccessVerifier accessVerifier = verifierProvider.provideFor(verifyAccessRequest);
            if (accessVerifier.verifyAccess(verifyAccessRequest)) {
                return ok().build();
            }
            throw new AccessVerificationException();
        }
    }
}
