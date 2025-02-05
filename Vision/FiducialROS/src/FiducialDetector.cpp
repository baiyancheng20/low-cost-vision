//******************************************************************************
//
//                 Low Cost Vision
//
//******************************************************************************
// Project:        Fiducial
// File:           FiducialDetector.cpp
// Description:    Detects fiduciary markers
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

#include <FiducialDetector/FiducialDetector.h>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <iostream>
#include <math.h>

FiducialDetector::FiducialDetector(int minRad, int maxRad, int distance,
		int circleVotes, float minDist, float maxDist, int lineVotes,
		unsigned int maxLines, int lowThreshold, int highThreshold, int centerMethod) {
	this->verbose = false;
	this->minRad = minRad;
	this->maxRad = maxRad;
	this->distance = distance;
	this->circleVotes = circleVotes;
	this->minDist = minDist;
	this->maxDist = maxDist;
	this->lineVotes = lineVotes;
	this->maxLines = maxLines;
	this->lowThreshold = lowThreshold;
	this->highThreshold = highThreshold;
	this->lineVotes = lineVotes;
	this->centerMethod = centerMethod;
}

FiducialDetector::~FiducialDetector() {
}

inline void FiducialDetector::polarLine(cv::Mat& image, float rho, float theta,
		cv::Scalar color, int thickness) {
	if (theta < M_PI / 4. || theta > 3. * M_PI / 4.) { // ~vertical line
		// point of intersection of the line with first row
		cv::Point pt1(rho / cos(theta), 0);
		// point of intersection of the line with last row
		cv::Point pt2((rho - image.rows * sin(theta)) / cos(theta), image.rows);
		// draw the line
		cv::line(image, pt1, pt2, color, thickness);
	} else { // ~horizontal line
		// point of intersection of the line with first column
		cv::Point pt1(0, rho / sin(theta));
		// point of intersection of the line with last column
		cv::Point pt2(image.cols, (rho - image.cols * cos(theta)) / sin(theta));
		// draw the line
		cv::line(image, pt1, pt2, color, thickness);
	}
}

void FiducialDetector::detect(cv::Mat& image, std::vector<cv::Point2f>& points,
		fiducial::fiducial_feedback* feedback, cv::Mat* debugImage) {
	// Apply gaussian blur
	cv::Mat blur;
	cv::GaussianBlur(image, blur, cv::Size(3,3), 2.0);

	// Detect circles
	std::vector<cv::Vec3f> circles;
	cv::HoughCircles(blur, circles, CV_HOUGH_GRADIENT, 2, // accumulator resolution divisor
			distance, // minimum distance between circles
			highThreshold, // Canny high threshold
			circleVotes, // minimum number of votes
			minRad, maxRad); // min and max radius

	// Accurately detect the center for every circle with sub-pixel precision
	for (std::vector<cv::Vec3f>::const_iterator it = circles.begin();
			it != circles.end(); it++) {
		// Set ROI to the circle
		cv::Point center((*it)[0], (*it)[1]);
		float rad = (*it)[2];
		cv::Rect bounds(MAX(center.x - rad, 0), MAX(center.y - rad, 0),
				center.x + rad < image.cols ? rad * 2 : image.cols - center.x,
				center.y + rad < image.rows ? rad * 2 : image.rows - center.y);
		cv::Mat roi = image(bounds);

		// Generate inverted circle mask
		cv::Mat roiMask = cv::Mat::zeros(roi.rows, roi.cols, CV_8U);
		cv::Point roiCenter(roi.cols / 2, roi.rows / 2);
		cv::circle(roiMask, roiCenter, rad - 1, cv::Scalar(255), -1);

		// If lines were found
		cv::Point2f roiPoint;
		bool ret = false;
		if (debugImage != NULL) {
			cv::Mat roiDebug = (*debugImage)(bounds);
			ret = detectCrosshair(roi, roiPoint, roiMask, feedback, &roiDebug);
		} else {
			ret = detectCrosshair(roi, roiPoint, roiMask, feedback);
		}

		if (ret) {
			cv::Point2f point(bounds.x + roiPoint.x, bounds.y + roiPoint.y);
			if (bounds.contains(point))
				points.push_back(point);
			else {
				feedback->outsideROI++;
				if (verbose) std::cout << "Center: " << center << " outside ROI!" << std::endl;
			}
		}

		// Draw the detected circles
		if (debugImage != NULL)
			cv::circle(*debugImage, cv::Point((*it)[0], (*it)[1]), (*it)[2],
					cv::Scalar(0, 255, 0), 2);
	}
}

bool rhoComp(cv::Vec2f i,cv::Vec2f j) { return (i[0]<j[0]); }
inline cv::Vec2f medoidRho(std::vector<cv::Vec2f>::iterator first, std::vector<cv::Vec2f>::iterator last) {
	std::vector<cv::Vec2f>::iterator n = first+std::distance(first,last)/2;
	nth_element(first, n, last, rhoComp);
	return *n;
}
bool thetaComp(cv::Vec2f i,cv::Vec2f j) { return (i[1]<j[1]); }
inline cv::Vec2f medoidTheta(std::vector<cv::Vec2f>::iterator first, std::vector<cv::Vec2f>::iterator last) {
	std::vector<cv::Vec2f>::iterator n = first+std::distance(first,last)/2;
	nth_element(first, n, last, thetaComp);
	return *n;
}

bool FiducialDetector::detectCrosshair(cv::Mat& image, cv::Point2f& center,
		const cv::Mat& mask, fiducial::fiducial_feedback* feedback, cv::Mat* debugImage) {
	cv::Mat filtered;
	cv::bilateralFilter(image, filtered, 5, 50, 50);

	cv::Mat canny;
	cv::Canny(filtered, canny, lowThreshold, highThreshold);

	if (!mask.empty()) {
		cv::Mat invMask;
		cv::threshold(mask, invMask, 128, 255, CV_THRESH_BINARY_INV);
		canny.setTo(cv::Scalar(0), invMask);
	}

	// Hough tranform for line detection
	int votes = lineVotes;
	std::vector<cv::Vec2f> lines;
	std::vector<cv::Vec2f> newLines;
	do {
		cv::HoughLines(canny, newLines, 1, M_PI / 180.0, // step size
				votes); // minimum number of votes
		if(newLines.size() > maxLines) {
			lines = newLines;
			votes++;
		}
	} while(newLines.size() > maxLines);

	if (lines.empty())
		return false;

	// Segment perpendicular lines along the mean angle for center detection
	std::vector<cv::Vec2f> lines1;
	std::vector<cv::Vec2f> lines2;
	float refAngle = medoidTheta(lines.begin(), lines.end())[1];

	// Segment the lines
	for (std::vector<cv::Vec2f>::iterator it = lines.begin(); it != lines.end();
			it++) {
		float angle = (*it)[1];
		float dist = abs(refAngle - angle);
		if (dist < M_PI/8.0)
			lines1.push_back(*it);
		else
			lines2.push_back(*it);

		// Draw a blue line
		if (debugImage != NULL)
			polarLine(*debugImage, (*it)[0], (*it)[1], cv::Scalar(255, 0, 0),
					1);
	}

	// Find two lines through the cross center
	cv::Vec2f center1;
	cv::Vec2f center2;

	bool found1;
	bool found2;
	if(centerMethod == MEAN) {
		found1 = detectMeanCenterLine(center1, lines1, debugImage);
		found2 = detectMeanCenterLine(center2, lines2, debugImage);
	} else {
		found1 = detectMedoidCenterLine(center1, lines1, debugImage);
		found2 = detectMedoidCenterLine(center2, lines2, debugImage);
	}


	if(!found1) {
		if(verbose) std::cout << "Primary center line not found" << std::endl;
		feedback->tooFewLines++;
		return false;
	}
	if(!found2) {
		if(verbose) std::cout << "Secondary center line not found" << std::endl;
		feedback->tooFewLines++;
		return false;
	}

	// If both have lines on opposing sides we can intersect them and find the center
	if (found1 && found2) {
		float rho1 = center1[0];
		float rho2 = center2[0];
		float omega1 = center1[1];
		float omega2 = center2[1];

		if (rho1 != 0 && rho2 != 0) {
			float theta = atan(
					(cos(omega1) - (rho1 / rho2) * cos(omega2))
							/ -(sin(omega1) - (rho1 / rho2) * sin(omega2)));
			float r = rho1 / cos(theta - omega1);
			center = cv::Point2f(r * cos(theta), r * sin(theta));
		} else if (rho1 != 0) {
			// Line 2 is through the origin
			omega2 -= M_PI / 2.0;
			if (omega1 < M_PI / 4. || omega1 > 3. * M_PI / 4.) {
				float y = rho1 * (sin(omega2) / cos(omega1 - omega2));
				float x = (rho1 - y * sin(omega1)) / cos(omega1);
				center = cv::Point2f(x, y);
			} else { // ~horizontal line
				float x = rho1 * (cos(omega2) / cos(omega1 - omega2));
				float y = (rho1 - x * cos(omega1)) / sin(omega1);
				center = cv::Point2f(x, y);
			}
		} else if (rho2 != 0) {
			// Line 1 is through the origin
			omega1 -= M_PI / 2.0;
			if (omega2 < M_PI / 4. || omega2 > 3. * M_PI / 4.) {
				float y = rho2 * (sin(omega1) / cos(omega2 - omega1));
				float x = (rho2 - y * sin(omega2)) / cos(omega2);
				center = cv::Point2f(x, y);
			} else { // ~horizontal line
				float x = rho2 * (cos(omega1) / cos(omega2 - omega1));
				float y = (rho2 - x * cos(omega2)) / sin(omega2);
				center = cv::Point2f(x, y);
			}
		} else {
			// Both lines are through the origin
			center = cv::Point2f(0, 0);
		}

		if (debugImage != NULL)
			cv::circle(*debugImage, center, 1, cv::Scalar(0, 0, 255), 2);

		return true;
	}
	return false;
}

bool FiducialDetector::detectMedoidCenterLine(cv::Vec2f& centerLine, std::vector<cv::Vec2f> lines, cv::Mat* debugImage) {
	if(lines.size() < 2) {
		if(verbose) std::cout << "Not enough lines" << std::endl;
		return false;
	}

	// Select the first medoid line
	cv::Vec2f line1;
	if(centerMethod == MEDOID_RHO) line1 = medoidRho(lines.begin(), lines.end());
	else line1 = medoidTheta(lines.begin(), lines.end());

	// Ignore all lines too close or too far from the first line
	std::vector<cv::Vec2f>::iterator first = lines.begin();
	std::vector<cv::Vec2f>::iterator last = lines.begin();
	for(std::vector<cv::Vec2f>::iterator it=lines.begin(); it!=lines.end(); ++it) {
		float dist = abs(line1[0] - (*it)[0]);
		if(minDist <= dist && dist <= maxDist) *last++ = *it;
	}

	if(lines.empty()) {
		if(verbose) std::cout << "Second line not found" << std::endl;
		return false;
	}

	// Select the second medoid line
	cv::Vec2f line2;
	if(centerMethod == MEDOID_RHO) line2 = medoidRho(first, last);
	else line2 = medoidTheta(first, last);

	// Determine the line between the two selected lines
	float rho = (line1[0] + line2[0]) / 2.0;
	float theta = (line1[1] + line2[1]) / 2.0;
	centerLine = cv::Vec2f(rho, theta);

	// Draw a red line on the debug image
	if (debugImage != NULL) polarLine(*debugImage, rho, theta, cv::Scalar(0, 0, 255), 1);

	return true;
}

bool FiducialDetector::detectMeanCenterLine(cv::Vec2f& centerLine, std::vector<cv::Vec2f> lines, cv::Mat* debugImage) {
	bool found1 = false;
	bool found2 = false;

	// If the are more than two lines we can detect the line through the center
	if(lines.size() < 2) {
		if(verbose) std::cout << "Not enough lines" << std::endl;
		return false;
	}

	// Select two lines representing the sides of the cross
	cv::Vec2f line1;
	cv::Vec2f line2;

	// Calculate the average angle
	float meanTheta = 0;
	for (std::vector<cv::Vec2f>::iterator it = lines.begin();
					it != lines.end(); it++)
			meanTheta += (*it)[1];
	meanTheta = meanTheta / lines.size();

	// Select the first line with an angle closest to the average
	float lastTheta = M_PI;
	for (std::vector<cv::Vec2f>::iterator it = lines.begin();
					it != lines.end(); it++) {
			// Check if this line has a better angle than the last
			float thetaDist = abs(meanTheta - (*it)[1]);
			if (thetaDist < lastTheta) {
					// Set the first line
					line1 = *it;
					lastTheta = thetaDist;
					found1 = true;
			}
	}

	if (!found1) {
		if(verbose) std::cout << "First line not found" << std::endl;
		return false;
	}

	// Ignore all lines too close or too far from the first line
	std::vector<cv::Vec2f>::iterator first = lines.begin();
	std::vector<cv::Vec2f>::iterator last = lines.begin();
	for(std::vector<cv::Vec2f>::iterator it=lines.begin(); it!=lines.end(); ++it) {
		float dist = abs(line1[0] - (*it)[0]);
		if(minDist <= dist && dist <= maxDist) *last++ = *it;
	}

	// Select the second line with an angle closest to the first line
	lastTheta = M_PI;
	for (std::vector<cv::Vec2f>::iterator it = first;
					it != last; it++) {
			// Check if this line has a better angle than the last
			float thetaDist = abs(line1[1] - (*it)[1]);
			if (thetaDist < lastTheta) {
				// Set the second line
				line2 = *it;
				lastTheta = thetaDist;
				found2 = true;
			}
	}

	if (!found2) {
		if(verbose) std::cout << "Second line not found" << std::endl;
		return false;
	}

	// Determine the line between the two selected lines
	float rho = (line1[0] + line2[0]) / 2.0;
	float theta = (line1[1] + line2[1]) / 2.0;
	centerLine = cv::Vec2f(rho, theta);

	// Draw a red line on the debug image
	if (debugImage != NULL) polarLine(*debugImage, rho, theta, cv::Scalar(0, 0, 255), 1);

	return true;
}
