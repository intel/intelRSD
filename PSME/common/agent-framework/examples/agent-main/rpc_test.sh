#!/bin/bash

# Copyright (c) 2015-2016 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

echo "Component UUID: "$1
echo "IP: "$2
echo "PORT: "$3
echo
echo "getManagerInfo:"
curl -d '{"jsonrpc": "2.0", "method": "getManagerInfo", "params": {"id": "'$1'"}, "id":1}' $2:$3
echo
echo "getComponentInfo:"
curl -d '{"jsonrpc": "2.0", "method": "getComponentInfo", "params": {"id": "'$1'"}, "id":1}' $2:$3
echo
echo "getMemorySlotCount:"
curl -d '{"jsonrpc": "2.0", "method": "getMemorySlotCount", "params": {"id": "'$1'"}, "id":1}' $2:$3
echo
echo "getMemoryInfo:"
curl -d '{"jsonrpc": "2.0", "method": "getMemoryInfo", "params": {"id": "'$1'", "slot": 1}, "id":1}' $2:$3
echo
echo "getProcessorSlotCount:"
curl -d '{"jsonrpc": "2.0", "method": "getProcessorSlotCount", "params": {"id": "'$1'"}, "id":1}' $2:$3
echo
echo "getProcessorInfo:"
curl -d '{"jsonrpc": "2.0", "method": "getProcessorInfo", "params": {"id": "'$1'", "slot": 0}, "id":1}' $2:$3
