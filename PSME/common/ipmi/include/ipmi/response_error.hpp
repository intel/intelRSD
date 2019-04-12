/*!
 * @brief Response Errors defined by IPMI specification.
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
 * @file ipmi/response_error.hpp
 */

#pragma once



#include "message.hpp"

#include <exception>
#include <stdexcept>
#include <string>



namespace ipmi {

/* forward declaration */
class Response;

using CompletionCode = std::uint8_t;

static constexpr const CompletionCode COMPLETION_CODE_NORMAL = 0;

/*!
 * @brief Base Exception type that will be thrown when error condition is
 * discovered while validating Response.
 */
class ResponseError : public std::runtime_error {
public:
    /*!
     * @brief Constructs exception object
     * @param response IPMI response object
     * @param what_arg Describes what happened
     * @param completion_code completion code
     */
    ResponseError(const Response& response,
                  const std::string& what_arg,
                  CompletionCode completion_code);


    ResponseError(const ResponseError&) = default;


    ResponseError(ResponseError&&) = default;


    virtual ~ResponseError();


    /*!
     * @brief Completion code getter
     * @return Completion code.
     */
    CompletionCode get_completion_code() const {
        return m_completion_code;
    }


    /*!
     * @brief Network function number getter.
     * @return Network function number.
     */
    IpmiInterface::NetFn get_network_function() const {
        return m_network_function;
    }


    /*!
     * @brief Command number getter.
     * @return Command number.
     */
    IpmiInterface::Cmd get_command() const {
        return m_command;
    }


private:
    CompletionCode m_completion_code;
    IpmiInterface::NetFn m_network_function;
    IpmiInterface::Cmd m_command;
};

/*!
 * @brief Exception type for invalid Response length.
 */
class ResponseLengthError : public ResponseError {
public:
    ResponseLengthError(const Response& response, const std::size_t expected, const std::size_t received)
        : ResponseError(response, "Wrong length of the response,"
                                      " expected " + std::to_string(expected) +
                                  " received " + std::to_string(received),
                        0) {}


    ResponseLengthError(const ResponseLengthError&) = default;


    ResponseLengthError(ResponseLengthError&&) = default;


    virtual ~ResponseLengthError();
};

// GENERIC CODES 0xC0 - 0xFF

/*!
 * @brief Exception type for node busy.
 *
 * Command can not be processed because command processing resources are
 * temporarily unavailable.
 */
class NodeBusyError : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0xC0;
    static constexpr const char ERROR_STRING[] = "Node Busy";


    explicit NodeBusyError(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    NodeBusyError(const NodeBusyError&) = default;


    NodeBusyError(NodeBusyError&&) = default;


    virtual ~NodeBusyError();
};

/*!
 * @brief Exception type for Invalid Command.
 *
 * Used to indicate an unrecognized or unsupported command.
 */
class InvalidCommandError : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0xC1;
    static constexpr const char ERROR_STRING[] = "Invalid Command";


    explicit InvalidCommandError(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    InvalidCommandError(const InvalidCommandError&) = default;


    InvalidCommandError(InvalidCommandError&&) = default;


    virtual ~InvalidCommandError();
};

/*!
 * @brief Exception type for timeout while processing Request.
 */
class TimeOutError : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0xC3;
    static constexpr const char ERROR_STRING[] = "Timeout while processing command";


    explicit TimeOutError(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    TimeOutError(const TimeOutError&) = default;


    TimeOutError(TimeOutError&&) = default;


    virtual ~TimeOutError();
};

/*!
 * @brief Exception type for Out of space condition.
 */
class OutOfSpaceError : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0xC4;
    static constexpr const char ERROR_STRING[] = "Out of space";


    explicit OutOfSpaceError(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    OutOfSpaceError(const OutOfSpaceError&) = default;


    OutOfSpaceError(OutOfSpaceError&&) = default;


    virtual ~OutOfSpaceError();
};

/*!
 * @brief Exception type for invalid data field in Request.
 */
class RequestedRecordNotPresentError : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0xCB;
    static constexpr const char ERROR_STRING[] = "Requested sensor, data or record not present";


    explicit RequestedRecordNotPresentError(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    RequestedRecordNotPresentError(const RequestedRecordNotPresentError&) = default;


    RequestedRecordNotPresentError(RequestedRecordNotPresentError&&) = default;


    virtual ~RequestedRecordNotPresentError();
};

/*!
 * @brief Exception type for invalid data field in Request.
 */
class InvalidDataFieldError : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0xCC;
    static constexpr const char ERROR_STRING[] = "Invalid data field in Request";


    explicit InvalidDataFieldError(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    InvalidDataFieldError(const InvalidDataFieldError&) = default;


    InvalidDataFieldError(InvalidDataFieldError&&) = default;


    virtual ~InvalidDataFieldError();
};

/*!
 * @brief Exception type for invalid data field in Request.
 */
class DestinationUnavailableError : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0xD3;
    static constexpr const char ERROR_STRING[] = "Destination unavailable";


    explicit DestinationUnavailableError(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    DestinationUnavailableError(const DestinationUnavailableError&) = default;


    DestinationUnavailableError(DestinationUnavailableError&&) = default;


    virtual ~DestinationUnavailableError();
};

/*!
 * @brief Exception type for invalid data field in Request.
 */
class CommandNotSupportedInPresentStateError : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0xD5;
    static constexpr const char ERROR_STRING[] = "Command not supported in present state";


    explicit CommandNotSupportedInPresentStateError(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    CommandNotSupportedInPresentStateError(const CommandNotSupportedInPresentStateError&) = default;


    CommandNotSupportedInPresentStateError(CommandNotSupportedInPresentStateError&&) = default;


    virtual ~CommandNotSupportedInPresentStateError();
};

/*!
 * @brief Exception type for invalid Request data length.
 */
class RequestDataLengthInvalidError : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0xC7;
    static constexpr const char ERROR_STRING[] = "Request data length invalid";


    explicit RequestDataLengthInvalidError(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    RequestDataLengthInvalidError(const RequestDataLengthInvalidError&) = default;


    RequestDataLengthInvalidError(RequestDataLengthInvalidError&&) = default;


    virtual ~RequestDataLengthInvalidError();
};

/*!
 * @brief Exception type for truncated Request data.
 */
class RequestDataTruncatedError : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0xC6;
    static constexpr const char ERROR_STRING[] = "Request data truncated";


    explicit RequestDataTruncatedError(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    RequestDataTruncatedError(const RequestDataTruncatedError&) = default;


    RequestDataTruncatedError(RequestDataTruncatedError&&) = default;


    virtual ~RequestDataTruncatedError();
};

/*!
 * @brief Exception type for reservation error.
 */
class ReservationError : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0xC5;
    static constexpr const char ERROR_STRING[] = "Reservation canceled or invalid reservation ID";


    explicit ReservationError(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    ReservationError(const ReservationError&) = default;


    ReservationError(ReservationError&&) = default;


    virtual ~ReservationError();
};

/*!
 * @brief Exception type for inability to return number of requested data bytes.
 */
class RequestedDataLengthError : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0xCA;
    static constexpr const char ERROR_STRING[] = "Cannot return number of requested data bytes";


    explicit RequestedDataLengthError(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    RequestedDataLengthError(const RequestedDataLengthError&) = default;


    RequestedDataLengthError(RequestedDataLengthError&&) = default;


    virtual ~RequestedDataLengthError();
};

/*!
 * @brief Exception type for unspecified error.
 */
class UnspecifiedError : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0xFF;
    static constexpr const char ERROR_STRING[] = "Unspecified error";


    explicit UnspecifiedError(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    UnspecifiedError(const UnspecifiedError&) = default;


    UnspecifiedError(UnspecifiedError&&) = default;


    virtual ~UnspecifiedError();
};


// COMMAND-SPECIFIC CODES 0x80 - 0xBE

/*!
 * @brief Exception type for parameter not supported error.
 */
class ParameterNotSupportedError : public ResponseError {
public:
    static constexpr CompletionCode ERROR_CODE = 0x80;
    static constexpr const char ERROR_STRING[] = "Parameter not supported";


    ParameterNotSupportedError(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    ParameterNotSupportedError(const ParameterNotSupportedError&) = default;


    ParameterNotSupportedError(ParameterNotSupportedError&&) = default;


    virtual ~ParameterNotSupportedError();
};

/*!
 * @brief Exception type for update in progress error.
 */
class UpdateInProgressError : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0x81;
    static constexpr const char ERROR_STRING[] = "Update in progress";

    explicit UpdateInProgressError(const Response& response)
        : ResponseError(response, ERROR_STRING, ERROR_CODE) {}


    UpdateInProgressError(const UpdateInProgressError&) = default;


    UpdateInProgressError(UpdateInProgressError&&) = default;


    virtual ~UpdateInProgressError();
};

/*!
 * @brief Exception type for incorrect response version.
 */
class IncorrectResponseVersionError : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0x82;
    static constexpr const char ERROR_STRING[] = "Incorrect response version";

    explicit IncorrectResponseVersionError(const Response& response);

    IncorrectResponseVersionError(const IncorrectResponseVersionError&) = default;

    IncorrectResponseVersionError(IncorrectResponseVersionError&&) = default;

    virtual ~IncorrectResponseVersionError();
};

/*!
 * @brief Exception type for SEL handling if erase in progress.
 */
class SelEraseInProgressError : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0x81;
    static constexpr const char ERROR_STRING[] = "SEL erase in progress";

    explicit SelEraseInProgressError(const Response& response);

    SelEraseInProgressError(const SelEraseInProgressError&) = default;

    SelEraseInProgressError(SelEraseInProgressError&&) = default;

    virtual ~SelEraseInProgressError();
};


/*!
 * @brief Exception type for SEL handling if (erase) done on wrong entity type
 */
class WrongEntityError : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0x80;
    static constexpr const char ERROR_STRING[] = "Operation not supported for this entity type";

    explicit WrongEntityError(const Response& response);

    WrongEntityError(const WrongEntityError&) = default;

    WrongEntityError(WrongEntityError&&) = default;

    virtual ~WrongEntityError();
};

/*!
 * @brief Exception type for MDR handling if an invalid MDR region is specified
 */
class InvalidMdrRegion : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0xC9;
    static constexpr const char ERROR_STRING[] = "Invalid MDR region specified";

    explicit InvalidMdrRegion(const Response& response);

    InvalidMdrRegion(const InvalidMdrRegion&) = default;

    InvalidMdrRegion(InvalidMdrRegion&&) = default;

    virtual ~InvalidMdrRegion();
};

/*!
 * @brief Exception type for MDR handling if the specified MDR region is in use
 */
class MdrRegionInUse : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0x81;
    static constexpr const char ERROR_STRING[] = "Specified MDR region in use";

    explicit MdrRegionInUse(const Response& response);

    MdrRegionInUse(const MdrRegionInUse&) = default;

    MdrRegionInUse(MdrRegionInUse&&) = default;

    virtual ~MdrRegionInUse();
};

/*!
 * @brief Exception type for MDR handling if the specified MDR region is locked
 */
class MdrRegionLocked : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0xD5;
    static constexpr const char ERROR_STRING[] = "Specified MDR region is locked";

    explicit MdrRegionLocked(const Response& response);

    MdrRegionLocked(const MdrRegionLocked&) = default;

    MdrRegionLocked(MdrRegionLocked&&) = default;

    virtual ~MdrRegionLocked();
};

/*!
 * @brief Exception type for MDR handling if the specified MDR region is not locked
 */
class MdrRegionNotLocked : public ResponseError {
public:
    static constexpr const CompletionCode ERROR_CODE = 0xD5;
    static constexpr const char ERROR_STRING[] = "Specified MDR region is not locked";

    explicit MdrRegionNotLocked(const Response& response);

    MdrRegionNotLocked(const MdrRegionNotLocked&) = default;

    MdrRegionNotLocked(MdrRegionNotLocked&&) = default;

    virtual ~MdrRegionNotLocked();
};

/*!
 * @brief Exception type for MDR handling if the requested data extends beyond length of region
 */
class MdrDataLengthError : public RequestDataLengthInvalidError {
public:
    static constexpr const char ERROR_STRING[] = "MDR request data length invalid";

    using RequestDataLengthInvalidError::RequestDataLengthInvalidError;

    virtual ~MdrDataLengthError();

    MdrDataLengthError(const MdrDataLengthError&) = default;

    MdrDataLengthError(MdrDataLengthError&&) = default;
};

/*!
 * @brief Exception type for MDR handling if the request is larger than maximum allowed payload size
 */
class MdrInvalidPayloadSize : public OutOfSpaceError {
public:
    static constexpr const char ERROR_STRING[] = "MDR request payload size invalid";

    using OutOfSpaceError::OutOfSpaceError;

    virtual ~MdrInvalidPayloadSize();

    MdrInvalidPayloadSize(const MdrInvalidPayloadSize&) = default;

    MdrInvalidPayloadSize(MdrInvalidPayloadSize&&) = default;
};

}
