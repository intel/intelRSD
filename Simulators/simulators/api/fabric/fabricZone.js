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
const httpStatusCode = require('../../utils/httpStatusCode').httpStatusCode;
const OdataIdArray = require('./../common/odataIdArray').OdataIdArray;

class FabricZone extends BaseAsset {
    constructor(mock, serviceRoot, id) {
        super(mock, serviceRoot, id);
        this.MockGetWithDynamicBody();
        this.switches = new OdataIdArray();
        this.endpoints = new OdataIdArray();

        mock.mockPatch(this.GetBaseUri(),
            httpStatusCode.OK,
            undefined,
            (req, response) => {
                var jsonRequest = JSON.parse(req.body);

                var newEndpoints = jsonRequest.Endpoints;
                this.endpoints = newEndpoints;

                response.body = this.CreateJsonTemplate();
                return response;
            }
        );
    }

    GetBaseUri() {
        return super.GetBaseUri('Zones');
    }

    CreateJsonTemplate() {
        var defaultJson = {
            '@odata.context': '/redfish/v1/$metadata#Zone.Zone',
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#Zone.v1_0_0.Zone',
            'Id': this.Id,
            'Name': (this.Name || 'PCIe Zone ') + this.Id,
            'Description': 'PCIe Zone ' + this.Id,
            'Status': this.Status || {
                'State': 'Enabled',
                'Health': 'OK'
            },
            'Links': {
                'Endpoints': this.Endpoints,
                'InvolvedSwitches': this.Switches
            },
            'Oem': {}
        };
        return defaultJson;
    }

    get Status() {
        return this.status;
    }

    set Status(val) {
        this.status = val;
    }

    get Switches() {
        return this.switches;
    }

    set Switches(elements) {
        this.AddToSwitches(elements);
    }

    AddToSwitches(elements) {
        this.switches.Add(elements);
    }

    get Endpoints() {
        return this.endpoints;
    }

    set Endpoints(elements) {
        this.AddToEndpoints(elements);
    }

    AddToEndpoints(elements) {
        this.endpoints.Add(elements);
    }

    get Name() {
        return this.name;
    }

    set Name(name) {
        this.name = name;
    }

}


exports.FabricZone = FabricZone;