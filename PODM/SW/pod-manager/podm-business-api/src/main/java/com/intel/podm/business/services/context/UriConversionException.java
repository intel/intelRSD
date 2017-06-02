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

package com.intel.podm.business.services.context;

import java.net.URI;

public class UriConversionException extends RuntimeException {
    private static final long serialVersionUID = -4359629537780290559L;

    private final URI sourceUri;
    private final ContextType[] targetContextTypes;

    public UriConversionException(String message, URI sourceUri, ContextType[] targetContextTypes) {
        super(message);
        this.sourceUri = sourceUri;
        this.targetContextTypes = targetContextTypes;
    }

    public URI getSourceUri() {
        return sourceUri;
    }

    public ContextType[] getTargetContextTypes() {
        return targetContextTypes;
    }
}
