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
//!  \file DwmWebUtils.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::WebUtils function declarations
//---------------------------------------------------------------------------

#ifndef _DWMWEBUTILS_HH_
#define _DWMWEBUTILS_HH_

#include <string>
// #define BOOST_BEAST_USE_STD_STRING_VIEW  // deprecated
#include <boost/beast.hpp>
#include <nlohmann/json.hpp>

#include "DwmWebUtilsGetFailure.hh"

namespace Dwm {

  namespace WebUtils {

    namespace http = boost::beast::http;

    //------------------------------------------------------------------------
    //!  Fetches the JSON at the given URL @c urlstr and stores it in @c json.
    //!  Returns true on success, false on failure.
    //------------------------------------------------------------------------
    bool GetJson(const std::string & urlstr, nlohmann::json & json,
                 bool verifyCertificate = true);

    //------------------------------------------------------------------------
    //!  Fetches the JSON at the given URL @c urlstr and stores it in @c json.
    //!  Returns true on success, false on failure.  On failure,
    //!  @c getFailure can be examined for more information.  If @c urlstr is
    //!  an https URL, will verify the server's certificate unless
    //!  @c verifyCertificate is @c false.
    //------------------------------------------------------------------------
    bool GetJson(const std::string & urlstr, nlohmann::json & json,
                 GetFailure & getFailure, bool verifyCertificate = true);
    
    //------------------------------------------------------------------------
    //!  Returns the status code of the given URL @c urlstr.  If we can't
    //!  get a status code at all, returns -1.  If @c urlstr is an https URL,
    //!  will verify the server's certificate unless @c verifyCertificate is
    //!  @c false.                   
    //------------------------------------------------------------------------
    int GetStatus(const std::string & urlstr, bool verifyCertificate = true);

    //------------------------------------------------------------------------
    //!  Returns the status code of the given URL @c urlstr.  If we can't
    //!  get a status code at all, returns -1.  On failure, @c getFailure can
    //!  be examined for more information.
    //!  If @c urlstr is an https URL, will verify the server's   
    //!  certificate unless @c verifyCertificate is @c false.
    //------------------------------------------------------------------------
    int GetStatus(const std::string & urlstr, GetFailure & getFailure,
                  bool verifyCertificate = true);
    
    //------------------------------------------------------------------------
    //!  Fetches the web page at the given URL @c urlstr and stores it in
    //!  @c response.  If @c urlstr is an https URL, will verify the server's
    //!  certificate unless @c verifyCertificate is @c false.  Returns true
    //!  on success, false on failure.
    //------------------------------------------------------------------------
    bool GetResponse(const std::string & urlstr,
                     http::response<http::string_body> & response,
                     bool verifyCertificate = true);

    //------------------------------------------------------------------------
    //!  Fetches the web page at the given URL @c urlstr and stores it in
    //!  @c response.  If @c urlstr is an https URL, will verify the
    //!  server's certificate unless @c verifyCertificate is @c false.
    //!  Returns true on success, false on failure.  On failure,
    //!  @c getFailure can be examined for more information.
    //------------------------------------------------------------------------
    bool GetResponse(const std::string & urlstr,
                     http::response<http::string_body> & response,
                     GetFailure & getFailure, bool verifyCertificate = true);

  }  // namespace WebUtils

}  // namespace Dwm

#endif  // _DWMWEBUTILS_HH_
