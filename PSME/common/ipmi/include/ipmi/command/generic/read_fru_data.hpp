/*!
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
 * @file command/generic/read_fru_data.hpp
 */

 #pragma once

 #include "ipmi/request.hpp"
 #include "ipmi/response.hpp"

 namespace ipmi {
 namespace command {
 namespace generic {

 namespace request {

 /*!
  * @brief Request message for ReadFruData.
  */
 class ReadFruData : public Request {
 public:

     /*!
      * @brief Default constructor.
      */
     ReadFruData();

     /*! Copy constructor. */
     ReadFruData(const ReadFruData&) = default;

     /*! Assignment operator */
     ReadFruData& operator=(const ReadFruData&) = default;

     /*!
      * @brief Default destructor.
      */
     virtual ~ReadFruData();

     const char* get_command_name() const override {
         return "ReadFruData";
     }

     /*!
      * @brief Sets device id.
      * @param device_id device id.
      */
     void set_device_id(std::uint8_t device_id) {
         m_device_id = device_id;
     }

     /*!
      * @brief Sets offset (in bytes/words depending on access mode).
      * @param offset the offset_id to set.
      */
     void set_offset(std::uint16_t offset) {
         m_offset = offset;
     }

     /*!
      * @brief Sets count of bytes to read
      * @param count Number of bytes to read.
      */
     void set_bytes_to_read(std::uint8_t count) {
         m_count = count;
     }

 private:
     std::uint8_t m_device_id{0};
     std::uint16_t m_offset{0};
     std::uint8_t m_count{0};

     void pack(IpmiInterface::ByteBuffer& data) const override;
 };

 }

 namespace response {

 /*!
  * @brief Response message for ReadFruData.
  */
 class ReadFruData : public Response {
 public:

     /*!
      * @brief Default constructor.
      */
     ReadFruData();

     /*! Copy constructor. */
     ReadFruData(const ReadFruData&) = default;

     /*! Assignment operator */
     ReadFruData& operator=(const ReadFruData&) = default;

     /*!
      * @brief Default destructor.
      */
     virtual ~ReadFruData();

     const char* get_command_name() const override {
         return "ReadFruData";
     }

     /*!
      * @brief Gets read fru data.
      * @return fru data
      */
     IpmiInterface::ByteBuffer get_data() const {
         return m_data;
     }

 private:
     static constexpr std::size_t MIN_RESPONSE_SIZE = 2;
     static constexpr std::uint8_t OFFSET_COUNT = 1;
     static constexpr std::uint8_t OFFSET_DATA = 2;

     IpmiInterface::ByteBuffer m_data{};

     void unpack(const IpmiInterface::ByteBuffer& data) override;
 };
 }

 }
 }
 }
