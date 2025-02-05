//******************************************************************************
//
//                 Low Cost Vision
//
//******************************************************************************
// Project:        CreateReport
// File:           ReportHistogram.hpp
// Description:    This class is a ReportField containing a histogram representation of a list of int's or doubles.
// Author:         Wouter Langerak
// Notes:          
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
#include <vector>
#include <iostream>
#include <sstream>

class ReportHistogram : public ReportField {
public:

template<class T>
ReportHistogram(const std::vector<T>& vec, unsigned int bns, unsigned rnge) {
	bins = bns;
	range = rnge;

	for (unsigned int i = 0; i < bins; ++i) {
		binList.push_back(0);
	}
	for (unsigned int it = 0; it < vec.size(); it++) {
		std::cout << ((vec.at(it) / (range/bins)))+1 << std::endl;
		binList.at((vec.at(it) / (range/bins)))++ ;
	}

}

std::string toString() {
	std::stringstream ss;

	for (unsigned int i = 0; i < binList.size(); i++) {
		ss << "From " << (i * (range / bins)) << " - "
				<< ((i + 1) * (range / bins)) << ":\t" << binList.at(i)
				<< std::endl;
	}
	return ss.str();
}
private:
	unsigned int bins;
	unsigned int range;
	std::vector<int> binList;

};
