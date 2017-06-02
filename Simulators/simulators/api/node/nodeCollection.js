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
const Collection = require('./../common/collection').Collection;
const System = require('./../system/system').System;
const httpStatusCode = require('../../utils/httpStatusCode').httpStatusCode;

class NodeCollection extends Collection {
    constructor(mock, options){
        super(mock, options);
        this.MockBehaviours();
    }

    InitProperties() {
        this.count = 0;
        this.collectionMembers = [];
        this.collectionTemplate = () => {
            return {
                '@odata.context': this.options['@odata.context'] || '',
                '@odata.id': this.options['@odata.id'] || '',
                '@odata.type': this.options['@odata.type'],
                'Name': this.options['Name'] || 'Collection',
                'Description': this.options['Description'],
                'Members@odata.count': this.Size(),
                'Members': this.GetODataIdsCollection(),
                'Actions': {
                    '#ComposedNodesCollection.Allocate': {
                        'target': '/redfish/v1/Nodes/Actions/Allocate'
                    }
                }
            };
        };
        this.mock.mockGet(this.options['@odata.id'], httpStatusCode.OK, undefined, (req, response) => {
            response.body = this.collectionTemplate();
            return response;
        });
    }

    AddNode(body) {
        var node = this.Add();

        if(body.NetworkInterfaces && body.NetworkInterfaces[0].VLANs) {
            this.SetNodeVlans(body, node)
        }


        return node;
    }

    SetNodeVlans(body, node) {
        var ports = this.Context.EthernetSwitches.GetElements()[0].Ports.GetElements();
        var port = ports[0];

        // choosing correct port
        if (node.EthernetInterfaces != null) {
            for (let selectedPort of ports) {
                if (selectedPort.NeighborMAC != null && selectedPort.NeighborMAC === node.EthernetInterfaces.GetElements().MACAddress) {
                    port = selectedPort;
                    break;
                }
            }
        }


        // removing all unnecessary vlans
        var requiredVlans = [ 10, 100 ];
        var vlansToRemove = [];

        if (port.vlanCollection != null) {
            for (let portVlan of  port.vlanCollection.GetElements()) {
                if (!(requiredVlans.indexOf(portVlan.VlanId) > -1)) {
                    vlansToRemove.push(portVlan.GetBaseUri());
                }
            }

            for (let vlanToRemove of vlansToRemove) {
                port.vlanCollection.RemoveWithOdataId(vlanToRemove);
            }
        }


        // Adding all vlans to selected port
        for (let vlan of body.NetworkInterfaces[0].VLANs) {
            var newVlan = port.AddVlan();
            newVlan.VlanId = vlan.VLANId;
            newVlan.VLANEnable = vlan.VLANEnable;
            newVlan.Tagged = vlan.Tagged;
            newVlan.Name = vlan.Name;
            newVlan.Description = vlan.Description;
        }
    }

    MockBehaviours(){
        this.mock.mockPost(this.options['@odata.id'] + '/Actions/Allocate', httpStatusCode.CREATED, undefined, (req, response) => {
            var body = JSON.parse(req.body);
            var resource = this.AddNode(body);

            response.headers = {'Location': this.mock.getBaseUri() + resource.GetBaseUri()};

            this.mock.mockDelete(resource.GetBaseUri(), httpStatusCode.NO_CONTENT, undefined, (req, response) => {
                var index = this.collectionMembers.map((e) => e.GetBaseUri()).indexOf(resource.GetBaseUri());

                if (index > -1) {
                    this.collectionMembers.splice(index, 1);
                }
                return response;
            });

            return response;
        });
    }
}

exports.NodeCollection = NodeCollection;
