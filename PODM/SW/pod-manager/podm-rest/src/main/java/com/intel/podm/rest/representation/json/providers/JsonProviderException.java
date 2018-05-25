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

package com.intel.podm.rest.representation.json.providers;

import java.io.IOException;

public final class JsonProviderException extends IOException {
    private static final long serialVersionUID = -3117662698154071519L;

    public JsonProviderException(Throwable cause) {
        super(cause);
    }

    public JsonProviderException() {
    }
}
