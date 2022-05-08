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
//!  \file DwmWebUtilsGetFailure.cc
//!  \author Daniel W. McRobb
//!  \brief NOT YET DOCUMENTED
//---------------------------------------------------------------------------

#include "DwmWebUtilsGetFailure.hh"

namespace Dwm {

  namespace WebUtils {

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    GetFailure::GetFailure(uint16_t failNum)
        : _failNum(failNum)
    {}
    
    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    GetFailure::FailTypeEnum GetFailure::FailType() const
    {
      FailTypeEnum  rc = e_failTypeNone;
      switch (_failNum) {
        case k_failNumURL:      rc = e_failTypeURL;                 break;
        case k_failNumConnect:  rc = e_failTypeConnect;             break;
        case k_failNumAuth:     rc = e_failTypeAuthenticateServer;  break;
        case k_failNumGet:      rc = e_failTypeGet;                 break;
        case k_failNumJSON:     rc = e_failTypeJSON;                break;
        default:
          if ((100 <= _failNum) && (1000 > _failNum)) {
            rc = e_failTypeGet;
          }
          else {
            rc = e_failTypeOther;
          }
          break;
      }
      return rc;
    }

    //------------------------------------------------------------------------
    //!  
    //------------------------------------------------------------------------
    std::string GetFailure::to_string() const
    {
      std::string  rc;
      switch (FailType()) {
        case e_failTypeNone:
          rc = "NONE";
          break;
        case e_failTypeURL:
          rc = "URL";
          break;
        case e_failTypeConnect:
          rc = "CONNECT";
          break;
        case e_failTypeAuthenticateServer:
          rc = "AUTH";
          break;
        case e_failTypeGet:
          rc = "GET:" + std::to_string(_failNum);
          break;
        case e_failTypeJSON:
          rc = "JSON";
          break;
        case e_failTypeOther:
          rc = std::to_string(_failNum);
          break;
        default:
          break;
      }
      return rc;
    }
    
  }  // namespace WebUtils

}  // namespace Dwm
