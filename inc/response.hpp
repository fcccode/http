// This file is a part of the IncludeOS unikernel - www.includeos.org
//
// Copyright 2015-2016 Oslo and Akershus University College of Applied Sciences
// and Alfred Bratterud
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef HTTP_RESPONSE_HPP
#define HTTP_RESPONSE_HPP

#include "message.hpp"
#include "status_line.hpp"

namespace http {

/**
 * @brief This class is used to represent
 * an HTTP response message
 */
class Response : public Message {
private:
  //------------------------------
  // Internal class type aliases
  using Code  = status_t;
  //------------------------------
public:
  /**
   * @brief Constructor to set up a response
   * by providing information for the
   * status line of the response message
   *
   * @param code:
   * The status code
   *
   * @param version:
   * The version of the message
   */
  explicit Response(const Code code = OK, const Version version = Version{}) noexcept;

  /**
   * @brief Construct a response message from the
   * incoming character stream of data which is a
   * {std::string} object
   *
   * @tparam T response:
   * The character stream of data
   *
   * @param limit:
   * Capacity of how many fields can be added to the header section
   */
  template
  <
    typename T,
    typename = std::enable_if_t
               <std::is_same
               <std::string, std::remove_const_t
               <std::remove_reference_t<T>>>::value>
  >
  explicit Response(T&& response, const Limit limit = 100);

  /**
   * @brief  Default copy constructor
   */
  Response(const Response&) = default;

  /**
   * @brief Default move constructor
   */
  Response(Response&&) noexcept = default;

  /**
   * @brief Default destructor
   */
  ~Response() noexcept = default;

  /**
   * @brief Default copy assignment operator
   */
  Response& operator = (const Response&) = default;

  /**
   * @brief Default move assignment operator
   */
  Response& operator = (Response&&) = default;

  /**
   * @brief Get the status code of this message
   *
   * @return The status code of this message
   */
  Code status_code() const noexcept;

  /**
   * @brief Change the status code of this message
   *
   * @param code:
   * The new status code to set on this message
   *
   * @return The object that invoked this method
   */
  Response& set_status_code(const Code code) noexcept;

  /**
   * @brief Reset the response message as if it was now
   * default constructed
   *
   * @return The object that invoked this method
   */
  virtual Response& reset() noexcept override;
  
  /**
   * @brief Get a string representation of this
   * class
   *
   * @return A string representation
   */
  virtual std::string to_string() const override;

  /**
   * @brief Operator to transform this class
   * into string form
   */
  operator std::string () const;
  //-----------------------------------
private:
  //------------------------------
  // Class data members
  Status_line status_line_;
  //------------------------------
}; //< class Response

/**--v----------- Implementation Details -----------v--**/

inline Response::Response(const Code code, const Version version) noexcept
  : status_line_{version, code}
{}

template <typename Egress, typename>
inline Response::Response(Egress&& response, const Limit limit)
  : Message{limit}
  , status_line_{response}
{
  add_headers(response);
  std::size_t start_of_body;
  if ((start_of_body = response.find("\r\n\r\n")) not_eq std::string::npos) {
    add_body(response.substr(start_of_body + 4));
  } else if ((start_of_body = response.find("\n\n")) not_eq std::string::npos) {
    add_body(response.substr(start_of_body + 2));
  }
}

inline Response::Code Response::status_code() const noexcept {
  return static_cast<status_t>(status_line_.get_code());
}

inline Response& Response::set_status_code(const Code code) noexcept {
  status_line_.set_code(code);
  return *this;
}

inline Response& Response::reset() noexcept {
  Message::reset();
  return set_status_code(OK);
}

inline std::string Response::to_string() const {
  std::ostringstream res;
  //-----------------------------------
  res << status_line_
      << Message::to_string();
  //-----------------------------------
  return res.str();
}

inline Response::operator std::string () const {
  return to_string();
}

inline Response& operator << (Response& res, const Header_set& headers) {
  for (const auto& field : headers) {
    res.add_header(field.first, field.second);
  }
  return res;
}

inline Response_ptr make_response(buffer_t buf, const size_t len) {
  return std::make_unique<Response>(std::string{reinterpret_cast<char*>(buf.get()), len});
}

inline std::ostream& operator << (std::ostream& output_device, const Response& res) {
  return output_device << res.to_string();
}

/**--^----------- Implementation Details -----------^--**/

} //< namespace http

#endif //< HTTP_RESPONSE_HPP
