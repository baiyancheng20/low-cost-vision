//******************************************************************************
//
//                 Low Cost Vision
//
//******************************************************************************
// Project:        TestBenchTools
// File:           Cell.cpp
// Description:    Base Class of IntegerCell, DoubleCell and StringCell.
// Author:         Wouter Langerak
// Notes:          
//
// License:        GNU GPL v3
//
// This file is part of TestBenchTools.
//
// TestBenchTools is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// TestBenchTools is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with TestBenchTools.  If not, see <http://www.gnu.org/licenses/>.
//******************************************************************************

#pragma once
#include <string>

namespace report {

////////////////////////////////////////////////////////
enum Type { INT,DOUBLE,STRING};
//////////////////////////////////////////////////////

/**
 * Base Class of IntegerCell, DoubleCell and StringCell
 */
class Cell {
public:
	Cell(Type t) : type (t) {}
	virtual std::string toString() {return "";}
	virtual ~Cell(){}
private:
	Type type;
};

}
