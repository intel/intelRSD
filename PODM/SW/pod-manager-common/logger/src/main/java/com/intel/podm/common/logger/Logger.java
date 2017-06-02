/*
 * Copyright (c) 2015-2017 Intel Corporation
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

/**
 * Simplified logging interface.
 */
public interface Logger {
    /**
     * Log at TRACE level
     * @param msg
     */
    void t(String msg);

    /**
     * Log at TRACE level
     * @param format
     * @param arguments
     */
    void t(String format, Object... arguments);

    /***
     * Log at DEBUG level
     * @param msg
     */
    void d(String msg);

    /**
     * Log at DEBUG level
     * @param format
     * @param arguments
     */
    void d(String format, Object... arguments);

    /**
     * Log at INFO level
     * @param msg
     */
    void i(String msg);

    /**
     * Log at INFO level
     * @param format
     * @param arguments
     */
    void i(String format, Object... arguments);

    /**
     * Log at WARN level
     * @param msg
     */
    void w(String msg);

    /**
     * Log at WARN level
     * @param format
     * @param arguments
     */
    void w(String format, Object... arguments);

    /**
     * Log at ERROR level
     * @param msg
     */
    void e(String msg);

    /**
     * Log at ERROR level
     * @param format
     * @param arguments
     */
    void e(String format, Object... arguments);

    /**
     * Log at ERROR level
     * @param msg
     * @param t
     */
    void e(String msg, Throwable t);
}
