//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2022
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions
//  are met:
//
//  1. Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//  2. Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//  3. The names of the authors and copyright holders may not be used to
//     endorse or promote products derived from this software without
//     specific prior written permission.
//
//  IN NO EVENT SHALL DANIEL W. MCROBB BE LIABLE TO ANY PARTY FOR
//  DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES,
//  INCLUDING LOST PROFITS, ARISING OUT OF THE USE OF THIS SOFTWARE,
//  EVEN IF DANIEL W. MCROBB HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
//  DAMAGE.
//
//  THE SOFTWARE PROVIDED HEREIN IS ON AN "AS IS" BASIS, AND
//  DANIEL W. MCROBB HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT,
//  UPDATES, ENHANCEMENTS, OR MODIFICATIONS. DANIEL W. MCROBB MAKES NO
//  REPRESENTATIONS AND EXTENDS NO WARRANTIES OF ANY KIND, EITHER
//  IMPLIED OR EXPRESS, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE,
//  OR THAT THE USE OF THIS SOFTWARE WILL NOT INFRINGE ANY PATENT,
//  TRADEMARK OR OTHER RIGHTS.
//===========================================================================

//---------------------------------------------------------------------------
//!  \file TestGetResponse.cc
//!  \author Daniel W. McRobb
//!  \brief Unit tests for Dwm::WebUtils::GetResponse
//---------------------------------------------------------------------------

#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

#include "DwmSysLogger.hh"
#include "DwmUnitAssert.hh"
#include "DwmWebUtils.hh"

using namespace std;
using namespace boost::beast;
using namespace Dwm::WebUtils;

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
template<bool isRequest, class Body, class Fields>
static void PrintWholeMessage(http::message<isRequest,Body,Fields> const & m)
{
  boost::system::error_code ec;
  http::serializer<isRequest, Body, Fields> sr{m};
  do
  {
    sr.next(ec,
            [&sr](boost::system::error_code & ec, auto const & buffer)
            {
              ec = {};
              std::cout << make_printable(buffer);
              sr.consume(buffer_bytes(buffer));
            });
  }
  while (! ec && ! sr.is_done());
  if (! ec) {
    std::cout << std::endl;
  }
  else {
    std::cerr << ec.message() << std::endl;
  }
  return;
}

//----------------------------------------------------------------------------
//!  
//----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
  Dwm::SysLogger::Open("TestGetResponse", LOG_PID|LOG_PERROR, LOG_USER);

  string  urlstr("https://api.weather.gov/stations/KPTK/observations/latest");
  http::response<http::string_body>  response;
  if (UnitAssert(GetResponse(urlstr, response))) {
    nlohmann::json  json =
      nlohmann::json::parse(response.body(), nullptr, false);
    UnitAssert(! json.is_discarded());
    // PrintWholeMessage(response);
  }

  GetFailure  getFail;
  if (UnitAssert(GetResponse(urlstr, response, getFail))) {
    UnitAssert(! getFail);
  }
  
  if (Dwm::Assertions::Total().Failed())
    Dwm::Assertions::Print(cerr, true);
  else
    cout << Dwm::Assertions::Total() << " passed" << endl;

}
