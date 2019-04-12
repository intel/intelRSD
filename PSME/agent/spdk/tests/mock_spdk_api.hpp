/*!
 * @brief Mock spdk api
 *
 * @copyright Copyright (c) 2018-2019 Intel Corporation
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
 * @file mock_spdk_api.hpp
 */
#pragma once



#include "spdk/spdk_api.hpp"
#include "spdk/model/lvol_store.hpp"



namespace agent {
namespace spdk {
namespace testing {

class MockSpdkApi : public ::spdk::SpdkApi {

public:

    static constexpr const char LVS[] = "Logical Volume Store";
    static constexpr const char LV[] = "Logical volume";
    static constexpr const char NVME[] = "Nvme disk";
    static constexpr const char NQN_TGT[] = "target:nqn";
    static constexpr const char NQN_INIT[] = "initiator:nqn";


    MockSpdkApi() :
        SpdkApi(nullptr, nullptr) {};


    ~MockSpdkApi() {};


    std::vector<::spdk::model::LvolStore> get_lvol_stores() override {

        std::vector<::spdk::model::LvolStore> lvol_stores{};

        ::spdk::model::LvolStore lvol_store{};
        lvol_store.set_uuid(LVS);
        lvol_store.set_free_cluster(31);
        lvol_store.set_cluster_size(4194304);
        lvol_store.set_block_size(4096);

        lvol_stores.emplace_back(std::move(lvol_store));
        return lvol_stores;
    }


    std::vector<::spdk::model::Bdev> get_bdevs(const ::spdk::JsonOptional<std::string>& name = {}) override {
        std::vector<::spdk::model::Bdev> bdevs{};

        ::spdk::model::Bdev lvol_bdev{};
        lvol_bdev.set_name(LV);
        lvol_bdev.set_product_name(::spdk::model::BdevProductName::LOGICAL_VOLUME);
        lvol_bdev.set_block_size(512);
        lvol_bdev.set_num_blocks(20480);

        ::spdk::model::SupportedIoTypes supported_io_types{};
        supported_io_types.set_read(true);
        lvol_bdev.set_supported_io_types(supported_io_types);

        ::spdk::model::Bdev nvme_bdev{};
        nvme_bdev.set_name(NVME);
        nvme_bdev.set_product_name(::spdk::model::BdevProductName::NVMe_DISK);
        nvme_bdev.set_block_size(1024);
        nvme_bdev.set_num_blocks(102400);

        if (name) {
            if (nvme_bdev.get_name() == name) {
                bdevs.emplace_back(std::move(nvme_bdev));
            }
            else if (lvol_bdev.get_name() == name) {
                bdevs.emplace_back(std::move(lvol_bdev));
            }
        }
        else {
            bdevs.emplace_back(std::move(nvme_bdev));
            bdevs.emplace_back(std::move(lvol_bdev));
        }

        return bdevs;
    }


    std::vector<::spdk::model::NvmfSubsystem> get_nvmf_subsystems() override {

        std::vector<::spdk::model::NvmfSubsystem> nvmf_subsystems{};

        ::spdk::model::NvmfSubsystem nvmf_subsystem{};
        nvmf_subsystem.set_nqn(NQN_TGT);

        ::spdk::model::Host host{};
        host.set_nqn(NQN_INIT);

        ::spdk::model::Namespace nvmf_namespce{};
        nvmf_namespce.set_bdev_name(get_bdevs(LV).front().get_name());

        nvmf_subsystem.set_hosts({host});
        nvmf_subsystem.set_namespaces({nvmf_namespce});
        nvmf_subsystems.emplace_back(std::move(nvmf_subsystem));

        return nvmf_subsystems;
    }

};
}
}
}
