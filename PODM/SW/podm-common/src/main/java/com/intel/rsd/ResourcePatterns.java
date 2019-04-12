/*
 * Copyright (c) 2019 Intel Corporation
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

package com.intel.rsd;

import lombok.experimental.UtilityClass;

import java.util.regex.Pattern;

import static java.util.regex.Pattern.compile;

@UtilityClass
public class ResourcePatterns {

    public static final Pattern COMPUTER_SYSTEM_PATTERN = compile("/redfish/v1/Systems/[A-Za-z0-9-.%:+]+");
    public static final Pattern CHASSIS_PATTERN = compile("/redfish/v1/Chassis/[A-Za-z0-9-.%:+]+");
    public static final Pattern MANAGER_PATTERN = compile("/redfish/v1/Managers/(?<uuid>[A-Za-z0-9-.%:+]+)");
    public static final Pattern ETHERNET_SWITCH_PORT_PATTERN = compile("/redfish/v1/EthernetSwitches/[A-Za-z0-9-.%:+_]+/Ports/[A-Za-z0-9-.%:+/]+");
    public static final Pattern COMPUTER_SYSTEM_ETHERNET_INTERFACE_PATTERN =
        compile("/redfish/v1/Systems/[A-Za-z0-9-.%:+]+/EthernetInterfaces/[A-Za-z0-9-.%:+/]+");
    public static final Pattern ENDPOINT_PATTERN = compile("/redfish/v1/Fabrics/[A-Za-z0-9-.%:+_]+/Endpoints/[A-Za-z0-9-.%:+]+");
    public static final Pattern ACTION_PATTERN
        = compile("(?<resourcePath>/redfish/v1(/[A-Za-z0-9-.%:+]+)+)/Actions(/[A-Za-z0-9-.%:+]+)*/(?<actionName>[A-Za-z0-9-.%:+]+)");
}
