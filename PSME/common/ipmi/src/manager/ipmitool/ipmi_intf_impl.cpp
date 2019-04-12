/*!
 * @brief Transport implementation for ipmitool library.
 *
 * Class directly uses ipmitool library by the means of its structs.
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
 * @file ipmitool/ipmi_intf_impl.cpp
 */

#include "ipmi/manager/ipmitool/ipmi_intf_impl.hpp"

#include "generic/assertions.hpp"
#include "safe-string/safe_lib.hpp"
#include <stdexcept>


using namespace ipmi::manager::ipmitool;
using namespace generic;

namespace {

static constexpr std::uint8_t CFG_LOOUPBIT_DEFAULT = 0x10;
static constexpr std::uint8_t CFG_CIPHER_SUITE_ID_DEFAULT = 0x03;


/*!
 * @brief Class used to keep "previous" settings of bridging.
 *
 * Bridging info is kept in ipmi_intf, isn't connected with the message!
 */
class BridgeReflector {
public:
    BridgeReflector(ipmi_intf& _intf, const ipmi::BridgeInfo& bridge);


    ~BridgeReflector();


private:
    ipmi_intf& intf;
    const ipmi::BridgeInfo::Level level;

    uint32_t target_addr{};
    uint8_t target_channel{};
    uint32_t transit_addr{};
    uint8_t transit_channel{};
};


BridgeReflector::BridgeReflector(ipmi_intf& _intf, const ipmi::BridgeInfo& info) :
    intf(_intf), level(info.get_level()) {

    switch (level) {

        case ipmi::BridgeInfo::Level::DUAL_BRIDGE:
            transit_addr = intf.transit_addr;
            transit_channel = intf.transit_channel;
            info.get_transit(intf.transit_addr, intf.transit_channel);
	    /* fallthrough */
        case ipmi::BridgeInfo::Level::SINGLE_BRIDGE:
            target_addr = intf.target_addr;
            target_channel = intf.target_channel;
            info.get_target(intf.target_addr, intf.target_channel);
	    /* fallthrough */
        case ipmi::BridgeInfo::Level::DIRECT:
            /* nothing more */
            break;
        default:
            assert(FAIL("Unhandled bridging level"));
            break;
    }
}


BridgeReflector::~BridgeReflector() {
    switch (level) {
        case ipmi::BridgeInfo::Level::DUAL_BRIDGE:
            intf.transit_addr = transit_addr;
            intf.transit_channel = transit_channel;
	    /* fallthrough */
        case ipmi::BridgeInfo::Level::SINGLE_BRIDGE:
            intf.target_addr = target_addr;
            intf.target_channel = target_channel;
	    /* fallthrough */
        case ipmi::BridgeInfo::Level::DIRECT:
            /* nothing more */
            break;
        default:
            assert(FAIL("Unhandled bridging level"));
            break;
    }
}

}

namespace {

struct ipmi_intf zero_ipmi_intf() {
    struct ipmi_intf intf;
    memzero_s(&intf, sizeof(struct ipmi_intf));
    return intf;
}

struct ipmi_rq zero_ipmi_rq() {
    struct ipmi_rq rq;
    memzero_s(&rq, sizeof(struct ipmi_rq));
    return rq;
}

}

/*!
 * @brief Implementation of LAN IpmiTool interface
 *
 * It doesn't use locking, this is done in LanIpmiIntf.
 */
IpmiIntfImpl::IpmiIntfImpl(ipmi::ConnectionData::ConstPtr connection_data, IpmiIntfImpl::InitFn init) : m_ipmi_intf(zero_ipmi_intf()) {

    ipmi_intf* ptr = ipmi_intf_load(const_cast<char*>(connection_data->get_interface_type().c_str()));
    if (!ptr) {
        throw std::runtime_error("Can't create " + connection_data->get_interface_type() + " IPMI intf impl.");
    }
    m_ipmi_intf = *ptr;
    ipmi_cleanup(ptr);

    ipmi_intf_session_set_lookupbit(&m_ipmi_intf, CFG_LOOUPBIT_DEFAULT);
    ipmi_intf_session_set_sol_escape_char(&m_ipmi_intf, SOL_ESCAPE_CHARACTER_DEFAULT);
    ipmi_intf_session_set_cipher_suite_id(&m_ipmi_intf, CFG_CIPHER_SUITE_ID_DEFAULT);
    m_ipmi_intf.my_addr = IPMI_BMC_SLAVE_ADDR;

    m_ipmi_intf.devnum = 0;
    m_ipmi_intf.devfile = nullptr;

    /* initialize the interface according the settings */
    init(&m_ipmi_intf, connection_data);
}


IpmiIntfImpl::~IpmiIntfImpl() {
    close();
    ipmi_cleanup(&m_ipmi_intf);
}


void IpmiIntfImpl::open() {
    if (m_ipmi_intf.opened > 0) {
        throw std::runtime_error("IPMI connection already opened.");
    }
    if (m_ipmi_intf.open(&m_ipmi_intf) < 0) {
        throw std::runtime_error("Can't open IPMI connection.");
    }
}


void IpmiIntfImpl::close() {
    if ((m_ipmi_intf.opened > 0) && (nullptr != m_ipmi_intf.close)) {
        m_ipmi_intf.close(&m_ipmi_intf);
    }
}


void
IpmiIntfImpl::send(ipmi::IpmiInterface::NetFn netfn, ipmi::IpmiInterface::Cmd command, ipmi::IpmiInterface::Lun lun,
                   const ipmi::BridgeInfo& bridge,
                   const ipmi::IpmiInterface::ByteBuffer& request, ipmi::IpmiInterface::ByteBuffer& response,
                   bool reconnect) {

    std::unique_lock<IpmiIntfImpl> guard(*this);
    send_unlocked(netfn, command, lun, bridge, request, response, reconnect);

}

void
IpmiIntfImpl::send_unlocked(ipmi::IpmiInterface::NetFn netfn, ipmi::IpmiInterface::Cmd command, ipmi::IpmiInterface::Lun lun,
                  const ipmi::BridgeInfo& bridge,
                  const ipmi::IpmiInterface::ByteBuffer& request, ipmi::IpmiInterface::ByteBuffer& response,
                  bool reconnect) {

    /* connection will be renew if session is (possibly) expired */
    if ((m_ipmi_intf.opened > 0) && (reconnect)) {
        close();
    }
    if (!(m_ipmi_intf.opened > 0)) {
        open();
    }

    /* reflector used for restoring bridge info in ipmi_intf, restore on out of scope (in destructor) */
    BridgeReflector reflector(m_ipmi_intf, bridge);

    ipmi_rq req = zero_ipmi_rq();
    req.msg.netfn = 0x3F & netfn;
    req.msg.cmd = command;
    req.msg.lun = 0x03 & lun;
    req.msg.target_cmd = 0;
    req.msg.data_len = std::uint16_t(request.size());
    req.msg.data = const_cast<uint8_t*>(request.data());

    ipmi_rs* rsp = m_ipmi_intf.sendrecv(&m_ipmi_intf, &req);
    if (!rsp) {
        throw std::runtime_error("Received null response from IPMI.");
    }
    /* our response always starts with completion code, always is followed
     * by received bytes
     */
    response.clear();
    response.push_back(rsp->ccode);
    for (int i = 0; i < rsp->data_len; i++) {
        response.push_back(rsp->data[i]);
    }
}

void
IpmiIntfImpl::lock() {
    mutex.lock();
}

void
IpmiIntfImpl::unlock() {
    mutex.unlock();
}
