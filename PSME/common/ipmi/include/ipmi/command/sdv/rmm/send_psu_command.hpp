/*!
 * @copyright
 * Copyright (c) 2017 Intel Corporation
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
        OPERATION = 0x01,
        STATUS_WORD = 0x79,
        READ_POUT = 0x96,
        MFR_ID = 0x99,
        MFR_MODEL = 0x9A,
        MFR_REVISION = 0x9B,
        MFR_SERIAL = 0x9E,
        MFR_POUT_MAX = 0xA7
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

class SendPsuReadStatusWord : public SendPsuCommand {
public:
    /*!
     * @brief Constructor
     */
    SendPsuReadStatusWord();


    SendPsuReadStatusWord(const SendPsuReadStatusWord&) = default;


    SendPsuReadStatusWord(SendPsuReadStatusWord&&) = default;


    virtual ~SendPsuReadStatusWord();

    virtual const char* get_command_name() const override {
        return "SendPsuReadStatusWord";
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

protected:
    void pack_fields(IpmiInterface::ByteBuffer& data) const override;


private:
};

}

namespace response {

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
