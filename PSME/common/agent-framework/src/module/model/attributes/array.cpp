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
 *
 * @file command/collection.cpp
 * @brief Arrays specific data
 * */

#include "agent-framework/module/utils/is_framework_enum.hpp"
#include "agent-framework/module/utils/is_framework_object.hpp"
#include "agent-framework/module/model/attributes/model_attributes.hpp"
#include "agent-framework/module/model/metric.hpp"



using namespace agent_framework::model;
using namespace agent_framework::model::attribute;

namespace {

template<typename T>
using is_f_enum = utils::is_framework_enum<T>;

template<typename T>
using is_f_obj = utils::is_framework_object<T>;


// empty template if neither framework enum or object
template<typename T, typename std::enable_if<(!is_f_obj<T>::value) && (!is_f_enum<T>::value)>::type* = nullptr>
void append_to_json(json::Json&, const T&) {
    // this should not be used, provide proper specialization instead
    assert(false);
}


// append framework enum to json
template<typename T, typename std::enable_if<is_f_enum<T>::value>::type* = nullptr>
void append_to_json(json::Json& json, const T& entry) {
    json.emplace_back(entry.to_string());
}


// append framework object to json
template<typename T, typename std::enable_if<is_f_obj<T>::value>::type* = nullptr>
void append_to_json(json::Json& json, const T& entry) {
    json.emplace_back(entry.to_json());
}


// append string to json
template<>
void append_to_json<std::string>(json::Json& json, const std::string& entry) {
    json.emplace_back(entry);
}


// append double to json
template<>
void append_to_json<double>(json::Json& json, const double& entry) {
    json.emplace_back(entry);
}


// append uint32_t to json
template<>
void append_to_json<std::uint32_t>(json::Json& json, const std::uint32_t& entry) {
    json.emplace_back(entry);
}


// empty template if neither framework enum or object
template<typename T, typename std::enable_if<(!is_f_obj<T>::value) && (!is_f_enum<T>::value)>::type* = nullptr>
void append_to_array(Array<T>&, const json::Json&) {
    // this should not be used, provide proper specialization instead
    assert(false);
}


// append framework enum from json
template<typename T, typename std::enable_if<is_f_enum<T>::value>::type* = nullptr>
void append_to_array(Array<T>& array, const json::Json& json) {
    array.add_entry(T::from_string(json));
}


// append framework object from json
template<typename T, typename std::enable_if<is_f_obj<T>::value>::type* = nullptr>
void append_to_array(Array<T>& array, const json::Json& json) {
    array.add_entry(T::from_json(json));
}


// append string from json
template<>
void append_to_array<std::string>(Array<std::string>& array, const json::Json& json) {
    array.add_entry(json);
}


// append double from json
template<>
void append_to_array<double>(Array<double>& array, const json::Json& json) {
    array.add_entry(json);
}


// append uint32_t from json
template<>
void append_to_array<std::uint32_t>(Array<std::uint32_t>& array, const json::Json& json) {
    array.add_entry(json);
}

}


template<typename T>
Array<T>::Array() { }


template<typename T>
Array<T>::~Array() { }


template<typename T>
Array<T> Array<T>::from_json(const json::Json& json) {
    Array<T> array;
    for (const auto& val : json) {
        ::append_to_array(array, val);
    }
    return array;
}


template<typename T>
json::Json Array<T>::to_json() const {
    json::Json json = json::Json::array();
    for (const auto& entry : *this) {
        ::append_to_json(json, entry);
    }
    return json;
}


namespace agent_framework {
namespace model {
namespace attribute {

/* Primitives */
template class Array<std::string>;
template class Array<std::uint32_t>;
template class Array<double>;

/* Model attributes */
template class Array<Collection>;
template class Array<NetworkService>;
template class Array<Ipv4Address>;
template class Array<Ipv6Address>;
template class Array<PciDevice>;
template class Array<UsbDevice>;
template class Array<PerformanceConfiguration>;
template class Array<SubcomponentEntry>;
template class Array<Message>;
template class Array<ManagerEntry>;
template class Array<Identifier>;
template class Array<Location>;
template class Array<Region>;
template class Array<MemorySet>;
template class Array<InterleaveSet>;
template class Array<ResultStatus>;
template class Array<TaskEntry>;
template class Array<ConnectedEntity>;
template class Array<IntegratedMemory>;
template class Array<Wildcard>;
template class Array<CalculationParameters>;
template class Array<MetricDefinitionEntry>;
template class Array<NextHop>;
template class Array<IpTransportDetail>;
template class Array<CapacitySource>;
template class Array<ReplicaInfo>;
template class Array<QosApplicationProtocol>;
template class Array<QosPriorityGroupMapping>;
template class Array<QosBandwidthAllocation>;
template class Array<FpgaReconfigurationSlot>;

/* Model classes */
template class Array<Metric>;

/* Enums */
template class Array<enums::GraphicalConsoleSupportedType>;
template class Array<enums::SerialConsoleSupportedType>;
template class Array<enums::CommandShellSupportedType>;
template class Array<enums::BootOverrideTarget>;
template class Array<enums::MemoryType>;
template class Array<enums::Media>;
template class Array<enums::MemoryModuleType>;
template class Array<enums::DeviceType>;
template class Array<enums::MemoryMode>;
template class Array<enums::SecurityStates>;
template class Array<enums::TransportProtocol>;
template class Array<enums::ResetType>;
template class Array<enums::VolumeType>;
template class Array<enums::EncryptionType>;
template class Array<enums::AccessCapability>;
template class Array<enums::FpgaInterface>;

}
}
}
