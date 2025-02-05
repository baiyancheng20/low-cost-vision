//******************************************************************************
//
//                 Low Cost Vision
//
//******************************************************************************
// Project:        VisionNode
// File:           visionNode.h
// Description:    This vision RosNode detects the crates and publishes events on the crateEvent topic.
// Author:         Kasper van Nieuwland en Zep Mouris
// Notes:          ...
//
// License:        GNU GPL v3
//
// This file is part of VisionNode.
//
// VisionNode is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// VisionNode is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with VisionNode.  If not, see <http://www.gnu.org/licenses/>.
//******************************************************************************
#pragma once
#include <pcrctransformation/pcrctransformer.hpp>
#include <pcrctransformation/point2f.hpp>
#include <CameraCalibration/RectifyImage.h>
#include <unicap_cv_bridge.hpp>
#include <FiducialDetector.h>
#include <QRCodeDetector.h>
#include <Crate.h>
#include <vision/CrateTracker.h>
#include "ros/ros.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <sstream>

#include <vision/CrateEventMsg.h>
#include <vision/error.h>
#include <vision/getCrate.h>
#include <vision/getAllCrates.h>
#include <std_srvs/Empty.h>

class visionNode{
public:

	/**
	 * the constructor
	 * @param argc use the parameters form main
	 * @param argv use the parameters form main
	 */
	visionNode(int argc, char* argv[]);
	/**
	 * the destructor
	 */
	~visionNode();

	/**
	 * blocking function that contains the main loop: take frame, detect crates, send event. this function ends when ros receives a ^c
	 */
	void run();

	/**
	 * callback function for the getCrate services of ROS
	 * @param req the request object
	 * @param res the response object
	 * @return
	 */
	bool getCrate(vision::getCrate::Request &req,vision::getCrate::Response &res);
	/**
	 * callback function for the getAllCrates services of ROS
	 * @param req the request object
	 * @param res the response object
	 * @return
	 */
	bool getAllCrates(vision::getAllCrates::Request &req,vision::getAllCrates::Response &res);
	/**
	 * callback function for the recalibrate services of ROS
	 * @param req the request object
	 * @param res the response object
	 * @return
	 */
	bool recalibrate(std_srvs::Empty &req, std_srvs::Empty &res);

private:
	unicap_cv_bridge::unicap_cv_camera * cam;
	FiducialDetector * fidDetector;
	QRCodeDetector * qrDetector;
	pcrctransformation::pc_rc_transformer * cordTransformer;
	RectifyImage * rectifier;
	CrateTracker * crateTracker;
	pcrctransformation::point2f::point2fvector markers;

	cv::Mat camFrame;
	cv::Mat rectifiedCamFrame;

	ros::NodeHandle node;
	ros::Publisher crateEventPublisher;
	ros::Publisher ErrorPublisher;
	ros::ServiceServer getCrateService;
	ros::ServiceServer getAllCratesService;

	double crateMovementThresshold;
	int numberOfStableFrames;
	bool invokeCalibration;

	bool calibrate(unsigned int measurements = 100, int maxErrors = 100);
	void printUsage(char* invokeName);
};
