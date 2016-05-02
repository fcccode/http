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

#ifndef HTTP_REQUEST_LINE_HPP
#define HTTP_REQUEST_LINE_HPP

#include <cctype>

#include "common.hpp"
#include "methods.hpp"
#include "version.hpp"

namespace http {

//-----------------------------------
// This class represents the request-line
// of an incoming http request message
//-----------------------------------
class Request_Line {
public:
  //-----------------------------------
  // Constructor to create a default
  // request-line which is as follows:
  //
  // <GET / HTTP/1.1>
  //-----------------------------------
  explicit Request_Line() = default;

  //-----------------------------------
  // Constructor to construct a request-line
  // from the incoming character stream of
  // data which is a <std::string> object
  //
  // @tparam (std::string) request - The character stream of
  //                                 data
  //-----------------------------------
  template <typename Request>
  explicit Request_Line(Request&& request);

  //-----------------------------------
  // Default copy constructor
  //-----------------------------------
  Request_Line(const Request_Line&) = default;

  //-----------------------------------
  // Default move constructor
  //-----------------------------------
  Request_Line(Request_Line&&) = default;

  //-----------------------------------
  // Default destructor
  //-----------------------------------
  ~Request_Line() noexcept = default;

  //-----------------------------------
  // Default copy assignment operator
  //-----------------------------------
  Request_Line& operator = (const Request_Line&) = default;

  //-----------------------------------
  // Default move assignment operator
  //-----------------------------------
  Request_Line& operator = (Request_Line&&) = default;

  //-----------------------------------
  // Get the method of the message
  //
  // @return - The method of the message
  //-----------------------------------
  const Method& get_method() const noexcept;

  //-----------------------------------
  // Set the method of the message
  //
  // @param method - The method of the message
  //-----------------------------------
  void set_method(const Method& method);

  //-----------------------------------
  // Get the URI of the message
  //
  // @return - The URI of the message
  //-----------------------------------
  const URI& get_uri() const noexcept;

  //-----------------------------------
  // Set the URI of the message
  //
  // @param uri - The URI of the message
  //-----------------------------------
  void set_uri(const URI& uri);

  //-----------------------------------
  // Get the version of the message
  //
  // @return - The version of the message
  //-----------------------------------
  const Version& get_version() const noexcept;

  //-----------------------------------
  // Set the version of the message
  //
  // @param version - The version of the message
  //-----------------------------------
  void set_version(const Version& version) noexcept;

  //-----------------------------------
  // Get a string representation of this
  // class
  //
  // @return - A string representation
  //-----------------------------------
  std::string to_string() const;

  //-----------------------------------
  // Operator to transform this class
  // into string form
  //-----------------------------------
  operator std::string () const;
  //-----------------------------------
private:
  //-----------------------------------
  // Class data members
  //-----------------------------------
  Method  method_  {method::GET};
  URI     uri_     {"/"};
  Version version_ {1U, 1U};
}; //< class Request_Line

/**--v----------- Implementation Details -----------v--**/

template <typename Request>
inline Request_Line::Request_Line(Request&& request) {
  if (request.empty() or request.size() < 16 /*<-(16) minimum request length */) {
    return;
  }
  //-----------------------------------
  std::string start {request.substr(request.find_first_not_of("\f\t\v "))};
  //-----------------------------------
  std::string rl {start.substr(0, start.find("\r\n"))};
  //-----------------------------------
  method_ = rl.substr(0, rl.find_first_of(" "));
  //-----------------------------------
  rl = rl.substr(rl.find_first_of(" ") + 1);
  //-----------------------------------
  uri_ = rl.substr(0, rl.find_last_of(" "));
  //-----------------------------------
  rl = rl.substr(rl.find_last_of(" ") + 1);
  //-----------------------------------
  std::string major {rl.substr(rl.find("/") + 1, rl.find("."))};
  std::string minor {rl.substr(rl.find(".") + 1)};
  //-----------------------------------
  unsigned maj = static_cast<unsigned>(std::stoul(major));
  unsigned min = static_cast<unsigned>(std::stoul(minor));
  //-----------------------------------
  version_ = Version{maj, min};
  //-----------------------------------
  request = request.substr(request.find("\r\n") + 2);
}

inline const Method& Request_Line::get_method() const noexcept {
  return method_;
}

inline void Request_Line::set_method(const Method& method) {
  method_ = method;
}

inline const URI& Request_Line::get_uri() const noexcept {
  return uri_;
}

inline void Request_Line::set_uri(const URI& uri) {
  uri_ = uri;
}

inline const Version& Request_Line::get_version() const noexcept {
  return version_;
}

inline void Request_Line::set_version(const Version& version) noexcept {
  version_ = version;
}

inline std::string Request_Line::to_string() const {
  return *this;
}

inline Request_Line::operator std::string () const {
  std::ostringstream req_line;
  //-----------------------------
  req_line << method_  << " "
           << uri_     << " "
           << version_ << "\r\n";
 //------------------------------
  return req_line.str();
}

inline std::ostream& operator << (std::ostream& output_device, const Request_Line& req_line) {
  return output_device << req_line.to_string();
}

/**--^----------- Implementation Details -----------^--**/

} //< namespace http

#endif //< HTTP_REQUEST_LINE_HPP
