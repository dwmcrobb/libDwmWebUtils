//===========================================================================
// @(#) $DwmPath$
//===========================================================================
//  Copyright (c) Daniel W. McRobb 2022, 2024
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
//!  \file DwmWebUtilsUrl.hh
//!  \author Daniel W. McRobb
//!  \brief Dwm::WebUtils::Url class declaration
//---------------------------------------------------------------------------

#ifndef _DWMWEBUTILSURL_HH_
#define _DWMWEBUTILSURL_HH_

#include <string>
#include <boost/asio.hpp>

namespace Dwm {

  namespace WebUtils {

    //------------------------------------------------------------------------
    //!  A simple encapsulation of a URL.
    //------------------------------------------------------------------------
    class Url
    {
    public:
      //----------------------------------------------------------------------
      //!  Default constructor
      //----------------------------------------------------------------------
      Url() = default;

      //----------------------------------------------------------------------
      //!  Convenience constructor to Construct from a string.  Throws
      //!  std::invalid_argument if @c url can't be parsed.
      //----------------------------------------------------------------------
      Url(const std::string & url);

      //----------------------------------------------------------------------
      //!  Parses @c url and stores the parsed components in @c this.
      //!  Returns true on success, false on failure.
      //----------------------------------------------------------------------
      bool Parse(const std::string & url);

      //----------------------------------------------------------------------
      //!  Clears the Url (sets the contents the same as the default
      //!  constructor).
      //----------------------------------------------------------------------
      void Clear();
    
      //----------------------------------------------------------------------
      //!  Returns the scheme portion of the Url.
      //----------------------------------------------------------------------
      const std::string & Scheme() const
      { return _scheme; }

      //----------------------------------------------------------------------
      //!  Sets and returns the scheme portion of the Url.
      //----------------------------------------------------------------------
      const std::string & Scheme(const std::string & scheme)
      { return _scheme = scheme; }

      //----------------------------------------------------------------------
      //!  Returns the user info portion of the Url.
      //----------------------------------------------------------------------
      const std::string & UserInfo() const
      { return _userinfo; }

      //----------------------------------------------------------------------
      //!  Sets and returns the user info portion of the Url.
      //----------------------------------------------------------------------
      const std::string & UserInfo(const std::string & userinfo)
      { return _userinfo = userinfo; }

      //----------------------------------------------------------------------
      //!  Returs the host portion of the Url.
      //----------------------------------------------------------------------
      const std::string & Host() const
      { return _host; }
    
      //----------------------------------------------------------------------
      //!  Sets and returns the host portion of the Url.
      //----------------------------------------------------------------------
      const std::string &	Host(const std::string & host)
      { return _host = host; }
    
      //----------------------------------------------------------------------
      //!  Returns the port portion of the Url.
      //----------------------------------------------------------------------
      uint16_t Port() const
      { return _port; }
    
      //----------------------------------------------------------------------
      //!  Sets and returns the port portion of the Url.
      //----------------------------------------------------------------------
      uint16_t Port(uint16_t port)
      { return _port = port; }
    
      //----------------------------------------------------------------------
      //!  Returns the path portion of the Url.
      //----------------------------------------------------------------------
      const std::string & Path() const
      { return _path; }
    
      //----------------------------------------------------------------------
      //!  Sets and returns the path portion of the Url.
      //----------------------------------------------------------------------
      const std::string & Path(const std::string & path)
      { return _path = path; }
    
      //----------------------------------------------------------------------
      //!  Returns the query portion of the Url.
      //----------------------------------------------------------------------
      const std::string & Query() const
      { return _query; }
    
      //----------------------------------------------------------------------
      //!  Sets and returns the query portion of the Url.
      //----------------------------------------------------------------------
      const std::string & Query(const std::string & query)
      { return _query = query; }
    
      //----------------------------------------------------------------------
      //!  Returns the fragment portion of the Url.
      //----------------------------------------------------------------------
      const std::string & Fragment() const
      { return _fragment; }
    
      //----------------------------------------------------------------------
      //!  Sets and returns the fragment portion of the Url.
      //----------------------------------------------------------------------
      const std::string & Fragment(const std::string & fragment)
      { return _fragment = fragment; }

      //----------------------------------------------------------------------
      //!  Convenience member that returns a string of the form
      //!  'path?query#fragment'
      //----------------------------------------------------------------------
      std::string AfterAuthority() const;

      //----------------------------------------------------------------------
      //!  Returns the Url as a string.
      //----------------------------------------------------------------------
      std::string to_string() const;

      //----------------------------------------------------------------------
      //!  Equality comparison operator
      //----------------------------------------------------------------------
      bool operator == (const Url & url) const;
      
    private:
      std::string   _scheme;
      std::string   _userinfo;
      std::string   _host;
      uint16_t      _port;
      std::string   _path;
      std::string   _query;
      std::string   _fragment;

      static bool HasIllegalHyphens(const std::string & host);
      static bool HasAlpha(const std::string & host);
      static bool IsAddress(const std::string & host);
      static bool IsZeroAddress(const std::string & host);
      static bool IsMulticast(const std::string & host);
      static uint16_t PortFromScheme(const std::string & scheme);
    };

  }  // namespace WebUtils
  
}  // namespace Dwm

#endif  // _DWMWEBUTILSURL_HH_
