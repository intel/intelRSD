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
 * @file configuration_space_register.hpp
 * @brief Configuration Space Register
 * */

#pragma once

#include "gas/access_interface.hpp"
#include "gas/utils.hpp"

#include <memory>

/*! Agent namespace */
namespace agent {
/*! PNC namespace */
namespace pnc {
/*! GAS namespace */
namespace gas {

class GlobalAddressSpaceRegisters;

/*! CSR namespace */
namespace csr {

/*! Configuration Space Register class */
class ConfigurationSpaceRegister final {
private:
    friend class agent::pnc::gas::GlobalAddressSpaceRegisters;

    /* Reads TopLevelRegisters memory region. */
    void read(AccessInterface* iface);

    /*! Size of the PCI Configuration space */
    static constexpr std::uint32_t PCI_HEADER_SIZE = 64;

    /*! Size of the PCI Capabilities Region */
    static constexpr std::uint32_t PCI_CAPABILITIES_SIZE = 192;

    /*! Mask marking MaxSpeed field bits */
    static constexpr std::uint32_t MAX_SPEED_MASK = 0x0000000F;

    /*! Mask marking MaxWidth field bits */
    static constexpr std::uint32_t MAX_WIDTH_MASK = 0x000003F0;

    /*! Bits to be shifted to get MaxWidth value */
    static constexpr std::uint8_t MAX_WIDTH_SHIFT = 4;

    /*! Mask marking Speed field bits */
    static constexpr std::uint16_t SPEED_MASK = 0x000F;

    /*! Mask marking Width field bits */
    static constexpr std::uint16_t WIDTH_MASK = 0x03F0;

    /*! Bits to be shifted to get Width value */
    static constexpr std::uint8_t WIDTH_SHIFT = 4;

    /*!
     * @brief Conversion of the speed bits to the speed they represent
     * @param[in] speed_bit Speed bit value
     * @return Speed value represented by the speed bit
     * */
    static double speed_bit_to_double(const uint32_t speed_bit);

public:

    /*! Number of used CSR bytes */
    static constexpr std::uint32_t CSR_REG_SIZE_USED = PCI_CAPABILITIES_SIZE;

    /*! Default constructor */
    ConfigurationSpaceRegister() {}

    /*! Copy constructor */
    ConfigurationSpaceRegister(const ConfigurationSpaceRegister&) = default;

    /*! Assignment operator */
    ConfigurationSpaceRegister& operator=(const ConfigurationSpaceRegister&) = default;

    /*!
     * @brief Extracts Max speed from the read data
     * @return Max speed
     * */
    inline double get_max_speed() const {
        return speed_bit_to_double(this->output.pci_caps.fields.link_capability & MAX_SPEED_MASK);
    }

    /*!
     * @brief Extracts speed from the read data
     * @return Speed
     * */
    inline double get_speed() const {
        return speed_bit_to_double(this->output.pci_caps.fields.link_status & SPEED_MASK);
    }

    /*!
     * @brief Extracts Max width from the read data
     * @return Max width
     * */
    inline std::uint32_t get_max_width() const {
        return (this->output.pci_caps.fields.link_capability & MAX_WIDTH_MASK) >> MAX_WIDTH_SHIFT;
    }

    /*!
     * @brief Extracts width from the read data
     * @return width
     * */
    inline std::uint32_t get_width() const {
        return (this->output.pci_caps.fields.link_status & WIDTH_MASK) >> WIDTH_SHIFT;
    }

#pragma pack(push, 1)
    /*! Partition Configuration input structure */
    union {
        struct {
            std::uint8_t instance_id;
        } fields;
    } input{};

    /*! Configuration Space Register structure  */
    struct {
        /*! PCI Capability Region */
        union {
            struct {
                std::uint8_t  capability_id{};
                std::uint8_t  next_capability_pointer{};
                std::uint16_t pci_express_capability{};
                std::uint32_t device_capability{};
                std::uint16_t device_control{};
                std::uint16_t device_status{};
                std::uint32_t link_capability{};
                std::uint16_t link_control{};
                std::uint16_t link_status{};
                std::uint32_t slot_capability{};
                std::uint16_t slot_control{};
                std::uint16_t slot_status{};
                std::uint32_t not_supported_fields[2]{};
                std::uint32_t device_capability_2{};
                std::uint16_t device_control_2{};
                std::uint16_t device_status_2{};
                std::uint32_t link_capability_2{};
                std::uint16_t link_control_2{};
                std::uint16_t link_status_2{};
            } fields{};
            std::uint8_t raw[CSR_REG_SIZE_USED];
        } pci_caps{};
    } output{};
#pragma pack(pop)

    ~ConfigurationSpaceRegister();
};

}
}
}
}
