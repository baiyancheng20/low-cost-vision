//******************************************************************************
//
//                 Low Cost Vision
//
//******************************************************************************
// Project:        huniplacer
// File:           crd514_kd_exception.h
// Description:    exception thrown if the motorcontroller alarm flag is set
// Author:         Lukas Vermond & Kasper van Nieuwland
// Notes:          -
//
// License: newBSD 
//  
// Copyright © 2012, HU University of Applied Sciences Utrecht. 
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
// - Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
// - Neither the name of the HU University of Applied Sciences Utrecht nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE HU UNIVERSITY OF APPLIED SCIENCES UTRECHT
// BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
// OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//******************************************************************************


#pragma once

#include <huniplacer/CRD514_KD.h>

#include <stdexcept>
#include <string>
#include <sstream>

namespace huniplacer
{
	/**
	 * @brief raised if the alarm flag is set
	 **/
	class crd514_kd_exception : public std::runtime_error
	{
		private:
			const crd514_kd::slaves::t slave;
			const bool warning, alarm;
			std::string message;

		public:
			crd514_kd_exception(const crd514_kd::slaves::t slave, const bool warning, const bool alarm) :
				std::runtime_error(""),
				slave(slave),
				warning(warning),
				alarm(alarm)
			{
				std::stringstream ss;
				ss << "slave: " << (int)slave << ": warning=" << (int)warning << " alarm=" << (int)alarm;
				message = ss.str();
			}

			virtual ~crd514_kd_exception() throw()
			{ }

			virtual const char* what() const throw()
			{
				return message.c_str();
			}

			crd514_kd::slaves::t get_slave(void)
			{
				return slave;
			}

			bool get_warning(void)
			{
				return warning;
			}

			bool get_alarm(void)
			{
				return alarm;
			}
	};
}
