//******************************************************************************
//
//                 Low Cost Vision
//
//******************************************************************************
// Project:        BOWClassifier
// File:           BOWClassifier.h
// Description:    A Bag of Words KeyPoint classifier
// Author:         Jules Blok
// Notes:          None
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

#include <BOWClassifier/BOWClassifier.h>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

BOWClassifier::~BOWClassifier() {
	delete classifier;
}

bool BOWClassifier::load(const std::string& path) {
	cout << "Loading classifier..." << endl;
	classifier->load(path.c_str());

	cout << "Loading vocabulary..." << endl;
	FileStorage fs(path, FileStorage::READ);
	Mat vocabulary;
	fs["vocabulary"] >> vocabulary;
	if (vocabulary.empty()) {
		cout << "Failed to load vocabulary" << endl;
		return false;
	}
	bowExtractor->setVocabulary(vocabulary);
	fs.release();

	return true;
}

bool BOWClassifier::save(const std::string& path) {
	cout << "Saving classifier..." << endl;
	classifier->save(path.c_str());

	cout << "Saving vocabulary..." << endl;
	FileStorage fs(path, FileStorage::APPEND);
	if (!fs.isOpened())
		return false;
	fs << "vocabulary" << bowExtractor->getVocabulary();
	fs.release();

	return true;
}
