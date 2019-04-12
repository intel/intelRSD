/*!
 * @copyright
 * Copyright (c) 2015-2019 Intel Corporation
 *
 * @copyright
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * @copyright
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * @copyright
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * */

#include "psme/rest/constants/fabric.hpp"

namespace psme {
namespace rest {
namespace constants {

namespace Fabric {
const char ZONES[] = "Zones";
const char ENDPOINTS[] = "Endpoints";
const char SWITCHES[] = "Switches";
const char MAX_ZONES[] = "MaxZones";
const char FABRIC_TYPE[] = "FabricType";

}

namespace Switch {
const char DOMAIN_ID[] = "DomainID";
const char TOTAL_SWITCH_WIDTH[] = "TotalSwitchWidth";
const char IS_MANAGED[] = "IsManaged";
const char SWITCH_RESET[] = "#Switch.Reset";
const char SWITCH_RESET_ENDPOINT[] = "Switch.Reset";
const char ON[] = "On";
const char GRACEFUL_SHUTDOWN[] = "GracefulShutdown";
const char NMI[] = "Nmi";
const char GRACEFUL_RESTART[] = "GracefulRestart";
const char PUSH_POWER_BUTTON[] = "PushPowerButton";
const char INDICATOR_LED[] = "IndicatorLED";
const char PORTS[] = "Ports";
const char POWER_STATE[] = "PowerState";
const char SWITCH_TYPE[] = "SwitchType";
}


namespace Zone {
const char INVOLVED_SWITCHES[] = "InvolvedSwitches";
}

namespace Port {
const char PORT_RESET[] = "#Port.Reset";
const char PORT_RESET_ENDPOINT[] = "Port.Reset";
const char MAX_SPEED[] = "MaxSpeedGbps";
const char CURRENT_SPEED[] = "CurrentSpeedGbps";
const char WIDTH[] = "Width";
const char PORT_ID[] = "PortId";
const char PORT_PROTOCOL[] = "PortProtocol";
const char PORT_TYPE[] = "PortType";
const char PCIE_CONNECTION_ID[] = "PCIeConnectionId";
const char ASSOCIATED_ENDPOINTS[] = "AssociatedEndpoints";
const char CONNECTED_SWITCHES[] = "ConnectedSwitches";
const char CONNECTED_SWITCH_PORTS[] = "ConnectedSwitchPorts";
}

namespace Endpoint {
const char ENDPOINT_PROTOCOL[] = "EndpointProtocol";
const char PCI_ID[] = "PciId";
const char CONNECTED_ENTITIES[] = "ConnectedEntities";
const char ENTITY_ROLE[] = "EntityRole";
const char PCI_FUNCTION_NUMBER[] = "PciFunctionNumber";
const char PCI_CLASS_CODE[] = "PciClassCode";
const char ENTITY_PCI_ID[] = "EntityPciId";
const char ENTITY_LINK[] = "EntityLink";
const char HOST_RESERVATION_MEMORY_BYTES[] = "HostReservationMemoryBytes";
const char IP_TRANSPORT_DETAILS[] = "IPTransportDetails";
const char TRANSPORT_PROTOCOL[] = "TransportProtocol";
const char IPV4_ADDRESS[] = "IPv4Address";
const char IPV6_ADDRESS[] = "IPv6Address";
const char PORT[] = "Port";
const char INTERFACES[] = "Interfaces";
const char AUTHENTICATION[] = "Authentication";
const char USERNAME[] = "Username";
const char PASSWORD[] = "Password";
const char PORTS[] = "Ports";
}


}
}
}
