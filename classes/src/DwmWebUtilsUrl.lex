%{
  //=========================================================================
  // @(#) $DwmPath$
  //=========================================================================
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
  //=========================================================================

  //-------------------------------------------------------------------------
  //!  \file DwmWebUtilsUrl.lex
  //!  \author Daniel W. McRobb
  //!  \brief Dwm::WebUtils::Url class implementation
  //-------------------------------------------------------------------------

  #include <iostream>
  #include <regex>
  #include <unordered_map>
  #include <boost/asio.hpp>
  
  #include "DwmWebUtilsUrl.hh"
%}

m_alpha    [A-Za-z]
m_digit    [0-9]
m_scheme   {m_alpha}({m_alpha}|{m_digit}|[+\-.])*
m_port     [0-9]{1,5}
m_hexdig   [0-9A-Fa-f]
m_ip4octet [0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5]
m_ip4addr  {m_ip4octet}\.{m_ip4octet}\.{m_ip4octet}\.{m_ip4octet}
m_h16      [0-9A-Fa-f]{1,4}
m_ls32     {m_h16}:{m_h16}|{m_ip4addr}
m_ip6addr  ({m_h16}:){6}{m_ls32}|::({m_h16}:){5}{m_ls32}|({m_h16})?::({m_h16}:){4}{m_ls32}|(({m_h16}:){0,1}{m_h16})?::({m_h16}:){3}{m_ls32}|(({m_h16}:){0,2}{m_h16})?::({m_h16}:){2}{m_ls32}|(({m_h16}:){0,3}{m_h16})?::{m_h16}:{m_ls32}|(({m_h16}:){0,4}{m_h16})?::{m_ls32}|(({m_h16}:){0,5}{m_h16})?::{m_h16}|(({m_h16}:){0,6}{m_h16})?::
m_iunreserved   ({m_alpha}|{m_digit}|[\-\._~])
m_pctencoded    (\%{m_hexdig}{m_hexdig})
m_subdelims     [\!\$\&\'\(\)\*\+,;=]
m_ireg_name     ({m_alpha}|{m_digit})({m_iunreserved}|{m_pctencoded}|{m_subdelims})*
m_pchar         ({m_iunreserved}|{m_pctencoded}|{m_subdelims}|[:@])
m_isegment      ({m_pchar})*
m_ipath_abempty ("/"{m_isegment})*
m_iuserinfo     ({m_iunreserved}|{m_pctencoded}|{m_subdelims}|[:])*

%option prefix="url_"
%option 8bit reentrant stack noyywrap debug
%option extra-type="Dwm::WebUtils::Url *"

%x x_ihier
%x x_iauth
%x x_ip6addr
%x x_port
%x x_path
%x x_query
%x x_fragment

%%

<INITIAL>{
  ^{m_scheme}   { yyextra->Scheme(yytext); }
  ":"           { yy_push_state(x_ihier, yyscanner); }
}

<x_ihier>"//"   { yy_push_state(x_iauth, yyscanner); }

<x_iauth>{
  {m_iuserinfo}\@    { std::string  s(yytext);
                       yyextra->UserInfo(s.substr(0, s.size() - 1)); }
  "["                           { yy_push_state(x_ip6addr, yyscanner); }
  ({m_ip4addr}|{m_ireg_name})   { yyextra->Host(yytext); }
  ":"                           { yy_push_state(x_port, yyscanner); }
  "/"                           {
    unput('/'); yy_push_state(x_path, yyscanner);
  }
}

<x_ip6addr>{
  {m_ip6addr}   { yyextra->Host(yytext); }
  "]"           { yy_pop_state(yyscanner); }
}

<x_port>{
  {m_port}      { yyextra->Port(std::stoul(yytext, nullptr, 10)); }
  "/"           { unput('/'); yy_pop_state(yyscanner); }
}

<x_path>{
  {m_ipath_abempty}   { yyextra->Path(yytext); }
  "?"                 { yy_push_state(x_query, yyscanner); }
}

<x_query>{
  ({m_pchar}|[/?])*   { yyextra->Query(yytext); }
  [ \t]               { return -1; }
  "#"                 { unput('#'); yy_pop_state(yyscanner); }
}

<x_path>"#"                     { yy_push_state(x_fragment, yyscanner); }

<x_fragment>({m_pchar}|[/?])*   { yyextra->Fragment(yytext); }

<*>.                            { return -1; }
\n                              { BEGIN(INITIAL); }

%%

namespace Dwm {

  namespace WebUtils {

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Url::HasAlpha(const std::string & host)
    {
      std::regex   rgx("[a-zA-Z]");
      std::smatch  sm;
      return regex_search(host.begin(), host.end(), sm, rgx);
    }
  
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Url::HasIllegalHypens(const std::string & host)
    {
      std::regex   rgx("([\\-][\\-])|([\\-][\\.])|([\\-]$)");
      std::smatch  sm;
      return regex_search(host.begin(), host.end(), sm, rgx);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Url::IsAddress(const std::string & host)
    {
      bool  rc = false;
      try {
        boost::asio::ip::address  a = boost::asio::ip::make_address(host);
        rc = true;
      }
      catch (...) {
      }
      return rc;
    }
  
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Url::IsZeroAddress(const std::string & host)
    {
      bool  rc = false;
      try {
        boost::asio::ip::address  a = boost::asio::ip::make_address(host);
        boost::asio::ip::address  z4 = boost::asio::ip::make_address("0.0.0.0");
        boost::asio::ip::address  z6 =
          boost::asio::ip::make_address("0:0:0:0:0:0:0:0");
        rc = ((a == z4) || (a == z6));
      }
      catch (...) {
      }
      return rc;
    }
  
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Url::IsMulticast(const std::string & host)
    {
      bool  rc = false;
      try {
        boost::asio::ip::address  a = boost::asio::ip::make_address(host);
        rc = a.is_multicast();
      }
      catch (...) {
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    uint16_t Url::PortFromScheme(const std::string & scheme)
    {
      static const std::unordered_map<std::string,uint16_t>  ports = {
        { "ftp",   21 },
        { "http",  80 },
        { "https", 443 }
      };
      auto  it = ports.find(scheme);
      if (it != ports.end()) {
        return it->second;
      }
      return 0;
    }
          
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    Url::Url(const std::string & url)
    {
      Parse(url);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool Url::Parse(const std::string & url)
    {
      Clear();
      yyscan_t  scanner;
      url_lex_init_extra(this, &scanner);
      // url_set_debug(1, scanner);
      url__switch_to_buffer(url__scan_string(url.c_str(), scanner), scanner);
      int  lexrc = url_lex(scanner);
      if (0 == _port) {
        _port = PortFromScheme(_scheme);
      }
      if (_path.empty()) {
        _path = "/";
      }
      
      bool  rc = false;
      if (lexrc != -1) {
        if (0 != _port) {
          if (! _host.empty()) {
            if (IsAddress(_host)) {
              if ((! IsZeroAddress(_host))
                  && (! IsMulticast(_host))) {
                rc = true;
              }
            }
            else {
              if ((! HasIllegalHypens(_host))
                  && HasAlpha(_host)) {
                rc = true;
              }
            }
          }
        }
      }
      return rc;
    }
  
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    void Url::Clear()
    {
      _scheme.clear();
      _userinfo.clear();
      _host.clear();
      _port = 0;
      _path.clear();
      _query.clear();
      _fragment.clear();
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    std::string Url::AfterAuthority() const
    {
      std::string  rc(_path);
      if (! _query.empty()) {
        rc += '?' + _query;
      }
      if (! _fragment.empty()) {
        rc += '#' + _fragment;
      }
      return rc;
    }

  }  // namespace WebUtils
  
}  // namespace Dwm
