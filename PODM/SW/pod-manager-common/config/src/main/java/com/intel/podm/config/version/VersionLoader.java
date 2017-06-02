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

package com.intel.podm.config.version;

import java.util.jar.Manifest;

/**
 * Retrieves application version from 'Implementation-Version' attribute present in MANIFEST.MF.
 *
 * Since there may be a number of MANIFEST.MF files present, the war MANIFEST.MF is identified
 * based on attribute 'Implementation-Title' equal to 'podm-rest'.
 */
public class VersionLoader {

    private static final String MANIFEST_TITLE_FIELD = "Implementation-Title";
    private static final String EXPECTED_MANIFEST_TITLE = "podm-rest";
    private static final String MANIFEST_VERSION_FIELD = "Implementation-Version";
    private static final String UNKNOWN_VERSION = "Unknown";

    ManifestReader manifestReader;

    public VersionLoader() {
        this.manifestReader = new ManifestReader();
    }

    public String loadAppVersion() {
        Manifest mf = manifestReader.findManifestByEntry(MANIFEST_TITLE_FIELD, EXPECTED_MANIFEST_TITLE);
        final String mfVersion = (mf == null) ? null : mf.getMainAttributes().getValue(MANIFEST_VERSION_FIELD);
        return (mfVersion == null) ? UNKNOWN_VERSION : mfVersion;
    }
}
