//******************************************************************************
//
//                 Low Cost Vision
//
//******************************************************************************
// Project:        huniplacer
// File:           huniplacer.h
// Description:    convenience include
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

#include <huniplacer/utils.h>
#include <huniplacer/crd514_kd_exception.h>
#include <huniplacer/CRD514_KD.h>
#include <huniplacer/deltarobot.h>
#include <huniplacer/imotor3.h>
#include <huniplacer/inverse_kinematics_exception.h>
#include <huniplacer/inverse_kinematics_impl.h>
#include <huniplacer/inverse_kinematics_model.h>
#include <huniplacer/measures.h>
#include <huniplacer/modbus_ctrl.h>
#include <huniplacer/modbus_exception.h>
#include <huniplacer/motion.h>
#include <huniplacer/point3.h>
#include <huniplacer/steppermotor3.h>
#include <huniplacer/motor3_exception.h>
#include <huniplacer/effector_boundaries.h>
