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
 *
 *
 * @file send_psu_command.hpp
 * @brief SendPsuCommand IPMI command request and response.
 * */

#pragma once



#include "ipmi/request.hpp"
#include "ipmi/response.hpp"
#include "ipmi/command/sdv/enums.hpp"



namespace ipmi {
namespace command {
namespace sdv {

namespace request {

/*!
 * @brief Base class for sending commands to PSU.
 */
class SendPsuCommand : public Request {
public:
    enum class OperationCode : std::uint8_t {
        READ_BYTE,
        READ_WORD,
        READ_BLOCK,
        WRITE_BYTE,
        WRITE_WORD,
        WRITE_BLOCK
    };

    enum class Command : std::uint8_t {
        PAGE = 0x00,
        OPERATION = 0x01,
        CLEAR_FAULTS = 0x03,
        STATUS_WORD = 0x79,
        READ_POUT = 0x96,
        MFR_ID = 0x99,
        MFR_MODEL = 0x9A,
        MFR_REVISION = 0x9B,
        MFR_SERIAL = 0x9E,
        MFR_POUT_MAX = 0xA7,
        MFR_PSU_STA = 0xE7
    };

    /*!
     * @brief PSU id setter
     * @param psu_id PSU id in range [0, 5].
     */
    void set_psu_id(std::uint8_t psu_id);

protected:
    /*!
     * @brief Constructor.
     * @param op_code operation code
     * @param cmd PSU command
     */
    SendPsuCommand(OperationCode op_code, Command cmd);


    /*!
     * @brief Places send PSU specific command fields in ByteBuffer.
     * @param data ByteBuffer into which command specific fields are placed.
     */
    virtual void pack_fields(IpmiInterface::ByteBuffer& data) const;

private:
    std::uint8_t m_psu_id{0};
    OperationCode m_operation_code{};
    Command m_command{};


    void pack(IpmiInterface::ByteBuffer& data) const override;
};


/*!
 * @brief Command to clear STATUS_WORD registry
 * @warning Doesn't work with DPST-2400CB A
 */
class SendPsuClearFaults : public SendPsuCommand {
public:
    /*!
     * @brief Constructor
     */
    SendPsuClearFaults();
    SendPsuClearFaults(const SendPsuClearFaults&) = default;
    SendPsuClearFaults(SendPsuClearFaults&&) = default;

    virtual ~SendPsuClearFaults();

    virtual const char* get_command_name() const override {
        return "SendPsuClearFaults";
    }

protected:
    void pack_fields(IpmiInterface::ByteBuffer& data) const override;

private:
    static constexpr const std::uint8_t REQUEST_DATA_SIZE = 1;
};


class SendPsuReadStatusWord : public SendPsuCommand {
public:
    SendPsuReadStatusWord();
    SendPsuReadStatusWord(const SendPsuReadStatusWord&) = default;
    SendPsuReadStatusWord(SendPsuReadStatusWord&&) = default;

    virtual ~SendPsuReadStatusWord();

    virtual const char* get_command_name() const override {
        return "SendPsuReadStatusWord";
    }
};


/*!
 * @brief Read data from MFR PSU STA registry
 * @warning This is extended interface available with DPST-2400CB A
 */
class SendPsuReadMfrPsuSta : public SendPsuCommand {
public:
    SendPsuReadMfrPsuSta();
    SendPsuReadMfrPsuSta(const SendPsuReadMfrPsuSta&) = default;
    SendPsuReadMfrPsuSta(SendPsuReadMfrPsuSta&&) = default;

    virtual ~SendPsuReadMfrPsuSta();

    virtual const char* get_command_name() const override {
        return "SendPsuReadMfrPsuSta";
    }
};


class SendPsuReadPowerOutput : public SendPsuCommand {
public:
    /*!
     * @brief Constructor
     */
    SendPsuReadPowerOutput();


    SendPsuReadPowerOutput(const SendPsuReadPowerOutput&) = default;


    SendPsuReadPowerOutput(SendPsuReadPowerOutput&&) = default;


    virtual ~SendPsuReadPowerOutput();

    virtual const char* get_command_name() const override {
        return "SendPsuReadPowerOutput";
    }
};

class SendPsuReadPowerCapacity : public SendPsuCommand {
public:
    /*!
     * @brief Constructor
     */
    SendPsuReadPowerCapacity();


    SendPsuReadPowerCapacity(const SendPsuReadPowerCapacity&) = default;


    SendPsuReadPowerCapacity(SendPsuReadPowerCapacity&&) = default;


    virtual ~SendPsuReadPowerCapacity();

    virtual const char* get_command_name() const override {
        return "SendPsuReadPowerCapacity";
    }
};

class SendPsuReadSerial : public SendPsuCommand {
public:
    /*!
     * @brief Constructor
     */
    SendPsuReadSerial();


    SendPsuReadSerial(const SendPsuReadSerial&) = default;


    SendPsuReadSerial(SendPsuReadSerial&&) = default;


    virtual ~SendPsuReadSerial();

    virtual const char* get_command_name() const override {
        return "SendPsuReadSerial";
    }
};

class SendPsuReadModel : public SendPsuCommand {
public:
    /*!
     * @brief Constructor
     */
    SendPsuReadModel();


    SendPsuReadModel(const SendPsuReadModel&) = default;


    SendPsuReadModel(SendPsuReadModel&&) = default;


    virtual ~SendPsuReadModel();

    virtual const char* get_command_name() const override {
        return "SendPsuReadModel";
    }
};

class SendPsuReadManufacturer : public SendPsuCommand {
public:
    /*!
     * @brief Constructor
     */
    SendPsuReadManufacturer();


    SendPsuReadManufacturer(const SendPsuReadManufacturer&) = default;


    SendPsuReadManufacturer(SendPsuReadManufacturer&&) = default;


    virtual ~SendPsuReadManufacturer();

    virtual const char* get_command_name() const override {
        return "SendPsuReadManufacturer";
    }
};

class SendPsuReadRevision : public SendPsuCommand {
public:
    /*!
     * @brief Constructor
     */
    SendPsuReadRevision();


    SendPsuReadRevision(const SendPsuReadRevision&) = default;


    SendPsuReadRevision(SendPsuReadRevision&&) = default;


    virtual ~SendPsuReadRevision();

    virtual const char* get_command_name() const override {
        return "SendPsuReadRevision";
    }
};

class SendPsuWriteOperation : public SendPsuCommand {
public:
    enum class State : std::uint8_t {
        IMMEDIATE_OFF = 0x00,
        ON = 0x80,
    };

    /*!
     * @brief Constructor
     */
    SendPsuWriteOperation();


    SendPsuWriteOperation(const SendPsuWriteOperation&) = default;


    SendPsuWriteOperation(SendPsuWriteOperation&&) = default;


    virtual ~SendPsuWriteOperation();

    /*!
     * @brief State of PSU setter.
     * @param state Requested state to change.
     * */
    void set_state(State state);

    virtual const char* get_command_name() const override {
        return "SendPsuWriteOperation";
    }

protected:
    void pack_fields(IpmiInterface::ByteBuffer& data) const override;


private:
    static constexpr const std::uint8_t REQUEST_DATA_SIZE = 1;

    State m_state{State::ON};
};

class SendPsuReadOperation : public SendPsuCommand {
public:

    /*!
     * @brief Constructor
     */
    SendPsuReadOperation();


    SendPsuReadOperation(const SendPsuReadOperation&) = default;


    SendPsuReadOperation(SendPsuReadOperation&&) = default;


    virtual ~SendPsuReadOperation();

    virtual const char* get_command_name() const override {
        return "SendPsuWriteOperation";
    }
};

}


namespace response {

class SendPsuClearFaults : public Response {
public:
    /*!
     * @brief Constructor
     */
    SendPsuClearFaults();
    SendPsuClearFaults(const SendPsuClearFaults&) = default;
    SendPsuClearFaults(SendPsuClearFaults&&) = default;

    virtual ~SendPsuClearFaults();

    virtual const char* get_command_name() const override {
        return "SendPsuClearFaults";
    }

private:
    static constexpr const std::size_t RESP_SIZE = 1;

    void unpack(const IpmiInterface::ByteBuffer& data) override;
};


class SendPsuReadStatusWord : public Response {
public:
    /*!
     * @brief Constructor
     */
    SendPsuReadStatusWord();


    SendPsuReadStatusWord(const SendPsuReadStatusWord&) = default;


    SendPsuReadStatusWord(SendPsuReadStatusWord&&) = default;


    virtual ~SendPsuReadStatusWord();

    /*
     * @brief Checks if any warning flags are present
     * @return True if any warning flag is set
     */
    bool warnings_present() const {
        return m_warnings_present;
    }

    /*
     * @brief Status Word getter
     * @return Status Word.
     */
    std::uint16_t get_status_word() const;

    virtual const char* get_command_name() const override {
        return "SendPsuReadStatusWord";
    }

private:
    static constexpr const std::size_t RESP_SIZE = 3;
    static constexpr const std::uint8_t LOW_WARNING_MASK = 0xBF;
    static constexpr const std::uint8_t HIGH_WARNING_MASK = 0xFF;
    std::uint16_t m_status_word{0};
    bool m_warnings_present{false};


    void unpack(const IpmiInterface::ByteBuffer& data) override;
};

class SendPsuReadMfrPsuSta : public Response {
public:
    SendPsuReadMfrPsuSta();
    SendPsuReadMfrPsuSta(const SendPsuReadMfrPsuSta&) = default;
    SendPsuReadMfrPsuSta(SendPsuReadMfrPsuSta&&) = default;

    virtual ~SendPsuReadMfrPsuSta();

    virtual const char* get_command_name() const override {
        return "SendPsuReadMfrPsuSta";
    }

    /*!
     * @brief Gets raw ALM_STA value
     * @return Raw ALM_STA value
     */
    std::uint32_t get_alm_sta() const {
        return m_alm_sta;
    }

    /*!
     * @brief Checks if AC has failed
     * @return true if AC has failed
     */
    bool ac_has_failed() const;

    /*!
     * @brief Checks if any error flags are present
     * @return True if any error flag is set
     */
    bool errors_present() const;

    /*
     * @brief Checks if any warning flags are present
     * @return True if any warning flag is set
     */
    bool warnings_present() const;

private:
    static constexpr const std::size_t RESP_SIZE = 6;

    // ALM_STA0
    static constexpr const std::uint32_t PSON_FAULT        = 0x80;
    static constexpr const std::uint32_t SHORT_PIN_FAULT   = 0x40;
    static constexpr const std::uint32_t NTC_OT_FAULT      = 0x20;
    static constexpr const std::uint32_t IOUT_OC_FAULT     = 0x10;
    static constexpr const std::uint32_t STDBY_OV_FAULT    = 0x08;
    static constexpr const std::uint32_t STDBY_UV_FAULT    = 0x04;
    static constexpr const std::uint32_t HW_OV_FAULT       = 0x02;
    static constexpr const std::uint32_t VOUT_UV_FAULT     = 0x01;

    // ALM_STA1
    // Bits 7..5 Reserved
    static constexpr const std::uint32_t SR2_OT_FAULT    = 0x1000;
    static constexpr const std::uint32_t SR1_OT_FAULT    = 0x0800;
    static constexpr const std::uint32_t VOUT_OV_FAULT   = 0x0400;
    static constexpr const std::uint32_t FAN_FAULT       = 0x0200;
    static constexpr const std::uint32_t FAN_WARN        = 0x0100;

    // ALM_STA2
    static constexpr const std::uint32_t DD_OT_FAULT   = 0x800000;
    static constexpr const std::uint32_t PFC_OT_FAULT  = 0x400000;
    static constexpr const std::uint32_t OT_FAULT      = 0x200000;
    static constexpr const std::uint32_t OT_WARN       = 0x100000;
    static constexpr const std::uint32_t VIN1_OV_FAULT = 0x080000;
    static constexpr const std::uint32_t VIN1_UV_FAULT = 0x040000;
    static constexpr const std::uint32_t VIN0_OV_FAULT = 0x020000;
    static constexpr const std::uint32_t VIN0_UV_FAULT = 0x010000;

    // ALM_STA3
    // All bits reserved

    std::uint32_t m_alm_sta{};
    std::uint8_t m_ac_status{};

    void unpack(const IpmiInterface::ByteBuffer& data) override;
};

class SendPsuReadPowerOutput : public Response {
public:
    /*!
     * @brief Constructor
     */
    SendPsuReadPowerOutput();


    SendPsuReadPowerOutput(const SendPsuReadPowerOutput&) = default;


    SendPsuReadPowerOutput(SendPsuReadPowerOutput&&) = default;


    virtual ~SendPsuReadPowerOutput();


    /*
     * @brief Output Power Value getter
     * @return Output Power Value converted from linear data format.
     */
    double get_power_output_value() const;

    virtual const char* get_command_name() const override {
        return "SendPsuReadPowerOutput";
    }


private:
    static constexpr const std::size_t RESP_SIZE = 3;
    std::uint16_t m_output_power{0};


    void unpack(const IpmiInterface::ByteBuffer& data) override;
};

class SendPsuReadPowerCapacity : public Response {
public:
    /*!
     * @brief Constructor
     */
    SendPsuReadPowerCapacity();


    SendPsuReadPowerCapacity(const SendPsuReadPowerCapacity&) = default;


    SendPsuReadPowerCapacity(SendPsuReadPowerCapacity&&) = default;


    virtual ~SendPsuReadPowerCapacity();


    /*
     * @brief Capacity Power Value getter
     * @return Capacity Power Value converted from linear data format.
     */
    double get_power_capacity_value() const;

    virtual const char* get_command_name() const override {
        return "SendPsuReadPowerCapacity";
    }

private:
    static constexpr const std::size_t RESP_SIZE = 3;
    std::uint16_t m_power_capacity{0};


    void unpack(const IpmiInterface::ByteBuffer& data) override;
};

class SendPsuReadSerial : public Response {
public:
    /*!
     * @brief Constructor
     */
    SendPsuReadSerial();


    SendPsuReadSerial(const SendPsuReadSerial&) = default;


    SendPsuReadSerial(SendPsuReadSerial&&) = default;


    virtual ~SendPsuReadSerial();


    /*
     * @brief Serial number getter
     * @return Serial number
     */
    std::string get_serial() const;

    virtual const char* get_command_name() const override {
        return "SendPsuReadSerial";
    }

private:
    static constexpr const std::size_t MIN_RESP_SIZE = 1;
    std::string m_serial{};


    void unpack(const IpmiInterface::ByteBuffer& data) override;
};

class SendPsuReadModel : public Response {
public:
    /*!
     * @brief Constructor
     */
    SendPsuReadModel();


    SendPsuReadModel(const SendPsuReadModel&) = default;


    SendPsuReadModel(SendPsuReadModel&&) = default;


    virtual ~SendPsuReadModel();


    /*
     * @brief Model getter
     * @return Model
     */
    std::string get_model() const;

    virtual const char* get_command_name() const override {
        return "SendPsuReadModel";
    }

private:
    static constexpr const std::size_t MIN_RESP_SIZE = 1;
    std::string m_model{};


    void unpack(const IpmiInterface::ByteBuffer& data) override;
};

class SendPsuReadManufacturer : public Response {
public:
    /*!
     * @brief Constructor
     */
    SendPsuReadManufacturer();


    SendPsuReadManufacturer(const SendPsuReadManufacturer&) = default;


    SendPsuReadManufacturer(SendPsuReadManufacturer&&) = default;


    virtual ~SendPsuReadManufacturer();


    /*
     * @brief Manufacturer getter
     * @return Manufacturer
     */
    std::string get_manufacturer() const;

    virtual const char* get_command_name() const override {
        return "SendPsuReadManufacturer";
    }

private:
    static constexpr const std::size_t MIN_RESP_SIZE = 1;
    std::string m_manufacturer{};


    void unpack(const IpmiInterface::ByteBuffer& data) override;
};

class SendPsuReadRevision : public Response {
public:
    /*!
     * @brief Constructor
     */
    SendPsuReadRevision();


    SendPsuReadRevision(const SendPsuReadRevision&) = default;


    SendPsuReadRevision(SendPsuReadRevision&&) = default;


    virtual ~SendPsuReadRevision();


    /*
     * @brief Revision getter
     * @return Revision
     */
    std::string get_revision() const;

    virtual const char* get_command_name() const override {
        return "SendPsuReadRevision";
    }

private:
    static constexpr const std::size_t MIN_RESP_SIZE = 1;
    std::string m_revision{};


    void unpack(const IpmiInterface::ByteBuffer& data) override;
};

class SendPsuWriteOperation : public Response {
public:
    /*!
     * @brief Constructor
     */
    SendPsuWriteOperation();


    SendPsuWriteOperation(const SendPsuWriteOperation&) = default;


    SendPsuWriteOperation(SendPsuWriteOperation&&) = default;


    virtual ~SendPsuWriteOperation();

    virtual const char* get_command_name() const override {
        return "SendPsuWriteOperation";
    }

private:
    static constexpr const std::size_t RESPONSE_SIZE = 1;

    void unpack(const IpmiInterface::ByteBuffer& data) override;
};

class SendPsuReadOperation : public Response {
public:
    /*!
     * @brief Constructor
     */
    SendPsuReadOperation();


    SendPsuReadOperation(const SendPsuReadOperation&) = default;


    SendPsuReadOperation(SendPsuReadOperation&&) = default;


    virtual ~SendPsuReadOperation();

    /*
     * @brief Gets state from the command
     * @brief True if psu is enabled
     */
    bool is_enabled() const {
        return m_is_enabled;
    }

    /*
     * @brief Gets the operation byte value
     * @brief Operation byte value
     */
    uint8_t get_operation_byte() const {
        return m_operation;
    }

    virtual const char* get_command_name() const override {
        return "SendPsuReadOperation";
    }

private:
    static constexpr const std::size_t RESPONSE_SIZE = 2;
    static constexpr const std::uint8_t ENABLED_STATE = 0x80;

    void unpack(const IpmiInterface::ByteBuffer& data) override;

    bool m_is_enabled{false};
    std::uint8_t m_operation{};
};

}

}
}
}
