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

const BaseAsset = require('../common/baseAsset').BaseAsset;

class NetworkProtocol extends BaseAsset {
    constructor(mock, serviceRoot) {
        super(mock, serviceRoot);
        this.MockGetWithDynamicBody();
    }

    GetBaseUri() {
        return `${this.context.GetBaseUri()}/NetworkProtocol`;
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': '/redfish/v1/$metadata#ManagerNetworkProtocol.ManagerNetworkProtocol',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#ManagerNetworkProtocol.v1_0_0.ManagerNetworkProtocol',
            'Id': 'NetworkProtocol',
            'Name': this.Name || 'Manager Network Protocol',
            'Description': 'Manager Network Service Status',
            'Status': this.Status || {
                'State': 'Enabled',
                'Health': 'OK'
            },
            'HostName': '',
            'FQDN': '',
            'HTTP': {
                'ProtocolEnabled': true,
                'Port': 8080
            },
            'HTTPS': {
                'ProtocolEnabled': true,
                'Port': 443
            },
            'IPMI': {
                'ProtocolEnabled': false
            },
            'SSH': {
                'ProtocolEnabled': false
            },
            'SNMP': {
                'ProtocolEnabled': false
            },
            'VirtualMedia': {
                'ProtocolEnabled': false
            },
            'SSDP': this.Ssdp || {
                'ProtocolEnabled': false
            },
            'Telnet': {
                'ProtocolEnabled': false
            },
            'KVMIP': {
                'ProtocolEnabled': false
            },
            'Oem': {}
        };
    }

    set Status(val) {
        this.status = val;
    }

    get Status() {
        return this.status;
    }

    set Name(name) {
        this.name = name;
    }

    get Name() {
        return this.name;
    }

    get Ssdp() {
        return this.ssdp;
    }

    set Ssdp(ssdp) {
        this.ssdp = ssdp;
    }
}

exports.NetworkProtocol = NetworkProtocol;