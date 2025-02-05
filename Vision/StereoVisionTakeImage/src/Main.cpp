//******************************************************************************
//
//                 Low Cost Vision
//
//******************************************************************************
// Project:        StereoVisionTakeImage
// File:           example_main.cpp
// Description:    Program witch use 2 camera's for making 2 images an calibrate them
// Author:         Glenn Meerstra
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

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <iostream>
#include <vector>
#include <cstdio>
#include <cstring>
#include <limits>

#include "RectifyStereo.h"

using namespace std;

int main(int argc, char** argv){

	RectifyStereo rs;
	if(argc < 2){
		std::cout << "to few arguments:\n"
			"   -t train\n"
			"   -c correct\n"
			"   -si create stereo images\n"
		<< endl;
		return -1;
	}

	if(!(strcmp(argv[1], "-t"))){

		if(argc < 9){
			std::cout << "to few arguments:\n"
					"   -bw {value}   value: the horizontal amount of squares\n"
					"   -bh {value}   value: the vertical amount of squares\n"
					"   -il {value}   value: the xml file with image names\n"
					"   -xn {value}   value: the name for the resulting xml file\n\n" << std::endl;
			return -1;
		}

		int allValues = 0;

		int BH, BW;
		std::string imageListName, xmlName;

		for(int i = 2; i < argc; i++){
			if(!(strcmp(argv[i], "-bw"))){
				BW = atoi(argv[i+1]);
				allValues = allValues | 1;
			}else if(!(strcmp(argv[i], "-bh"))){
				BH = atoi(argv[i+1]);
				allValues = allValues | 2;
			}else if(!(strcmp(argv[i], "-il"))){
				imageListName = argv[i+1];
				allValues = allValues | 4;
			}else if(!(strcmp(argv[i], "-xn"))){
				xmlName = argv[i+1];
				allValues = allValues | 8;
			}
		}

		if(allValues != 15){
			std::cout << "The arguments aren't correct" << std::endl;
			return -1;
		}

		std::cout << "CREATE MATRIX" << std::endl;
		cv::Size boardSize(BW,BH);

		vector<string> list;
		if(rs.readStringList(imageListName, list) && rs.StereoCalib(list, xmlName, cv::Size(9, 6))){
			cout << "DONE TRAINING" << endl << endl;
			return 0;
		}

		cout << "FAILED" << endl << endl;
		return -1;

	}else if(!(strcmp(argv[1], "-c")) || !(strcmp(argv[1], "-si"))){

		if(argc < 7 && !(strcmp(argv[1], "-c"))){
			std::cout << "to few arguments:\n"
					"   -cam {value} {value} value: the device number of the cameras\n"
					"   -xn  {value}         value: the name for the correcting xml file\n"
					"   -iml {value}         value: the path to the left image\n"
					"   -imR {value}         value: the path to the rigth image\n"
					"   -sn  {value}         value: the value at which the image number starts\n"
					"\n"
					"When using the camera's the starting value is nessecary,\n"
					"the left and right image aren't supposed to be filled in\n"
					"\n"
					"When using images, the cam and the starting image number\n"
					"aren't supposed to be filled in\n\n"
					<< std::endl;
			return -1;
		}else if(argc < 7 && !(strcmp(argv[1], "-si"))){
			std::cout << "to few arguments:\n"
					"   -cam {value} {value} value: the device number of the cameras\n"
					"   -sn  {value}         value: the value at which the image number starts\n\n"
					<< std::endl;
			return -1;
		}

		cv::VideoCapture camL, camR;

		int camNrL = 1;
		int camNrR = 2;
		int imageNr = 0;
		int allValues = 0;
		cv::Mat imageL, imageR;
		std::string xmlFileName;

		for(int i = 2; i < argc; i++){
			if(!(strcmp(argv[i], "-cam"))){
				camNrL = atoi(argv[i+1]);
				camNrR = atoi(argv[i+2]);
				allValues = allValues | 1;
			}else if(!(strcmp(argv[i], "-xn"))){
				xmlFileName = argv[i+1];
				allValues = allValues | 2;
			}else if(!(strcmp(argv[i], "-iml"))){
				imageL = cv::imread(argv[i+1]);
				if(!imageL.data){
					continue;
				}
				allValues = allValues | 4;
			}else if(!(strcmp(argv[i], "-imr"))){
				imageR = cv::imread(argv[i+1]);
				if(!imageR.data){
					continue;
				}
				allValues = allValues | 8;
			}else if(!(strcmp(argv[i], "-sn"))){
				imageNr = atoi(argv[i+1]);
				allValues = allValues | 16;
			}
		}

 		if(allValues != 30 && allValues != 19 && !(strcmp(argv[1], "-c"))){
			std::cout << "The arguments aren't correct\n"
				"\n"
				"When using the camera's the starting value is nessecary,\n"
				"the left and right image aren't supposed to be filled in\n"
				"\n"
				"When using images, the cam and the starting image number\n"
				"aren't supposed to be filled in\n\n"
			<< std::endl;
			return -1;
		}else if(allValues != 17 && !(strcmp(argv[1], "-si"))){
			std::cout << "The arguments aren't correct" << std::endl;
			return -1;
		}

		int key = 0;
		if(allValues != 30){
			camL = cv::VideoCapture(camNrL);
			camR = cv::VideoCapture(camNrR);

			if(!camL.isOpened() || !camR.isOpened()){
				std::cout << "A camera isn't connected: \n"
					"   camNr: " << camNrL << ", " << camNrR << "\n"
					"\n"
					"check /dev/video.. and see if the numbers are the same" <<
				std::endl;

				return -1;
			}

			camL.read(imageL);

			cvNamedWindow("LEFT");
			cvNamedWindow("RIGHT");
			cvMoveWindow("LEFT", 0, 0);
			cvMoveWindow("RIGHT", imageL.cols, 0);

			std::cout << "WAIT ON c(ontinue) || s(witch cams)" << std::endl;
			while((key = cv::waitKey(10)) != 'c'){
				camL.read(imageL);
				camR.read(imageR);

				imshow("LEFT", imageL);
				imshow("RIGHT", imageR);

				if(key == 's'){
					camL.release();
					camR.release();

					int temp = camNrL;
					camNrL = camNrR;
					camNrR = temp;

					camL = cv::VideoCapture(camNrL);
					camR = cv::VideoCapture(camNrR);
				}else if(key  == 'q'){
					return 0;
				}
			}
		}

		if(!(strcmp(argv[1], "-c"))){
			std::cout << "CORRECT" << std::endl;

			if(!rs.initRectifyImage(xmlFileName)){
				cout << "COULDN'T READ XML" << endl;
				return -1;
			}

			if(allValues == 19){
				camL.read(imageL);
				camR.read(imageR);

				cv::Mat imageL2 = imageL.clone(), imageR2 = imageR.clone();

				cout << "WAIT ON q(uit) || s(ave image) || d(rawOnImage)" << endl;
				bool drawOnImage = false;
				while(key != 'q'){

					camL.read(imageL);
					camR.read(imageR);
					if(key == 'd'){
						drawOnImage= !drawOnImage;
					}
					rs.RectifyImage(imageL, imageL2, imageR, imageR2,drawOnImage);
					cv::imshow("LEFT", imageL2);
					cv::imshow("RIGHT", imageR2);

					key = cv::waitKey(10);

					if(key == 's'){
						stringstream sl;
						sl << "left" << imageNr << ".jpg";
						cv::imwrite(sl.str().c_str(), imageL2);
						stringstream sr;
						sr << "right" << imageNr << ".jpg";
						cv::imwrite(sr.str().c_str(), imageR2);
						imageNr++;
					}

				}
			}else if(allValues == 30){
				cv::Mat imageL2 = imageL.clone(), imageR2 = imageR.clone();

				rs.RectifyImage(imageL, imageL2, imageR, imageR2);

				stringstream sl;
				sl << "left" << imageNr << ".jpg";
				cv::imwrite(sl.str().c_str(), imageL2);
				stringstream sr;
				sr << "right" << imageNr << ".jpg";
				cv::imwrite(sr.str().c_str(), imageR2);
			}

		}else{
			std::cout << "CREATE TRAINING SET" << std::endl;
			cout << "WAIT ON p(icture) || q(uit)" << endl;

			while(key != 'q'){
				camL.read(imageL);
				camR.read(imageR);

				key = cv::waitKey(10);

				if(key == 'p'){
					stringstream s;
					s << "src/RectifyImages/left" << imageNr << ".jpg";
					imwrite(s.str().c_str(), imageL.clone());
					s.str("");
					s << "src/RectifyImages/right" << imageNr << ".jpg";
					imwrite(s.str().c_str(), imageR.clone());
					key = 0;
					cout << "image" << imageNr << endl;
					key = cv::waitKey(10);
					imageNr++;
				}
				cv::imshow("LEFT", imageL);
				cv::imshow("RIGHT", imageR);
			}
		}
		cout << "STOPPED" << endl << endl;
	}

	return 0;
}
