/*
 * Copyright (c) 2017 Intel Corporation
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

package com.intel.podm.common.types.redfish;

import com.intel.podm.common.types.ChapType;
import com.intel.podm.common.types.Ref;

import java.util.List;

public interface RedfishRemoteTarget extends RedfishResource {

    List<Address> getAddresses();

    List<Initiator> getInitiator();

    interface Address {

        IScsiAddress getIScsiAddresses();

        interface IScsiAddress {
            Ref<Chap> getChap();

            interface Chap {
                Ref<ChapType> getType();

                Ref<String> getUsername();

                Ref<String> getSecret();

                Ref<String> getMutualUsername();

                Ref<String> getMutualSecret();
            }
        }
    }

    interface Initiator {

        IScsiInitiator getIScsiInitators();

        interface IScsiInitiator {
            Ref<String> getInitiatorIqn();
        }
    }
}
