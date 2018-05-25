/*
 * Copyright (c) 2017-2018 Intel Corporation
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

package com.intel.podm.business.entities.redfish;

public final class ComputerSystemQueries {

    public static final String GET_COMPUTER_SYSTEM_IDS_FROM_PRIMARY_DATA_SOURCE = "SELECT computerSystem.entityId "
        + "FROM ComputerSystem computerSystem WHERE computerSystem.isComplementary = false";

    public static final String GET_PRIMARY_COMPUTER_SYSTEM = "SELECT computerSystem "
        + "FROM ComputerSystem computerSystem "
        + "WHERE computerSystem.uuid = :uuid "
        + "AND computerSystem.isComplementary = false "
        + "AND computerSystem.systemType != :systemTypeToBeExcluded";

    public static final String GET_COMPUTER_SYSTEM_MULTI_SOURCE = "SELECT computerSystem "
        + "FROM ComputerSystem computerSystem "
        + "WHERE computerSystem.isComplementary = :isComplementary "
        + "AND computerSystem.uuid = :uuid";

    public static final String GET_COMPUTER_SYSTEMS_AVAILABLE_TO_ALLOCATE = "SELECT computerSystem "
        + "FROM ComputerSystem computerSystem "
        + "WHERE computerSystem.metadata.allocated = :allocated "
        + "AND computerSystem.isComplementary = :isComplementary "
        + "AND computerSystem.systemType = :systemType";

    public static final String GET_COMPUTER_SYSTEMS_MATCHING_CONNECTION_ID = "SELECT computerSystem "
        + "FROM ComputerSystem computerSystem "
        + "WHERE :pcieConnectionId MEMBER computerSystem.pcieConnectionIds";

    public static final String GET_PHYSICAL_COMPUTER_SYSTEM_BY_UUID = "SELECT computerSystem "
        + "FROM ComputerSystem computerSystem "
        + "WHERE computerSystem.uuid = :uuid "
        + "AND computerSystem.systemType = :systemType";

    private ComputerSystemQueries() {
    }
}
