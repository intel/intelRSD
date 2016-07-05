/*!
 * @copyright
 * INTEL CONFIDENTIAL
 *
 * @copyright
 * Copyright 2015-2016 Intel Corporation All Rights Reserved.
 *
 * @copyright
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its suppliers
 * or licensors. Title to the Material remains with Intel Corporation or its
 * suppliers and licensors. The Material may contain trade secrets and
 * proprietary and confidential information of Intel Corporation and its
 * suppliers and licensors, and is protected by worldwide copyright and trade
 * secret laws and treaty provisions. No part of the Material may be used,
 * copied, reproduced, modified, published, uploaded, posted, transmitted,
 * distributed, or disclosed in any way without Intel's prior express written
 * permission.
 *
 * @copyright
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by Intel in writing.
 *
 * @copyright
 * Unless otherwise agreed by Intel in writing, you may not remove or alter this
 * notice or any other notice embedded in Materials by Intel or Intel's
 * suppliers or licensors in any way.
 *
 * @file discovery_manager.hpp
 *
 * @brief discovery_manager
 * */

#ifndef AGENT_COMPUTE_DISCOVERY_DISCOVERY_MANAGER_HPP
#define AGENT_COMPUTE_DISCOVERY_DISCOVERY_MANAGER_HPP

#include "agent-framework/discovery/discovery_manager.hpp"
#include "agent-framework/logger_ext.hpp"

using agent_framework::generic::Module;

namespace agent {
namespace compute {
namespace discovery {

class DiscoveryManager: public agent_framework::generic::DiscoveryManager {
public:

    DiscoveryManager();
    DiscoveryManager(const DiscoveryManager& orig);
    virtual ~DiscoveryManager();

    virtual void discover(Module& module) const;
    virtual void discover();

};

}
}
}


#endif /* AGENT_COMPUTE_DISCOVERY_DISCOVERY_MANAGER_HPP */
