/*
 * Copyright (c) 2015-2017 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

const BaseAsset = require('../common/baseAsset').BaseAsset;
const Collection = require('./../common/collection').Collection;
const NetworkDeviceFunction = require('./networkDeviceFunction').NetworkDeviceFunction;

class NetworkInterface extends BaseAsset {
    constructor(mock, system, id) {
        super(mock, system, id);
        this.CreateCollections();
        this.MockGetWithDynamicBody();
    }

    GetBaseUri() {
        return super.GetBaseUri('NetworkInterfaces');
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': '/redfish/v1/$metadata#Systems/Members/1/NetworkInterfaces/Members/$entity',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#NetworkInterface.v1_0_0.NetworkInterface',
            'Name': 'NetworkInterface',
            'Id': this.Id,
            'Description' : 'description',
            'Status': {
                'State': 'Enabled',
                'Health': 'OK'
            },
            'Links': {},
            'NetworkDeviceFunctions': {
                '@odata.id': this.GetBaseUri() + '/NetworkDeviceFunctions'
            }
        };
    }

    CreateCollections() {
        this.networkDeviceFunctionsCollection = new Collection(this.mock, {
            '@odata.context': '/redfish/v1/$metadata#Systems/Members/1/NetworkInterfaces/Members/1/NetworkDeviceFunctions/#entity',
            '@odata.id': this.GetBaseUri() + '/NetworkDeviceFunctions',
            '@odata.type': '#NetworkDeviceFunctionCollection.NetworkDeviceFunctionCollection',
            'Name': 'Network Device Function Collection',
            'Context': this,
            'Resource': NetworkDeviceFunction
        });
    }

    AddNetworkDeviceFunction() {
        return this.networkDeviceFunctionsCollection.Add();
    }

    get NetworkDeviceFunctions() {
        return this.networkDeviceFunctionsCollection;
    }
}

exports.NetworkInterface = NetworkInterface;
