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

package com.intel.rsd.resourcemanager.layers.queryparameter;

import static org.apache.commons.lang3.RandomStringUtils.random;

public class QueryParameterConstants {
    public static final String REDFISH_MEMBERS = "Members";
    public static final String REDFISH_ODATA_ID = "@odata.id";
    public static final String REDFISH_MEMBERS_COUNT = "Members@odata.count";
    public static final String REDFISH_ALLOWABLE_VALUES = "@Redfish.AllowableValues";
    public static final String RANDOM_STRING = random(10, true, false);
}
