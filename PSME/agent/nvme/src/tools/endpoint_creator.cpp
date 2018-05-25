/*!
 * @brief Implementation of endpoint creator class.
 *
 * @header{License}
 * @copyright Copyright (c) 2017-2018 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @header{Files}
 * @file endpoint_creator.cpp
 */

#include "tools/endpoint_creator.hpp"
#include "tools/endpoint_reader.hpp"
#include "tools/dir.hpp"
#include "tools/tools.hpp"
#include "logger/logger_factory.hpp"

#include <stdexcept>
#include <fstream>

#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

using namespace agent::nvme::tools;
using namespace std;

namespace {


const std::string VALUE_RDMA = "rdma";
const std::string VALUE_IPV4 = "ipv4";
const std::string NAMESPACE_NAME = "1";

BaseTransactionHandler::func_pair get_subsystem_handlers(nvme_target::Id& subsystem, const nvme_target::Id& nqn,
    std::shared_ptr<nvme_target::BaseNvmeTargetHandler> configurator) {

    auto to_commit = [&subsystem, &nqn, configurator]() {
        configurator->add_subsystem(nqn);
        subsystem = nqn;
        configurator->set_subsystem_allow_all_hosts(subsystem, false);
    };

    auto rollback = [&subsystem, configurator]() {
        configurator->remove_subsystem(subsystem);
    };

    return {to_commit, rollback};
}

BaseTransactionHandler::func_pair get_namespace_handlers(nvme_target::Id& ns, const nvme_target::Id& subsystem,
    const string& device_path, std::shared_ptr<nvme_target::BaseNvmeTargetHandler> configurator) {

    auto to_commit = [&ns, &subsystem, &device_path, configurator]() {
        // for now we only support single namespace subsystems
        ns = NAMESPACE_NAME;
        configurator->add_subsystem_namespace(subsystem, ns);
        configurator->set_subsystem_namespace_device(subsystem, ns, device_path);
        configurator->set_subsystem_namespace_enable(subsystem, ns, false);
    };

    auto rollback = [&ns, &subsystem, configurator]() {
        configurator->remove_subsystem_namespace(subsystem, ns);
    };

    return {to_commit, rollback};
}

BaseTransactionHandler::func_pair get_port_handlers(nvme_target::Id& nvme_target_port, const string& ip_address,
    uint16_t port, std::shared_ptr<nvme_target::BaseNvmeTargetHandler> configurator) {

    auto to_commit = [&nvme_target_port, &ip_address, port, configurator]() {
        // try to find an existing port with the same IP and port address
        for (const auto& existing_port : configurator->get_ports()) {
            std::string port_ip;
            uint16_t port_num;
            std::tie(port_ip, port_num, std::ignore, std::ignore) = configurator->get_port_params(existing_port);
            if ((ip_address == port_ip) && (port_num == port)) {
                nvme_target_port = existing_port;
                // port found, nothing left to do
                return;
            }
        }

        // a matching port has not been found, try to create one
        bool failed {true};
        for (int port_num = 1; port_num <= UINT16_MAX && failed; port_num++) {
            failed = false;
            try {
                configurator->add_port(std::to_string(port_num));
            }
            catch (const runtime_error& error) {
                failed = true;
                // TODO this should be handled in the lower layer
                if (EEXIST == errno) {
                    // port already used, try next one
                    continue;
                }
                // other error
                throw error;
            }

            if (!failed) {
                nvme_target_port = std::to_string(port_num);
                log_debug("nvme-agent", "Setting port " << port_num << " ip: " << ip_address << " port: " << port);
                configurator->set_port_params(nvme_target_port, std::tie(ip_address, port, VALUE_IPV4, VALUE_RDMA));
            }
        }

        if (failed) {
            throw std::runtime_error("Failed to find a free NVMEt port");
        }
    };

    auto rollback = [&nvme_target_port, configurator]() {
        // remove port only when there are no subsystems present
        if (configurator->get_port_subsystems(nvme_target_port).empty()) {
            configurator->remove_port(nvme_target_port);
        }
    };

    return {to_commit, rollback};
}

BaseTransactionHandler::func_pair get_link_handlers(const nvme_target::Id& ss, const nvme_target::Id& port,
    std::shared_ptr<nvme_target::BaseNvmeTargetHandler> configurator) {

    auto to_commit = [&ss, &port, configurator]() {
        configurator->add_port_subsystem(port, ss);
    };

    auto rollback = [&ss, &port, configurator]() {
        configurator->remove_port_subsystem(port, ss);
    };

    return {to_commit, rollback};
}

}

uint16_t EndpointCreator::create_target_endpoint(std::shared_ptr<NvmeAgentContext> context, const string& ip_address,
    uint16_t port, const string& nqn, const string& device_path) const {
    log_debug("nvme-agent", "Creating endpoint nqn " << nqn << " ip: " << ip_address
        << " port: " << port << " device_path " << device_path);

    auto target_configurator = context->nvme_target_handler;
    auto transaction_handler = context->transaction_handler_factory->get_handler();
    nvme_target::Id ss{};
    nvme_target::Id ns{};
    nvme_target::Id nvme_target_port{};

    // the order of operations is important
    transaction_handler->add_handler_pair(get_subsystem_handlers(ss, nqn, target_configurator));
    transaction_handler->add_handler_pair(get_namespace_handlers(ns, ss, device_path, target_configurator));
    transaction_handler->add_handler_pair(
        get_port_handlers(nvme_target_port, ip_address, port, target_configurator));
    transaction_handler->add_handler_pair(get_link_handlers(ss, nvme_target_port, target_configurator));

    try {
        transaction_handler->commit();
    }
    catch (const std::exception& e) {
        transaction_handler->rollback();
        throw;
    }

    // convert port to number
    return static_cast<uint16_t>(std::stoi(nvme_target_port));
}

void EndpointCreator::create_initiator_endpoint(std::shared_ptr<NvmeAgentContext>, const string& /*nqn*/) const {
}

void EndpointCreator::delete_target_endpoint(std::shared_ptr<NvmeAgentContext> context, const string& nqn,
    const std::string& nvme_port) const {
    string subsystem = nqn;
    tools::convert_to_subnqn(subsystem);
    auto nvme_target_port = nvme_port;
    auto target_configurator = context->nvme_target_handler;
    auto transaction_handler = context->transaction_handler_factory->get_handler();
    auto nop = []() {};

    // unlink subsystem from port
    auto funcs = get_link_handlers(subsystem, nvme_target_port, target_configurator);
    // add unlink as commit action and nop as rollback
    transaction_handler->add_handler_pair({funcs.second, nop});

    // delete port
    funcs = get_port_handlers(nvme_target_port, "", 0, target_configurator);
    transaction_handler->add_handler_pair({funcs.second, nop});

    // delete all namespaces
    auto namespaces = target_configurator->get_subsystem_namespaces(subsystem);
    for (auto& ns : namespaces) {
        funcs = get_namespace_handlers(ns, subsystem, "", target_configurator);
        transaction_handler->add_handler_pair({funcs.second, nop});
    }

    // delete subsystem
    funcs = get_subsystem_handlers(subsystem, "", target_configurator);
    transaction_handler->add_handler_pair({funcs.second, nop});

    // no need to perform rollback
    transaction_handler->commit();
}
