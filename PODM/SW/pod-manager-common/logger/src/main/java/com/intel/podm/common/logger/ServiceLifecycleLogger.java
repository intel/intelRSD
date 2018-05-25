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

package com.intel.podm.common.logger;

public interface ServiceLifecycleLogger extends Logger {

    /**
     * Log at INFO level about service lifecycle (with marker [ServiceLifecycle])
     * @param msg
     */
    void lifecycleInfo(String msg);

    /**
     * Log at INFO level about service lifecycle (with marker [ServiceLifecycle])
     * @param format
     * @param arguments
     */
    void lifecycleInfo(String format, Object... arguments);
}
