/*
 * Copyright (c) 2018-2019 Intel Corporation
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

package com.intel.rsd.service.detector.ssdp;

import com.intel.rsd.service.detector.data.RemoteServiceType;
import org.fourthline.cling.model.ValidationException;
import org.fourthline.cling.model.meta.RemoteDevice;
import org.fourthline.cling.model.meta.RemoteDeviceIdentity;

public class RsdDevice extends RemoteDevice {
    private RemoteServiceType remotedeviceType;

    public RsdDevice(RemoteDeviceIdentity identity, RemoteServiceType remotedeviceType) throws ValidationException {
        super(identity);
        this.remotedeviceType = remotedeviceType;
    }

    public RemoteServiceType getRemoteDeviceType() {
        return remotedeviceType;
    }
}
