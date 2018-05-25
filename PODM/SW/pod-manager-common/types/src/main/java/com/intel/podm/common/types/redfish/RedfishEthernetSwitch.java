/*
 * Copyright (c) 2018 Intel Corporation
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

import com.intel.podm.common.types.ProtocolType;

import java.util.List;

public interface RedfishEthernetSwitch extends RedfishResource {

    default Boolean getLldpEnabled() {
        return null;
    }
    default Boolean getEtsEnabled() {
        return null;
    }
    default Boolean getDcbxEnabled() {
        return null;
    }
    default Boolean getPfcEnabled() {
        return null;
    }

    default DcbxConfiguration getDcbxSharedConfiguration() {
        return null;
    }

    interface DcbxConfiguration {
        default List<? extends ApplicationProtocolType> getApplicationProtocolList() {
            return null;
        }
        default List<? extends PriorityClassMapping> getPriorityToPriorityGroupMappingList() {
            return null;
        }
        default List<? extends BandwidthMapping> getBandwidthAllocationList() {
            return null;
        }

        interface ApplicationProtocolType {
            default Long getPriority() {
                return null;
            }
            default ProtocolType getProtocol() {
                return null;
            }
            default Long getPort() {
                return null;
            }
        }

        interface PriorityClassMapping {
            default Long getPriorityGroup() {
                return null;
            }
            default Long getPriority() {
                return null;
            }
        }

        interface BandwidthMapping {
            default Long getPriorityGroup() {
                return null;
            }
            default Long getBandwidthPercent() {
                return null;
            }
        }
    }
}
