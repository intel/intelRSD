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

import org.testng.annotations.DataProvider;
import org.testng.annotations.Test;

import java.util.jar.Attributes;
import java.util.jar.Manifest;

import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;
import static org.testng.Assert.assertEquals;

@SuppressWarnings({"checkstyle:MethodName"})
public class VersionLoaderTest {

    @Test(dataProvider = "versions")
    public void whenProperManifestIsFound_ShouldReturnVersionObtainedFromManifest(String expectedVersion) {

        Attributes mockedAttributes = mock(Attributes.class);
        when(mockedAttributes.getValue("Implementation-Version")).thenReturn(expectedVersion);

        Manifest mockedManifest = mock(Manifest.class);
        when(mockedManifest.getMainAttributes()).thenReturn(mockedAttributes);

        VersionLoader versionLoader = createVersionLoader(mockedManifest);

        String version = versionLoader.loadAppVersion();

        assertEquals(version, expectedVersion);
    }

    @DataProvider
    private Object[][] versions() {
        return new Object[][] {
            {"0.99"},
            {"1.00"},
            {"1.01"},
            {"1.1Beta"}
        };
    }

    @Test
    public void whenProperManifestIsNotFound_ShouldReturnDefaultValue() {
        Manifest manifest = null;
        VersionLoader versionLoader = createVersionLoader(manifest);

        String version = versionLoader.loadAppVersion();

        assertEquals(version, "Unknown");
    }

    @Test
    public void whenProperManifestIsFoundButDoesNotContainVersion_ShouldReturnDefaultValue() {
        Attributes mockedAttributes = mock(Attributes.class);
        when(mockedAttributes.getValue("Implementation-Version")).thenReturn(null);

        Manifest mockedManifest = mock(Manifest.class);
        when(mockedManifest.getMainAttributes()).thenReturn(mockedAttributes);

        VersionLoader versionLoader = createVersionLoader(mockedManifest);

        String version = versionLoader.loadAppVersion();

        assertEquals(version, "Unknown");
    }

    private VersionLoader createVersionLoader(Manifest manifest) {
        VersionLoader versionLoader = new VersionLoader();
        versionLoader.manifestReader = mock(ManifestReader.class);
        when(versionLoader.manifestReader.findManifestByEntry("Implementation-Title", "podm-rest")).thenReturn(manifest);
        return versionLoader;
    }
}
