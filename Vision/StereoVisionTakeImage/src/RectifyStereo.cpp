//******************************************************************************
//
//                 Low Cost Vision
//
//******************************************************************************
// Project:        StereoVisionTakeImage
// File:           RectifyStereo.cpp
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
#include "RectifyStereo.h"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

using namespace cv;
using namespace std;

bool RectifyStereo::StereoCalib(const vector<string>& imagelist, const string& xmlName,	Size boardSize) {
	if (imagelist.size() % 2 != 0) {
		cout<< "Error: the image list contains odd (non-even) number of elements\n";
		return false;
	}

	const int maxScale = 5;
	const float squareSize = 1.f; // Set this to your actual square size
	// ARRAY AND VECTOR STORAGE:

	vector<vector<Point2f> > imagePoints[2];
	vector<vector<Point3f> > objectPoints;
	Size imageSize;

	int i, j, k, nimages = (int) imagelist.size() / 2;

	imagePoints[0].resize(nimages);
	imagePoints[1].resize(nimages);
	vector<string> goodImageList;

	for (i = j = 0; i < nimages; i++) {
		for (k = 0; k < 2; k++) {
			const string& filename = imagelist[i * 2 + k];
			Mat img = imread(filename, 0);

			if (img.empty()) {
				break;
			}

			if (imageSize == Size()) {
				imageSize = img.size();
			} else if (img.size() != imageSize) {
				cout<< filename	<< " is different from the first image, Skipping the pair\n";
				break;
			}

			bool found = false;
			vector<Point2f>& corners = imagePoints[k][j];
			for (int scale = 1; scale <= maxScale; scale++) {

				Mat timg;
				if (scale == 1) {
					timg = img;
				} else {
					resize(img, timg, Size(), scale, scale);
				}

				found = findChessboardCorners(
						timg,
						boardSize,
						corners,
						CV_CALIB_CB_ADAPTIVE_THRESH	| CV_CALIB_CB_NORMALIZE_IMAGE);

				if (found) {
					if (scale > 1) {
						Mat cornersMat(corners);
						cornersMat *= 1. / scale;
					}
					break;
				}
			}

			if (!found) {
				break;
			}

			cornerSubPix(img, corners, Size(11, 11), Size(-1, -1),
					TermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS,
					30, 0.01));
		}

		if (k == 2) {
			goodImageList.push_back(imagelist[i * 2]);
			goodImageList.push_back(imagelist[i * 2 + 1]);
			j++;
		}
	}

	cout << j << " pairs have been successfully detected.\n";
	nimages = j;

	if (nimages < 10) {
		cout << "Error: too little pairs to run the calibration\n";
		return false;
	}

	imagePoints[0].resize(nimages);
	imagePoints[1].resize(nimages);
	objectPoints.resize(nimages);

	for (i = 0; i < nimages; i++) {
		for (j = 0; j < boardSize.height; j++) {
			for (k = 0; k < boardSize.width; k++) {
				objectPoints[i].push_back(Point3f(j * squareSize, k * squareSize, 0));
			}
		}
	}

	(cout << "Running stereo calibration ... ").flush();

	Mat cameraMatrix[2], distCoeffs[2];
	cameraMatrix[0] = Mat::eye(3, 3, CV_64F);
	cameraMatrix[1] = Mat::eye(3, 3, CV_64F);
	Mat R, T, E, F;

	double rms = stereoCalibrate(
			objectPoints,
			imagePoints[0],
			imagePoints[1],
			cameraMatrix[0],
			distCoeffs[0],
			cameraMatrix[1],
			distCoeffs[1],
			imageSize,
			R,T,E,F,
			TermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 100, 1e-5),
			CV_CALIB_FIX_ASPECT_RATIO + CV_CALIB_ZERO_TANGENT_DIST
					+ CV_CALIB_SAME_FOCAL_LENGTH + CV_CALIB_RATIONAL_MODEL
					+ CV_CALIB_FIX_K3 + CV_CALIB_FIX_K4 + CV_CALIB_FIX_K5);
	cout << "RMS error = " << rms << endl;

	Mat R1, R2, P1, P2, Q;

	stereoRectify(cameraMatrix[0], distCoeffs[0], cameraMatrix[1],
			distCoeffs[1], imageSize, R, T, R1, R2, P1, P2, Q,
			CALIB_ZERO_DISPARITY, 1, imageSize, &validRoi[0], &validRoi[1]);

	initUndistortRectifyMap(cameraMatrix[0], distCoeffs[0], R1, P1, imageSize,
			CV_16SC2, rmap[0][0], rmap[0][1]);
	initUndistortRectifyMap(cameraMatrix[1], distCoeffs[1], R2, P2, imageSize,
			CV_16SC2, rmap[1][0], rmap[1][1]);

	FileStorage fs(xmlName, CV_STORAGE_WRITE);
	if (fs.isOpened()) {
		fs << "RMAP00" << rmap[0][0]
		    << "RMAP01" << rmap[0][1]
			<< "RMAP10" << rmap[1][0]
			<< "RMAP11" << rmap[1][1]
			<< "ROI1_X" << validRoi[0].x
			<< "ROI2_X" << validRoi[1].x
			<< "ROI1_Y" << validRoi[0].y
			<< "ROI2_Y" << validRoi[1].y
			<< "ROI1_W" << validRoi[0].width
			<< "ROI2_W" << validRoi[1].width
			<< "ROI1_H" << validRoi[0].height
			<< "ROI2_H" << validRoi[1].height;
		fs.release();
	} else {
		return false;
	}

	return true;
}

bool RectifyStereo::initRectifyImage(const std::string& xmlName) {

	FileStorage fs(xmlName, CV_STORAGE_READ);
	if(fs.isOpened()){
		fs["RMAP00"] >> rmap[0][0];
		fs["RMAP01"] >> rmap[0][1];
		fs["RMAP10"] >> rmap[1][0];
		fs["RMAP11"] >> rmap[1][1];
		fs["ROI1_X"] >> validRoi[0].x;
		fs["ROI2_X"] >> validRoi[1].x;
		fs["ROI1_Y"] >> validRoi[0].y;
		fs["ROI2_Y"] >> validRoi[1].y;
		fs["ROI1_W"] >> validRoi[0].width;
		fs["ROI2_W"] >> validRoi[1].width;
		fs["ROI1_H"] >> validRoi[0].height;
		fs["ROI2_H"] >> validRoi[1].height;

		if(!rmap[0][0].data || !rmap[0][1].data || !rmap[1][1].data || !rmap[1][1].data){
			return false;
		}
	}else{
		return false;
	}
	fs.release();
	return true;
}

void RectifyStereo::RectifyImage(const Mat inputL, Mat &outputL,
		const Mat inputR, Mat &outputR, bool drawOnImage) {
	remap(inputL, outputL, rmap[0][0], rmap[0][1], CV_INTER_LINEAR);
	remap(inputR, outputR, rmap[1][0], rmap[1][1], CV_INTER_LINEAR);

	if(drawOnImage){

		if(inputL.size() == inputR.size()){

			double sf = 600. / MAX(inputL.size().width, inputL.size().height);
			Rect vroiL = Rect(cvRound(validRoi[0].x * sf), cvRound(validRoi[0].y * sf),
					cvRound( validRoi[0].width * sf), cvRound(validRoi[0].height * sf));

			Rect vroiR = Rect(cvRound(validRoi[1].x * sf), cvRound(validRoi[1].y * sf),
					cvRound( validRoi[1].width * sf), cvRound(validRoi[1].height * sf));

			rectangle(outputR, vroiL, Scalar(0,0,255), 2, 8);
			rectangle(outputL, vroiL, Scalar(0,0,255), 2, 8);
			for (int j = outputL.rows; j > 0; j -= outputL.rows / 20) {
				line(outputL, Point(0, j), Point(outputL.cols, j), Scalar(0, 255, 0), 1,8);
				line(outputR, Point(0, j), Point(outputR.cols, j), Scalar(0, 255, 0), 1,8);
			}
		}
	}
}

bool RectifyStereo::readStringList(const string& filename,	vector<string>& list) {
	list.resize(0);
	FileStorage fs(filename, FileStorage::READ);
	if (!fs.isOpened())
		return false;
	FileNode n = fs.getFirstTopLevelNode();
	if (n.type() != FileNode::SEQ)
		return false;
	FileNodeIterator it = n.begin(), it_end = n.end();
	for (; it != it_end; ++it)
		list.push_back((string) *it);
	return true;
}
