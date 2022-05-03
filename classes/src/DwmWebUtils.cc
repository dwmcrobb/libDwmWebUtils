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

namespace beast = boost::beast;
namespace asio = boost::asio;
namespace ssl = asio::ssl;
namespace http = boost::beast::http;
using tcp = boost::asio::ip::tcp;

namespace Dwm {

  namespace WebUtils {

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    tcp::resolver::results_type
    resolve(asio::io_context& ctx, std::string const & hostname,
            const std::string & service)
    {
      tcp::resolver resolver{ctx};
      return resolver.resolve(hostname, service);
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    tcp::socket
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
    std::unique_ptr<ssl::stream<tcp::socket>>
    Connect(asio::io_context & ctx, ssl::context & ssl_ctx,
            std::string const & hostname, const std::string & service)
    {
      auto stream =
        boost::make_unique<ssl::stream<tcp::socket>>(Connect(ctx, hostname,
                                                             service),
                                                     ssl_ctx);
      boost::certify::set_server_hostname(*stream, hostname);
      boost::certify::sni_hostname(*stream, hostname);
      boost::system::error_code  ec;
      try {
        stream->handshake(ssl::stream_base::handshake_type::client);
      }
      catch (std::exception & ex) {
        Syslog(LOG_ERR, "Failed to connect to '%s:%s': %s", hostname.c_str(),
               service.c_str(), ex.what());
        throw;
      }
      catch (...) {
        Syslog(LOG_ERR, "Failed to connect to '%s:%s'", hostname.c_str(),
               service.c_str());
        throw;
      }
      return stream;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    template <typename T>
    http::response<http::string_body>
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
    static int GetHttpStatus(const Url & url)
    {
      int                rc = 0;
      asio::io_context   ctx;
      beast::tcp_stream  stream(ctx);
      try {
        stream.connect(resolve(ctx, url.Host(), url.Scheme()));
        auto  response = Get(stream, url.Host(), url.AfterAuthority());
        rc = response.result_int();
      }
      catch (...) {
      }
      beast::error_code ec;
      stream.socket().shutdown(tcp::socket::shutdown_both, ec);
    
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static int GetHttpsStatus(const Url & url)
    {
      int               rc = 0;
      asio::io_context  ctx;
      ssl::context      ssl_ctx{ssl::context::tls_client};
      ssl_ctx.set_verify_mode(ssl::context::verify_peer |
                              ssl::context::verify_fail_if_no_peer_cert);
      ssl_ctx.set_default_verify_paths();
      boost::certify::enable_native_https_server_verification(ssl_ctx);
      std::unique_ptr<ssl::stream<tcp::socket>>  stream_ptr;
      try {
        stream_ptr = Connect(ctx, ssl_ctx, url.Host(), url.Scheme());
        auto  response = Get(*stream_ptr, url.Host(), url.AfterAuthority());
        rc = response.result_int();
      }
      catch (const boost::system::system_error & ex) {
        Syslog(LOG_ERR, "Exception getting status for %s://%s%s: %s",
               url.Scheme().c_str(), url.Host().c_str(),
               url.AfterAuthority().c_str(), ex.what());
      }
      catch (...) {
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
    int GetStatus(const std::string & urlstr)
    {
      int  rc = 0;
      Url  url;
      if (url.Parse(urlstr)) {
        if (url.Scheme() == "https") {
          rc = GetHttpsStatus(url);
        }
        else if (url.Scheme() == "http") {
          rc = GetHttpStatus(url);
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
    bool
    GetResponseViaHttp(const Url & url,
                       http::response<http::string_body> & response)
    {
      bool  rc = false;
      asio::io_context ctx;
      beast::tcp_stream stream(ctx);
      try {
        stream.connect(resolve(ctx, url.Host(), url.Scheme()));
        response = Get(stream, url.Host(), url.AfterAuthority());
        rc = true;
      }
      catch (...) {
        Syslog(LOG_ERR, "Got exception");
      }
      beast::error_code ec;
      stream.socket().shutdown(tcp::socket::shutdown_both, ec);
    
      return rc;
    }
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static bool GetJsonViaHttp(const Url & url, nlohmann::json & json)
    {
      bool  rc = false;
      http::response<http::string_body> response;
      if (GetResponseViaHttp(url, response)) {
        json = nlohmann::json::parse(response.body());
        rc = true;
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    static bool 
    GetResponseViaHttps(const Url & url,
                        http::response<http::string_body> & response)
    {
      bool  rc = false;
      asio::io_context ctx;
      ssl::context ssl_ctx{ssl::context::tls_client};
      ssl_ctx.set_verify_mode(ssl::context::verify_peer |
                              ssl::context::verify_fail_if_no_peer_cert);
      ssl_ctx.set_default_verify_paths();
      boost::certify::enable_native_https_server_verification(ssl_ctx);
      std::unique_ptr<ssl::stream<tcp::socket>>  stream_ptr;
      try {
        stream_ptr = Connect(ctx, ssl_ctx, url.Host(), url.Scheme());
        response = Get(*stream_ptr, url.Host(), url.AfterAuthority());
        rc = true;
      }
      catch (const std::exception & ex) {
        Syslog(LOG_ERR, "Got exception: %s", ex.what());
      } 
      catch (...) {
        Syslog(LOG_ERR, "Got exception");
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
    static bool GetJsonViaHttps(const Url & url, nlohmann::json & json)
    {
      bool  rc = false;
      asio::io_context ctx;
      ssl::context ssl_ctx{ssl::context::tls_client};
      ssl_ctx.set_verify_mode(ssl::context::verify_peer |
                              ssl::context::verify_fail_if_no_peer_cert);
      ssl_ctx.set_default_verify_paths();
      boost::certify::enable_native_https_server_verification(ssl_ctx);
      std::unique_ptr<ssl::stream<tcp::socket>>  stream_ptr;
      try {
        stream_ptr = Connect(ctx, ssl_ctx, url.Host(), url.Scheme());
        auto  response = Get(*stream_ptr, url.Host(), url.AfterAuthority());
        if (response.result_int() == 200) {
          json = nlohmann::json::parse(response.body());
          rc = true;
        }
      }
      catch (...) {
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
    bool GetResponse(const std::string & urlstr,
                     http::response<http::string_body> & response)
    {
      bool  rc = false;
      Url  url;
      if (url.Parse(urlstr)) {
        if (url.Scheme() == "https") {
          rc = GetResponseViaHttps(url, response);
        }
        else if (url.Scheme() == "http") {
          rc = GetResponseViaHttp(url, response);
        }
        else {
          Syslog(LOG_ERR, "Unhandled URL scheme '%s'", urlstr.c_str());
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
    bool GetJson(const std::string & urlstr, nlohmann::json & json)
    {
      bool  rc = false;
      Url  url;
      if (url.Parse(urlstr)) {
        if (url.Scheme() == "https") {
          rc = GetJsonViaHttps(url, json);
        }
        else if (url.Scheme() == "http") {
          rc = GetJsonViaHttp(url, json);
        }
      }
      else {
        Syslog(LOG_ERR, "Failed to parse URL '%s'", urlstr.c_str());
      }
      return rc;
    }

    
  }  // namespace WebUtils

}  // namespace Dwm
