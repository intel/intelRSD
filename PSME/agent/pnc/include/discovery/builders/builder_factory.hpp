/*!
 * @copyright
 * Copyright (c) 2016-2019 Intel Corporation
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
 *
 * @file builder_factory.hpp
 *
 * @brief Builder factory interface
 * */

#pragma once



#include "discovery/builders/drive_builder.hpp"
#include "discovery/builders/endpoint_builder.hpp"
#include "discovery/builders/fabric_builder.hpp"
#include "discovery/builders/pcie_device_builder.hpp"
#include "discovery/builders/pcie_function_builder.hpp"
#include "discovery/builders/port_builder.hpp"
#include "discovery/builders/processor_builder.hpp"
#include "discovery/builders/storage_subsystem_builder.hpp"
#include "discovery/builders/switch_builder.hpp"
#include "discovery/builders/zone_builder.hpp"
#include "discovery/builders/chassis_builder.hpp"
#include "discovery/builders/metric_builder.hpp"
#include "discovery/builders/metric_definition_builder.hpp"
#include "agent-framework/module/common_components.hpp"
#include "agent-framework/module/pnc_components.hpp"

#include <memory>



namespace agent {
namespace pnc {
namespace discovery {
namespace builders {

/*!
 * @brief Builder factory class
 * */
class BuilderFactory {
public:

    /*! Default constructor */
    BuilderFactory() = default;


    /*! Enable copy */
    BuilderFactory(const BuilderFactory&) = default;


    BuilderFactory& operator=(const BuilderFactory&) = default;


    /*! Default destructor */
    virtual ~BuilderFactory();


    /*!
     * @brief Returns drive builder object
     * */
    virtual std::shared_ptr<DriveBuilder> get_drive_builder() {
        return std::shared_ptr<DriveBuilder>(new DriveBuilder());
    }


    /*!
     * @brief Returns endpoint builder object
     * */
    virtual std::shared_ptr<EndpointBuilder> get_endpoint_builder() {
        return std::shared_ptr<EndpointBuilder>(new EndpointBuilder());
    }


    /*!
     * @brief Returns fabric builder object
     * */
    virtual std::shared_ptr<FabricBuilder> get_fabric_builder() {
        return std::shared_ptr<FabricBuilder>(new FabricBuilder());
    }


    /*!
     * @brief Returns pcie device builder object
     * */
    virtual std::shared_ptr<PcieDeviceBuilder> get_pcie_device_builder() {
        return std::shared_ptr<PcieDeviceBuilder>(new PcieDeviceBuilder());
    }


    /*!
     * @brief Returns pcie function builder object
     * */
    virtual std::shared_ptr<PcieFunctionBuilder> get_pcie_function_builder() {
        return std::shared_ptr<PcieFunctionBuilder>(new PcieFunctionBuilder());
    }


    /*!
     * @brief Returns port builder object
     * */
    virtual std::shared_ptr<PortBuilder> get_port_builder() {
        return std::shared_ptr<PortBuilder>(new PortBuilder());
    }


    /*!
     * @brief Returns storage subsystem builder object
     * */
    virtual std::shared_ptr<StorageSubsystemBuilder> get_storage_subsystem_builder() {
        return std::shared_ptr<StorageSubsystemBuilder>(new StorageSubsystemBuilder());
    }


    /*!
     * @brief Returns switch builder object
     * */
    virtual std::shared_ptr<SwitchBuilder> get_switch_builder() {
        return std::shared_ptr<SwitchBuilder>(new SwitchBuilder());
    }


    /*!
     * @brief Returns zone builder object
     * */
    virtual std::shared_ptr<ZoneBuilder> get_zone_builder() {
        return std::shared_ptr<ZoneBuilder>(new ZoneBuilder());
    }


    /*!
    * @brief Returns chassis builder object
    * */
    virtual std::shared_ptr<ChassisBuilder> get_chassis_builder() {
        return std::shared_ptr<ChassisBuilder>(new ChassisBuilder());
    }


    /*!
     * @brief Returns metric builder object
     * */
    virtual std::shared_ptr<MetricBuilder> get_metric_builder() {
        return std::shared_ptr<MetricBuilder>(new MetricBuilder());
    }


    /*!
     * @brief Returns metric definition builder object
     * */
    virtual std::shared_ptr<MetricDefinitionBuilder> get_metric_definition_builder() {
        return std::shared_ptr<MetricDefinitionBuilder>(new MetricDefinitionBuilder());
    }


    /*!
     * @brief Returns processor builder object
     */
    virtual std::shared_ptr<ProcessorBuilder> get_processor_builder() {
        return std::shared_ptr<ProcessorBuilder>(new ProcessorBuilder());
    }


    /*!
     * @brief Helper template method that preinitializes builder with given parameters. Used for inline builder
     *        initialization.
     * @param[in] builder Builder to be initialized
     * @param[in] params Builder initalization params
     * @return Pointer to the initialized builder
     * */
    template<typename BUILDER_TYPE, typename PARAM>
    static std::shared_ptr<BUILDER_TYPE> init_builder(std::shared_ptr<BUILDER_TYPE> builder, PARAM& params) {
        builder->init(params);
        return builder;
    }
};

using BuilderFactoryPtr = std::shared_ptr<BuilderFactory>;

}
}
}
}
