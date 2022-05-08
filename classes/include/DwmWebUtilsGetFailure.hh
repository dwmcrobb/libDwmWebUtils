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
//!  \file DwmWebUtilsGetFailure.hh
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#ifndef _DWMWEBUTILSGETFAILURE_HH_
#define _DWMWEBUTILSGETFAILURE_HH_

#include <cstdint>
#include <string>

namespace Dwm {

  namespace WebUtils {

    //------------------------------------------------------------------------
    //!  Encapsulates failure information for a GET request.
    //------------------------------------------------------------------------
    class GetFailure
    {
    public:
      //----------------------------------------------------------------------
      //!  Failure type categories.
      //----------------------------------------------------------------------
      typedef enum {
        //--------------------------------------------------------------------
        //!  No failure.
        //--------------------------------------------------------------------
        e_failTypeNone,
        //--------------------------------------------------------------------
        //!  Failed to parse URL
        //--------------------------------------------------------------------
        e_failTypeURL,
        //--------------------------------------------------------------------
        //!  connect() failure
        //--------------------------------------------------------------------
        e_failTypeConnect,
        //--------------------------------------------------------------------
        //!  failed to validate server certificate (https)
        //--------------------------------------------------------------------
        e_failTypeAuthenticateServer,
        //--------------------------------------------------------------------
        //!  incorrect response to GET request.
        //--------------------------------------------------------------------
        e_failTypeGet,
        //--------------------------------------------------------------------
        //!  Bad JSON in GET response.
        //--------------------------------------------------------------------
        e_failTypeJSON,
        //--------------------------------------------------------------------
        //!  other errors
        //--------------------------------------------------------------------
        e_failTypeOther
      } FailTypeEnum;

      //----------------------------------------------------------------------
      //!  Special failure number values.
      //!
      //!  @c k_failNumURL indicates URL parse failure
      //!  @c k_failNumConnect indicates a connect() failure
      //!  @c k_failNumAuth indicates failure to validate server certificate
      //!  @c k_failNumGet indicates GET request failure (unknown http header)
      //!  @c k_failNumJSON indicates failure to parse JSON in GET response
      //!  @c k_failNumUnknown indicates an unknown failure
      //!
      //!  The range 100 to 999 are reserved for HTTP response codes.
      //!  Failure numbers in this range map to FailType() of
      //!  @c e_failTypeGet.
      //----------------------------------------------------------------------
      static constexpr uint16_t  k_failNumNone    = 0;
      static constexpr uint16_t  k_failNumURL     = 1;
      static constexpr uint16_t  k_failNumConnect = 2;
      static constexpr uint16_t  k_failNumAuth    = 3;
      static constexpr uint16_t  k_failNumGet     = 4;
      static constexpr uint16_t  k_failNumJSON    = 5;
      static constexpr uint16_t  k_failNumUnknown = 0xFFFF;
      
      //----------------------------------------------------------------------
      //!  Constructor
      //----------------------------------------------------------------------
      GetFailure(uint16_t failNum = k_failNumNone);

      //----------------------------------------------------------------------
      //!  Returns the failure number.
      //----------------------------------------------------------------------
      uint16_t FailNum() const  { return _failNum; }

      //----------------------------------------------------------------------
      //!  Sets and returns the failure number.
      //----------------------------------------------------------------------
      uint16_t FailNum(uint16_t failNum)  { return _failNum = failNum; }
      
      //----------------------------------------------------------------------
      //!  
      //----------------------------------------------------------------------
      operator bool () const { return (_failNum != k_failNumNone); }

      //----------------------------------------------------------------------
      //!  Returns the failure type.
      //----------------------------------------------------------------------
      FailTypeEnum FailType() const;
      
      //----------------------------------------------------------------------
      //!  Returns a string representing the failure.
      //----------------------------------------------------------------------
      std::string to_string() const;
      
    private:
      uint16_t  _failNum;
    };
    
  }  // namespace WebUtils

}  // namespace Dwm

#endif  // _DWMWEBUTILSGETFAILURE_HH_
