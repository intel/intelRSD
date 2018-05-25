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

package com.intel.podm.business;

import com.intel.podm.business.services.context.Context;

import static java.lang.String.format;

public final class ContextResolvingException extends BusinessApiException {
    private static final long serialVersionUID = -7472189531358360267L;
    private final Context context;

    public ContextResolvingException(String message) {
        super(message);
        this.context = null;
    }

    public ContextResolvingException(Context context) {
        super(format("Context cannot be resolved: %s", context));
        this.context = context;
    }

    public ContextResolvingException(String message, Context context, Throwable cause) {
        super(message, cause);
        this.context = context;
    }

    public Context getContext() {
        return context;
    }
}
