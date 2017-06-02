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

const BaseAsset = require('../../common/baseAsset').BaseAsset;
const httpStatusCode = require('../../../utils/httpStatusCode').httpStatusCode;

class Drawer extends BaseAsset {
    constructor (mock, rack, id) {
        super(mock, rack, id);
        this.MockGetWithDynamicBody();
        this.EnablePatch(mock);
    }

    GetBaseUri () {
        return super.GetBaseUri('Chassis');
    }

    CreateJsonTemplate () {
        return {
            '@odata.context': `/redfish/v1/$metadata#Chassis/${this.Id}`,
            '@odata.id': this.GetBaseUri(),
            '@odata.type': '#Chassis.1.0.0.Chassis',
            'ChassisType': this.ChassisType,
            'Oem': {
                'Intel_RackScale': {
                    '@odata.type': '#Intel.Oem.RmmDrawerChassis',
                    'DrawerRUID': this.DrawerRUID || 1,
                    'Location': {
                        'RackUnits': 'OU',
                        'XLocation': 0,
                        'ULocation': 3,
                        'UHeight': 2
                    }
                }
            },
            'Links': {
                'Oem': {
                    'Intel_RackScale': {
                        'ContainedBy': {
                            '@odata.id': this.ContainedBy
                        }
                    }
                }
            }
        };
    };

    EnablePatch (mock) {
        var self = this;
        mock.mockPatch(self.GetBaseUri(),
            httpStatusCode.OK,
            undefined,
            function (req, response) {
                var jsonRequest = JSON.parse(req.body);
                var newDrawerRuid = jsonRequest.Oem.Intel_RackScale.DrawerRUID;

                if (newDrawerRuid !== undefined) {
                    self.DrawerRUID = newDrawerRuid;
                }
                response.body = self.CreateJsonTemplate();
                return response;
            }
        );
    }

    get DrawerRUID () {
        return this.drawerRUID;
    }

    set DrawerRUID (drawerRUID) {
        this.drawerRUID = drawerRUID;
    }

    get ChassisType () {
        return 'Drawer';
    }

    get ContainedBy () {
        return this.containedBy || '';
    }

    set ContainedBy (url) {
        this.containedBy = url;
    }

    BindToRack (rack) {
        rack.AddContains(this);
        this.ContainedBy = rack.GetBaseUri();
        return this;
    }
}

exports.Drawer = Drawer;
