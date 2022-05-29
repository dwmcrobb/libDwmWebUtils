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
//!  \file DwmWebUtils.cc
//!  \author Daniel W. McRobb
//!  \brief Dwm::WebUtils function implementations
//---------------------------------------------------------------------------

#define BOOST_BEAST_USE_STD_STRING_VIEW

#include <iostream>
#include <regex>
#include <string_view>

#include <boost/asio.hpp>
#include <boost/certify/extensions.hpp>
#include <boost/certify/https_verification.hpp>

#include "DwmSysLogger.hh"
#include "DwmWebUtilsUrl.hh"
#include "DwmWebUtils.hh"

namespace Dwm {

  namespace WebUtils {

    namespace beast = boost::beast;
    namespace asio = boost::asio;
    namespace ssl = asio::ssl;
    using tcp = boost::asio::ip::tcp;

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static tcp::resolver::results_type
    resolve(asio::io_context& ctx, std::string const & hostname,
            const std::string & service)
    {
      tcp::resolver resolver{ctx};
      return resolver.resolve(hostname, service);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static tcp::socket
    Connect(asio::io_context & ctx, std::string const & hostname,
            const std::string & service)
    {
      tcp::socket socket{ctx};
      asio::connect(socket, resolve(ctx, hostname, service));
      return socket;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static std::unique_ptr<ssl::stream<tcp::socket>>
    Connect(asio::io_context & ctx, ssl::context & ssl_ctx,
            std::string const & hostname, const std::string & service)
    {
      std::unique_ptr<ssl::stream<tcp::socket>>  rc(nullptr);
      try {
        tcp::socket  sock = Connect(ctx, hostname, service);
        rc = boost::make_unique<ssl::stream<tcp::socket>>(std::move(sock),
                                                          ssl_ctx);
        Syslog(LOG_INFO, "Connected to %s:%s", hostname.c_str(),                                                       
               service.c_str());                                                                                                   }
      catch (...) {
        rc = nullptr;
        Syslog(LOG_ERR, "Failed to connect to %s:%s", hostname.c_str(),
               service.c_str());
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static bool HttpsHandshake(ssl::stream<tcp::socket> * strm,
                               std::string const & hostname)
    {
      bool  rc = false;
      assert(strm);
      try {
        boost::certify::set_server_hostname(*strm, hostname);
        boost::certify::sni_hostname(*strm, hostname);
        strm->handshake(ssl::stream_base::handshake_type::client);
        rc = true;
      }
      catch (...) {
        Syslog(LOG_ERR, "HTTPS handshake with %s failed", hostname.c_str());
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    static http::response<http::string_body>
    Get(T & stream, std::string_view hostname, std::string_view urlstr)
    {
      http::request<http::string_body> request;
      request.method(http::verb::get);
      request.target(urlstr);
      request.keep_alive(false);
      request.set(http::field::host, hostname);
      request.set(http::field::user_agent, "mcrover/1.0");
      http::write(stream, request);
      http::response<http::string_body> response;
      beast::flat_buffer buffer;
      http::read(stream, buffer, response);
      
      return response;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static bool
    GetResponseViaHttp(const Url & url,
                       http::response<http::string_body> & response,
                       GetFailure & failure)
    {
      bool  rc = false;
      asio::io_context ctx;
      beast::tcp_stream stream(ctx);
      try {
        stream.connect(resolve(ctx, url.Host(), std::to_string(url.Port())));
      }
      catch (...) {
        failure.FailNum(GetFailure::k_failNumConnect);
        return false;
      }

      try {
        response = Get(stream, url.Host(), url.AfterAuthority());
        failure.FailNum(GetFailure::k_failNumNone);
        rc = true;
      }
      catch (...) {
        failure.FailNum((response.result() == http::status::ok)
                        ? GetFailure::k_failNumGet : response.result_int());
      }
      
      beast::error_code ec;
      stream.socket().shutdown(tcp::socket::shutdown_both, ec);
    
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static bool
    GetResponseViaHttp(const Url & url,
                       http::response<http::string_body> & response)
    {
      GetFailure  failure;
      return GetResponseViaHttp(url, response, failure);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static bool 
    GetResponseViaHttps(const Url & url,
                        http::response<http::string_body> & response,
                        GetFailure & failure, bool verifyCertificate)
    {
      bool  rc = false;
      asio::io_context ctx;
      ssl::context ssl_ctx{ssl::context::tls_client};
      if (verifyCertificate) {
        ssl_ctx.set_verify_mode(ssl::context::verify_peer |
                                ssl::context::verify_fail_if_no_peer_cert);
      }
      else {
        ssl_ctx.set_verify_mode(ssl::verify_none);
      }
      ssl_ctx.set_default_verify_paths();
      boost::certify::enable_native_https_server_verification(ssl_ctx);
      std::unique_ptr<ssl::stream<tcp::socket>>  stream_ptr;
      try {
        stream_ptr = Connect(ctx, ssl_ctx, url.Host(),
                             std::to_string(url.Port()));
        if (! stream_ptr) {
          failure.FailNum(GetFailure::k_failNumConnect);
          return false;
        }
      }
      catch (...) {
        failure.FailNum(GetFailure::k_failNumConnect);
        return false;
      }

      if (HttpsHandshake(stream_ptr.get(), url.Host())) {
        try {
          response = Get(*stream_ptr, url.Host(), url.AfterAuthority());
          failure.FailNum(GetFailure::k_failNumNone);
          rc = true;
        }
        catch (...) {
          failure.FailNum((response.result() == http::status::ok)
                          ? GetFailure::k_failNumGet : response.result_int());
        }
      }
      else {
        failure.FailNum(GetFailure::k_failNumAuth);
      }

      if (stream_ptr) {
        boost::system::error_code  ec;
        stream_ptr->shutdown(ec);
        stream_ptr->next_layer().close(ec);
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static bool 
    GetResponseViaHttps(const Url & url,
                        http::response<http::string_body> & response,
                        bool verifyCertificate)
    {
      GetFailure  failure;
      return GetResponseViaHttps(url, response, failure, verifyCertificate);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static bool GetResponse(const Url & url,
                            http::response<http::string_body> & response,
                            GetFailure & failure, bool verifyCertificate)
    {
      bool  rc = false;
      if (url.Scheme() == "https") {
        rc = GetResponseViaHttps(url, response, failure, verifyCertificate);
      }
      else if (url.Scheme() == "http") {
        rc = GetResponseViaHttp(url, response, failure);
      }
      else {
        failure.FailNum(GetFailure::k_failNumURL);
        Syslog(LOG_ERR, "Unhandled URL scheme '%s'", url.Scheme().c_str());
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static bool GetResponse(const Url & url,
                            http::response<http::string_body> & response,
                            bool verifyCertificate)
    {
      GetFailure  failure;
      return GetResponse(url, response, failure, verifyCertificate);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool GetResponse(const std::string & urlstr,
                     http::response<http::string_body> & response,
                     GetFailure & getFail, bool verifyCertificate)
    {
      bool  rc = false;
      Url  url;
      if (url.Parse(urlstr)) {
        rc = GetResponse(url, response, getFail, verifyCertificate);
      }
      else {
        getFail.FailNum(GetFailure::k_failNumURL);
        Syslog(LOG_ERR, "Failed to parse URL '%s'", urlstr.c_str());
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool GetResponse(const std::string & urlstr,
                     http::response<http::string_body> & response,
                     bool verifyCertificate)
    {
      GetFailure  failure;
      return GetResponse(urlstr, response, failure, verifyCertificate);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    int GetStatus(const std::string & urlstr, GetFailure & failure,
                  bool verifyCertificate)
    {
      int  rc = -1;
      Url  url;
      if (url.Parse(urlstr)) {
        http::response<http::string_body>  response;
        if (GetResponse(url, response, failure, verifyCertificate)) {
          rc = response.result_int();
          failure.FailNum(GetFailure::k_failNumNone);
        }
      }
      else {
        failure.FailNum(GetFailure::k_failNumURL);
        Syslog(LOG_ERR, "Failed to parse URL '%s'", urlstr.c_str());
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    int GetStatus(const std::string & urlstr, bool verifyCertificate)
    {
      int  rc = -1;
      Url  url;
      if (url.Parse(urlstr)) {
        http::response<http::string_body>  response;
        if (GetResponse(url, response, verifyCertificate)) {
          rc = response.result_int();
        }
      }
      else {
        Syslog(LOG_ERR, "Failed to parse URL '%s'", urlstr.c_str());
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool GetJson(const std::string & urlstr, nlohmann::json & json,
                 GetFailure & getFailure, bool verifyCertificate)
    {
      bool  rc = false;
      Url  url;
      if (url.Parse(urlstr)) {
        http::response<http::string_body>  response;
        if (GetResponse(url, response, getFailure, verifyCertificate)) {
          json = nlohmann::json::parse(response.body(), nullptr, false);
          if (! json.is_discarded()) {
            getFailure.FailNum(GetFailure::k_failNumNone);
            rc = true;
          }
          else {
            getFailure.FailNum(GetFailure::k_failNumJSON);
          }
        }
      }
      else {
        Syslog(LOG_ERR, "Failed to parse URL '%s'", urlstr.c_str());
        getFailure.FailNum(GetFailure::k_failNumURL);
      }
      return rc;
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    bool GetJson(const std::string & urlstr, nlohmann::json & json,
                 bool verifyCertificate)
    {
      GetFailure  getFailure;
      return GetJson(urlstr, json, getFailure, verifyCertificate);
    }

    
  }  // namespace WebUtils

}  // namespace Dwm
