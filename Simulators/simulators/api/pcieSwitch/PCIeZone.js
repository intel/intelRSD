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
const OdataIdArray = require('./../common/odataIdArray').OdataIdArray;
const httpStatusCode = require('../../utils/httpStatusCode').httpStatusCode;

class PCIeZone extends BaseAsset {
    constructor(mock, pcieSwitch, id) {
        super(mock, pcieSwitch, id);
        this.MockGetWithDynamicBody();
        this.MockBehaviours();

        this.ports = new OdataIdArray();
        this.deviceFunctions = new OdataIdArray();
        this.attachFunctionAllowableValues = new OdataIdArray();
        this.detachFunctionAllowableValues = new OdataIdArray();
    }

    GetBaseUri() {
        return super.GetBaseUri('PCIeZones');
    }

    CreateJsonTemplate() {
        return {
            '@odata.context': '/redfish/v1/$metadata#PCIeSwitches/Members/1/PCIeZones/Members$entity',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#PCIeZone.v0_9_0.PCIeZone',
            'Id': this.Id,
            'Name': 'PCIe Zone',
            'Description': 'PCIe Zone with PCIe Ports',
            'Status': {
                'State': 'Enabled',
                'Health': 'OK'
            },
            'Links': {
                'PortsInZone': this.Ports,
                'DeviceFunctionsInZone': this.DeviceFunctions
            },
            'Oem': {},
            'Actions': {
                'Oem': {
                    'Intel_RackScale': {
                        '#PCIeZone.AttachFunction': {
                            'target': '/redfish/v1/PCIeSwitches/1/PCIeZones/1/Actions/Oem/PCIeZone.AttachFunction',
                            'Function@Redfish.AllowableValues': this.AttachFunctionAllowableValues
                        },
                        '#PCIeZone.DetachFunction': {
                            'target': '/redfish/v1/PCIeSwitches/1/PCIeZones/1/Actions/Oem/PCIeZone.DetachFunction',
                            'Function@Redfish.AllowableValues': this.DetachFunctionAllowableValues
                        }
                    }
                }
            }
        };
    }

    MockBehaviours() {
        let self = this;

        this.mock.mockPost(self.GetBaseUri() + '/Actions/Oem/PCIeZone.AttachFunction',
            httpStatusCode.NO_CONTENT,
            undefined,
            function (req, response) {
                var jsonRequest = JSON.parse(req.body);
                var deviceFunction = jsonRequest.Function['@odata.id'];
                self.deviceFunctions.AddWithODataId(deviceFunction);
                self.detachFunctionAllowableValues.AddWithODataId(deviceFunction);
                self.attachFunctionAllowableValues.Remove(deviceFunction);
                return response;
            }
        );

        this.mock.mockPost(self.GetBaseUri() + '/Actions/Oem/PCIeZone.DetachFunction',
            httpStatusCode.NO_CONTENT,
            undefined,
            function (req, response) {
                var jsonRequest = JSON.parse(req.body);
                var deviceFunction = jsonRequest.Function['@odata.id'];
                self.deviceFunctions.Remove(deviceFunction);
                self.detachFunctionAllowableValues.Remove(deviceFunction);
                self.attachFunctionAllowableValues.AddWithODataId(deviceFunction);
                return response;
            }
        );
    }

    get Ports() {
        return this.ports;
    }

    AddPort(port) {
        this.ports.Add(port);
    }

    get DeviceFunctions() {
        return this.deviceFunctions;
    }

    AddDeviceFunctions(deviceFunction) {
        this.deviceFunctions.Add(deviceFunction);
    }

    get AttachFunctionAllowableValues() {
        return this.attachFunctionAllowableValues;
    }

    AddAttachFunctionAllowableValues(deviceFunction) {
        this.attachFunctionAllowableValues.Add(deviceFunction);
    }

    get DetachFunctionAllowableValues() {
        return this.detachFunctionAllowableValues;
    }

    AddDetachFunctionAllowableValues(deviceFunction) {
        this.detachFunctionAllowableValues.Add(deviceFunction);
    }
}

exports.PCIeZone = PCIeZone;
