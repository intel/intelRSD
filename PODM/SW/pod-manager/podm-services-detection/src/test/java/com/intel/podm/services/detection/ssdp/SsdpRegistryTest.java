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

package com.intel.podm.services.detection.ssdp;

import org.fourthline.cling.UpnpService;
import org.fourthline.cling.mock.MockUpnpService;
import org.fourthline.cling.mock.MockUpnpServiceConfiguration;
import org.fourthline.cling.model.ValidationException;
import org.fourthline.cling.model.meta.RemoteDevice;
import org.fourthline.cling.model.meta.RemoteDeviceIdentity;
import org.fourthline.cling.model.types.UDN;
import org.fourthline.cling.registry.RegistryListener;
import org.testng.annotations.BeforeMethod;
import org.testng.annotations.Test;

import static java.util.UUID.randomUUID;
import static java.util.concurrent.TimeUnit.MILLISECONDS;
import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.Matchers.equalTo;
import static org.hamcrest.collection.IsEmptyCollection.empty;
import static org.mockito.Matchers.any;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.timeout;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;


public class SsdpRegistryTest {

    public static final int CACHE_CONTROL = 1000;
    public static final int REGISTRY_MAINTENENCE_INTERVAL = 100;
    private static final int OPERATION_TIMEOUT = 5000;

    private UpnpService service;
    private RegistryListener listener;

    @BeforeMethod
    public void setUp() throws Exception {
        service = initSsdpService();
        listener = createRegistryListener(service);
    }

    @Test
    public void registryShouldBeEmptyAfterCreation() {
        assertThat("Registry should be empty", service.getRegistry().getRemoteDevices(), empty());
    }

    @Test
    public void registryIsRegisteringRemoteDevices() throws ValidationException {
        RemoteDevice remoteDevice = createDevice(CACHE_CONTROL);
        service.getRegistry().addDevice(remoteDevice);
        assertThat("Registry should contain exactly one device", service.getRegistry().getRemoteDevices().size(), equalTo(1));
    }

    @Test
    public void secondAdditionOfAlreadyExistingDeviceShouldBeAnUpdate() throws ValidationException {
        RemoteDevice remoteDevice = createDevice(CACHE_CONTROL);
        service.getRegistry().addDevice(remoteDevice);
        service.getRegistry().addDevice(remoteDevice);
        assertThat("Registry should contain exactly one device", service.getRegistry().getRemoteDevices().size(), equalTo(1));
        verify(listener, times(1)).remoteDeviceUpdated(any(), any());
    }

    @Test
    public void ifCacheControlIsExceededThenDeviceShouldBeRemoved() throws Exception {
        RemoteDevice remoteDevice = createDevice(CACHE_CONTROL);
        service.getRegistry().addDevice(remoteDevice);
        verify(listener, timeout(OPERATION_TIMEOUT).times(1)).remoteDeviceRemoved(any(), any());
    }

    @Test
    public void registyUpdatePreventsDeviceDeletion() throws Exception {
        RemoteDevice remoteDevice = createDevice(CACHE_CONTROL);
        service.getRegistry().addDevice(remoteDevice);
        exceed(CACHE_CONTROL / 2);

        service.getRegistry().update(remoteDevice.getIdentity());
        exceed(CACHE_CONTROL / 2);

        service.getRegistry().update(remoteDevice.getIdentity());
        exceed(CACHE_CONTROL / 2);

        verify(listener, timeout(OPERATION_TIMEOUT).times(1)).remoteDeviceRemoved(any(), any());
    }

    private RemoteDevice createDevice(int cacheControlInMillis) throws ValidationException {
        RemoteDeviceIdentity identity = new RemoteDeviceIdentity(
                new UDN(randomUUID()),
                (int) MILLISECONDS.toSeconds(cacheControlInMillis), null, null, null
        );
        return new RemoteDevice(identity);
    }

    private MockUpnpService initSsdpService() {
        return new MockUpnpService(new MockUpnpServiceConfiguration(true, false) {
            @Override
            public int getRegistryMaintenanceIntervalMillis() {
                return REGISTRY_MAINTENENCE_INTERVAL;
            }
        });
    }

    private RegistryListener createRegistryListener(UpnpService service) {
        RegistryListener listener = mock(RegistryListener.class);
        service.getRegistry().addListener(listener);
        return listener;
    }

    private void exceed(long cacheControlInMillis) throws InterruptedException {
        MILLISECONDS.sleep(cacheControlInMillis);
    }
}
