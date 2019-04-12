/*
 * Copyright (c) 2017-2019 Intel Corporation
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

package com.intel.rsd.nodecomposer.utils.lock;

import java.lang.annotation.Retention;
import java.lang.annotation.Target;

import static java.lang.annotation.ElementType.METHOD;
import static java.lang.annotation.RetentionPolicy.RUNTIME;

@Target({METHOD})
@Retention(RUNTIME)
public @interface Lock {
    int FIVE_SECONDS_IN_MS = 5 * 1000;
    int FIVE_MINUTES_IN_MS = 5 * 60 * 1000;
    int TEN_MINUTES_IN_MS = 10 * 60 * 1000;
    int DEFAULT_ACCESS_TIMEOUT_IN_MS = FIVE_SECONDS_IN_MS;

    long accessTimeoutInMs() default DEFAULT_ACCESS_TIMEOUT_IN_MS;
}
