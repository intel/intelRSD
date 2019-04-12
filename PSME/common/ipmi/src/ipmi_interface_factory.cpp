/*!
* @brief Factory to create IPMI communication interfaces ("sessions").
 *
 * Sessions are built on interface type. All builders are to be added to the factory.
  *
 * @copyright Copyright (c) 2017-2019 Intel Corporation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file ipmi/ipmi_intf_factory.cpp
 */

#include "ipmi/ipmi_interface_factory.hpp"

#include "ipmi/manager/ipmitool/lan_ipmi_interface.hpp"
#include "ipmi/manager/ipmitool/serial_ipmi_interface.hpp"

namespace {

/*!
 * @brief "anonymous" factory with all possible builders used in the software
 */
class IpmitoolInterfaceFactory : public ipmi::IpmiInterfaceFactory {
public:
    IpmitoolInterfaceFactory() {
        add_builder<ipmi::manager::ipmitool::LanIpmiInterface>(ipmi::manager::ipmitool::LanConnectionData::INTF_TYPE);
        add_builder<ipmi::manager::ipmitool::SerialIpmiInterface>(ipmi::manager::ipmitool::SerialConnectionData::INTF_TYPE);
    }
};

}


using namespace ipmi;

IpmiInterfaceFactory::IpmiInterfaceFactory() { }

IpmiInterfaceFactory::~IpmiInterfaceFactory() { }

IpmiInterfaceFactory& IpmiInterfaceFactory::get_instance() {
    static ::IpmitoolInterfaceFactory factory{};
    return factory;
}


void IpmiInterfaceFactory::add_builder(const std::string& interface_type, IpmiInterfaceFactory::Builder builder) {
    builders[interface_type] = builder;
}


IpmiInterface::Ptr IpmiInterfaceFactory::get_interface(ConnectionData::ConstPtr connection_data) {

    std::unique_lock<std::mutex> lock(mutex);

    /* search in already created interfaces */
    for (IpmiInterface_WeakPtr wptr : interfaces) {
        IpmiInterface::Ptr ptr = wptr.lock();
        if (ptr && (ptr->matches(connection_data))) {
            return ptr;
        }
    }

    /* build new one */
    IpmiInterface::Ptr ptr;
    Builders::iterator bld = builders.find(connection_data->get_interface_type());
    if (bld != builders.end()) {
        ptr = (bld->second)(connection_data);
    }
    if (ptr) {
        interfaces.push_back(ptr);
    }
    return ptr;
}


void IpmiInterfaceFactory::release_interface(IpmiInterface::Ptr&& intf) {

    std::unique_lock<std::mutex> lock(mutex);

    intf.reset();

    /* remove all "empty" pointers */
    Collection::iterator it = interfaces.begin();
    while (it != interfaces.end()) {
        if (it->expired()) {
            it = interfaces.erase(it);
        } else {
            it++;
        }
    }
}
