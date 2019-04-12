/*
 * Copyright (c) 2016-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.business;

import org.apache.commons.lang3.builder.ToStringBuilder;

public final class RequestValidationException extends BusinessApiException implements ViolationsDisclosingException {
    private static final long serialVersionUID = 1541341901703650235L;
    private final Violations violations = new Violations();

    public RequestValidationException(String message, Violations violations, Throwable throwable) {
        super(message, throwable);
        this.violations.addAll(violations);
    }

    public RequestValidationException(Violations violations) {
        super("Invalid request");
        this.violations.addAll(violations);
    }

    @Override
    public Violations getViolations() {
        return violations;
    }

    @Override
    public String toString() {
        return new ToStringBuilder(this)
            .append(violations)
            .toString();
    }
}
