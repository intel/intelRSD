/*
 * Copyright (c) 2015 Intel Corporation
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

package com.intel.podm.config;

import javax.annotation.PostConstruct;
import javax.ejb.Singleton;
import java.io.IOException;

import static java.nio.file.Files.deleteIfExists;
import static java.nio.file.Paths.get;

@Singleton
public class DatabaseCleanupConfig {

    private static final String DB_CLEANUP_MARKER_FILE = "/var/lib/pod-manager/clear-db-on-next-startup";

    private boolean dbCleanupRequested;

    @PostConstruct
    private void init() {
        try {
            dbCleanupRequested = deleteIfExists(get(DB_CLEANUP_MARKER_FILE));
        } catch (IOException e) {
            throw new Error("Application failed to start properly. Database cleanup marker file cannot be removed.");
        }
    }

    public boolean isDbCleanupRequested() {
        return dbCleanupRequested;
    }
}
