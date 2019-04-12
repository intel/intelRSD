/*!
 * @copyright
 * Copyright (c) 2017-2019 Intel Corporation
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

#include "agent-framework/module/model/attributes/extended_cpu_id.hpp"
#include "agent-framework/module/constants/compute.hpp"



using namespace agent_framework::model::attribute;
using namespace agent_framework::model;


ExtendedCpuId::ExtendedCpuId() {}


ExtendedCpuId::~ExtendedCpuId() {}


json::Json ExtendedCpuId::to_json() const {
    json::Json result = json::Json();
    result[literals::ExtendedCpuId::EAX_00H] = get_eax_00h();
    result[literals::ExtendedCpuId::EAX_01H] = get_eax_01h();
    result[literals::ExtendedCpuId::EAX_02H] = get_eax_02h();
    result[literals::ExtendedCpuId::EAX_03H] = get_eax_03h();
    result[literals::ExtendedCpuId::EAX_04H] = get_eax_04h();
    result[literals::ExtendedCpuId::EAX_05H] = get_eax_05h();
    result[literals::ExtendedCpuId::EAX_06H] = get_eax_06h();
    result[literals::ExtendedCpuId::EAX_07H] = get_eax_07h();
    result[literals::ExtendedCpuId::EAX_09H] = get_eax_09h();
    result[literals::ExtendedCpuId::EAX_0aH] = get_eax_0ah();
    result[literals::ExtendedCpuId::EAX_0bH] = get_eax_0bh();
    result[literals::ExtendedCpuId::EAX_0dH] = get_eax_0dh();
    result[literals::ExtendedCpuId::EAX_0fH] = get_eax_0fh();
    result[literals::ExtendedCpuId::EAX_10H] = get_eax_10h();

    result[literals::ExtendedCpuId::EAX_14H] = get_eax_14h();
    result[literals::ExtendedCpuId::EAX_15H] = get_eax_15h();
    result[literals::ExtendedCpuId::EAX_16H] = get_eax_16h();
    result[literals::ExtendedCpuId::EAX_17H_ECX_00H] = get_eax_17h_ecx_00h();
    result[literals::ExtendedCpuId::EAX_17H_ECX_01H] = get_eax_17h_ecx_01h();
    result[literals::ExtendedCpuId::EAX_17H_ECX_02H] = get_eax_17h_ecx_02h();
    result[literals::ExtendedCpuId::EAX_17H_ECX_03H] = get_eax_17h_ecx_03h();
    result[literals::ExtendedCpuId::EAX_80000000H] = get_eax_80000000h();
    result[literals::ExtendedCpuId::EAX_80000001H] = get_eax_80000001h();
    result[literals::ExtendedCpuId::EAX_80000002H] = get_eax_80000002h();
    result[literals::ExtendedCpuId::EAX_80000003H] = get_eax_80000003h();
    result[literals::ExtendedCpuId::EAX_80000004H] = get_eax_80000004h();
    result[literals::ExtendedCpuId::EAX_80000006H] = get_eax_80000006h();
    result[literals::ExtendedCpuId::EAX_80000007H] = get_eax_80000007h();
    result[literals::ExtendedCpuId::EAX_80000008H] = get_eax_80000008h();
    return result;
}


ExtendedCpuId ExtendedCpuId::from_json(const json::Json& json) {
    ExtendedCpuId cpu_id{};

    cpu_id.set_eax_00h(json[literals::ExtendedCpuId::EAX_00H]);
    cpu_id.set_eax_01h(json[literals::ExtendedCpuId::EAX_01H]);
    cpu_id.set_eax_02h(json[literals::ExtendedCpuId::EAX_02H]);
    cpu_id.set_eax_03h(json[literals::ExtendedCpuId::EAX_03H]);
    cpu_id.set_eax_04h(json[literals::ExtendedCpuId::EAX_04H]);
    cpu_id.set_eax_05h(json[literals::ExtendedCpuId::EAX_05H]);
    cpu_id.set_eax_06h(json[literals::ExtendedCpuId::EAX_06H]);
    cpu_id.set_eax_07h(json[literals::ExtendedCpuId::EAX_07H]);
    cpu_id.set_eax_09h(json[literals::ExtendedCpuId::EAX_09H]);
    cpu_id.set_eax_0ah(json[literals::ExtendedCpuId::EAX_0aH]);
    cpu_id.set_eax_0bh(json[literals::ExtendedCpuId::EAX_0bH]);
    cpu_id.set_eax_0dh(json[literals::ExtendedCpuId::EAX_0dH]);
    cpu_id.set_eax_0fh(json[literals::ExtendedCpuId::EAX_0fH]);
    cpu_id.set_eax_10h(json[literals::ExtendedCpuId::EAX_10H]);

    cpu_id.set_eax_14h(json[literals::ExtendedCpuId::EAX_14H]);
    cpu_id.set_eax_15h(json[literals::ExtendedCpuId::EAX_15H]);
    cpu_id.set_eax_16h(json[literals::ExtendedCpuId::EAX_16H]);
    cpu_id.set_eax_17h_ecx_00h(json[literals::ExtendedCpuId::EAX_17H_ECX_00H]);
    cpu_id.set_eax_17h_ecx_01h(json[literals::ExtendedCpuId::EAX_17H_ECX_01H]);
    cpu_id.set_eax_17h_ecx_02h(json[literals::ExtendedCpuId::EAX_17H_ECX_02H]);
    cpu_id.set_eax_17h_ecx_03h(json[literals::ExtendedCpuId::EAX_17H_ECX_03H]);
    cpu_id.set_eax_80000000h(json[literals::ExtendedCpuId::EAX_80000000H]);
    cpu_id.set_eax_80000001h(json[literals::ExtendedCpuId::EAX_80000001H]);
    cpu_id.set_eax_80000002h(json[literals::ExtendedCpuId::EAX_80000002H]);
    cpu_id.set_eax_80000003h(json[literals::ExtendedCpuId::EAX_80000003H]);
    cpu_id.set_eax_80000004h(json[literals::ExtendedCpuId::EAX_80000004H]);
    cpu_id.set_eax_80000006h(json[literals::ExtendedCpuId::EAX_80000006H]);
    cpu_id.set_eax_80000007h(json[literals::ExtendedCpuId::EAX_80000007H]);
    cpu_id.set_eax_80000008h(json[literals::ExtendedCpuId::EAX_80000008H]);

    return cpu_id;
}
