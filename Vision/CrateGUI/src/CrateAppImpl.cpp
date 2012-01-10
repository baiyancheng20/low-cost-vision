//******************************************************************************
//
//                 Low Cost Vision
//
//******************************************************************************
// Project:        CrateGUI
// File:           CrateAppImpl.cpp
// Description:    The functions not generated by the GUI
// Author:         Glenn Meerstra
// Notes:          ...
//
// License:        GNU GPL v3
//
// This file is part of CrateGUI.
//
// CrateGUI is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// CrateGUI is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with CrateGUI.  If not, see <http://www.gnu.org/licenses/>.
//******************************************************************************
#include "CrateGUI/CrateAppImpl.h"
#include "CrateGUI/CrateApp.h"

#include "DetectQRCode/BarcodeDetector.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <math.h>

#include <wx/pen.h>
#include <wx/colour.h>
#include <wx/dcclient.h>

#ifdef __CDT_PARSER__
#define foreach(a, b) for(a : b)
#else
#define foreach(a, b) BOOST_FOREACH(a, b)
#endif

void CrateAppImpl::setCurrentImagePath(const wxString& path) {
	currentImagePath = path;
	image = wxImage(path);

	wxSize imageMaxSize = this->GetSize();
	imageMaxSize.SetWidth(
			imageMaxSize.GetWidth() - bSizer121->GetSize().GetWidth());
	imageMaxSize.SetHeight(
			imageMaxSize.GetHeight() - MessageLabel->GetSize().GetHeight()
					- 20);

	double heightScale = (double) image.GetHeight()
			/ (double) imageMaxSize.GetHeight();
	double widthScale = (double) image.GetWidth() / (double) imageMaxSize.GetWidth();

	if(widthScale < heightScale){
		Scale = heightScale;
	}else{
		Scale = widthScale;
	}

	UpdateImageField();
	this->Layout();
}

void CrateAppImpl::getPathFromChosenDir(boost::filesystem::path& path) {

	std::stringstream s;
	while (path.leaf() != dirPath.leaf()) {
		std::stringstream ss(s.str().c_str());
		s.str("");
		s << "/" << path.leaf() << ss.str().c_str();
		path = path.parent_path();
	}
	path = s.str().c_str();

}

void CrateAppImpl::UpdateImageField() {
	if (!zoom) {
		wxImage copy = image.Copy();
		copy.Rescale(image.GetWidth() / Scale, image.GetHeight() / Scale);
		ImageField->SetBitmap(copy);
	} else {
		wxImage copy = zoomImage.Copy();
		copy.Rescale(zoomImage.GetWidth() / LargestScale, zoomImage.GetHeight() / LargestScale);
		ImageField->SetBitmap(copy);
	}
	ImageField->Refresh();
}

void CrateAppImpl::calculateFiducialPoints(){

	double distance = sqrt(pow(double(QRCorner.x - OppositeCorner.x), 2) + pow(double(QRCorner.y - OppositeCorner.y), 2));
	double angle = atan2(double(QRCorner.y - OppositeCorner.y), double(OppositeCorner.x - QRCorner.x));
	center = wxPoint2DDouble(QRCorner.x + (distance / 2.0) * cos(-angle), QRCorner.y + (distance / 2.0) * sin(-angle));

	if(ObjectTypeCombo->GetValue() == wxT("Crate")){
		fid1 = wxPoint2DDouble(center.m_x + (FID_OFFSET*(distance/LINE_LENGTH)) * cos(-angle-M_PI/2.0),
						center.m_y + (FID_OFFSET*(distance/LINE_LENGTH)) * sin(-angle-M_PI/2.0));
		fid2 = wxPoint2DDouble(center.m_x + (FID_OFFSET*(distance/LINE_LENGTH)) * cos(-angle),
				center.m_y + (FID_OFFSET*(distance/LINE_LENGTH)) * sin(-angle));
		fid3 = wxPoint2DDouble(center.m_x + (FID_OFFSET*(distance/LINE_LENGTH)) * cos(-angle+M_PI/2.0),
				center.m_y + (FID_OFFSET*(distance/LINE_LENGTH)) * sin(-angle+M_PI/2.0));
	}else if(ObjectTypeCombo->GetValue() == wxT("QR code")){
		float QRCodeLineLength = sqrt(pow(QR_CODE_SIDE, 2) * 2.0);
		float MarkerOffset = QRCodeLineLength/2.0;// - sqrt(pow(QR_MARKER_SIDE / 2.0 , 2) * 2.0);
		fid1 = wxPoint2DDouble(center.m_x + (MarkerOffset*(distance/QRCodeLineLength)) * cos(-angle-M_PI/2.0),
						center.m_y + (MarkerOffset*(distance/QRCodeLineLength)) * sin(-angle-M_PI/2.0));
		fid2 = wxPoint2DDouble(center.m_x + (MarkerOffset*(distance/QRCodeLineLength)) * cos(-angle),
				center.m_y + (MarkerOffset*(distance/QRCodeLineLength)) * sin(-angle));
		fid3 = wxPoint2DDouble(center.m_x + (MarkerOffset*(distance/QRCodeLineLength)) * cos(-angle+M_PI/2.0),
				center.m_y + (MarkerOffset*(distance/QRCodeLineLength)) * sin(-angle+M_PI/2.0));
	}

	if(!mousePressedInImageField){
		std::string code;
		if((code = getBarcode(distance, angle)) != ""){
			QRCodeTextBox->SetValue(wxString(code.c_str(), wxConvLocal));
		}
	}
}

std::string CrateAppImpl::getBarcode(double distance, double angle){
	std::string result = "";

	if(distance != 0){
		double actualCenterX = center.m_x;
		double actualCenterY = center.m_y;
		double crateSideLength = sqrt((distance*distance) / 2.0);

		if(zoom){
			actualCenterX = actualCenterX * LargestScale;
			actualCenterY = actualCenterY * LargestScale;
			crateSideLength = crateSideLength * LargestScale;
			actualCenterX += zoomX* Scale;
			actualCenterY += zoomY* Scale;
		}else{
			actualCenterX *= Scale;
			actualCenterY *= Scale;
			crateSideLength *= Scale;
		}

		if(crateSideLength > 0){
			cv::RotatedRect rect(cv::Point2f(actualCenterX, actualCenterY), cv::Size2f(crateSideLength, crateSideLength), angle-M_PI/4.0);

			// Draw rect
			std::vector<cv::Point2f> points;
			points.push_back(cv::Point2f(
					rect.center.x + (rect.size.width / 2.0) * cos(-rect.angle)
							- (rect.size.height / 2.0) * sin(-rect.angle),
					rect.center.y + (rect.size.height / 2.0) * cos(-rect.angle)
							+ (rect.size.width / 2.0) * sin(-rect.angle)));
			points.push_back(cv::Point2f(
					rect.center.x - (rect.size.width / 2.0) * cos(-rect.angle)
							- (rect.size.height / 2.0) * sin(-rect.angle),
					rect.center.y + (rect.size.height / 2.0) * cos(-rect.angle)
							- (rect.size.width / 2.0) * sin(-rect.angle)));
			points.push_back(cv::Point2f(
					rect.center.x - (rect.size.width / 2.0) * cos(-rect.angle)
							+ (rect.size.height / 2.0) * sin(-rect.angle),
					rect.center.y - (rect.size.height / 2.0) * cos(-rect.angle)
							- (rect.size.width / 2.0) * sin(-rect.angle)));
			points.push_back(cv::Point2f(
					rect.center.x + (rect.size.width / 2.0) * cos(-rect.angle)
							+ (rect.size.height / 2.0) * sin(-rect.angle),
					rect.center.y - (rect.size.height / 2.0) * cos(-rect.angle)
							+ (rect.size.width / 2.0) * sin(-rect.angle)));

			cv::Rect bounds = cv::boundingRect(points);
			if(bounds.x < 0) bounds.x = 0;
			if(bounds.y < 0) bounds.y = 0;
			if(bounds.x + bounds.width > image.GetWidth()) bounds.width = image.GetWidth() - bounds.x;
			if(bounds.y + bounds.height > image.GetHeight()) bounds.height = image.GetHeight() - bounds.y;

			cv::Mat barcode = (cv::imread((std::string)currentImagePath.ToAscii()))(bounds);
			DetectBarcode detector;
			detector.detect(barcode, result);
		}
	}
	return result;
}

void CrateAppImpl::drawCrateAttributes(){
	UpdateImageField();
	ImageField->Update();

	int color = ColorSlider->GetValue();

	if(QRCorner != wxPoint(0, 0) &&
			OppositeCorner != wxPoint(0, 0)){
		if(ObjectTypeCombo->GetValue() == wxT("Crate")){
			wxPaintDC dc(ImageField);
			dc.SetPen(wxPen(wxColour(color, color, color), 2, wxSOLID));
			dc.SetBrush(wxBrush(wxColour(0, 0, 0), wxTRANSPARENT));
			dc.DrawLine(QRCorner + wxPoint(coordinateOffset, coordinateOffset), OppositeCorner + wxPoint(coordinateOffset, coordinateOffset));

			double distance = sqrt(pow(double(QRCorner.x - OppositeCorner.x), 2) + pow(double(QRCorner.y - OppositeCorner.y), 2));
			double angle = atan2(double(QRCorner.y - OppositeCorner.y), double(OppositeCorner.x - QRCorner.x));
			double fidRadius = FID_RADIUS*(distance/LINE_LENGTH);

			dc.SetPen(wxPen(wxColour(255-color, color, color), 2, wxSOLID));
			dc.DrawCircle(fid1.m_x + coordinateOffset, fid1.m_y + coordinateOffset, fidRadius);
			dc.DrawLine(fid1.m_x + coordinateOffset + fidRadius * cos(-angle-M_PI/4.0),
					fid1.m_y + coordinateOffset + fidRadius * sin(-angle-M_PI/4.0),
					fid1.m_x + coordinateOffset - fidRadius * cos(-angle-M_PI/4.0),
					fid1.m_y + coordinateOffset - fidRadius * sin(-angle-M_PI/4.0));
			dc.DrawLine(fid1.m_x + coordinateOffset + fidRadius * cos(-angle+M_PI/4.0),
					fid1.m_y + coordinateOffset + fidRadius * sin(-angle+M_PI/4.0),
					fid1.m_x + coordinateOffset - fidRadius * cos(-angle+M_PI/4.0),
					fid1.m_y + coordinateOffset - fidRadius * sin(-angle+M_PI/4.0));

			dc.SetPen(wxPen(wxColour(color, 255-color, color), 2, wxSOLID));
			dc.DrawCircle(fid2.m_x + coordinateOffset, fid2.m_y + coordinateOffset, fidRadius);
			dc.DrawLine(fid2.m_x + coordinateOffset + fidRadius * cos(-angle-M_PI/4.0),
					fid2.m_y + coordinateOffset + fidRadius * sin(-angle-M_PI/4.0),
					fid2.m_x + coordinateOffset - fidRadius * cos(-angle-M_PI/4.0),
					fid2.m_y + coordinateOffset - fidRadius * sin(-angle-M_PI/4.0));
			dc.DrawLine(fid2.m_x + coordinateOffset + fidRadius * cos(-angle+M_PI/4.0),
					fid2.m_y + coordinateOffset + fidRadius * sin(-angle+M_PI/4.0),
					fid2.m_x + coordinateOffset - fidRadius * cos(-angle+M_PI/4.0),
					fid2.m_y + coordinateOffset - fidRadius * sin(-angle+M_PI/4.0));

			dc.SetPen(wxPen(wxColour(color, color, 255-color), 2, wxSOLID));
			dc.DrawCircle(fid3.m_x + coordinateOffset, fid3.m_y + coordinateOffset, fidRadius);
			dc.DrawLine(fid3.m_x + coordinateOffset + fidRadius * cos(-angle-M_PI/4.0),
					fid3.m_y + coordinateOffset + fidRadius * sin(-angle-M_PI/4.0),
					fid3.m_x + coordinateOffset - fidRadius * cos(-angle-M_PI/4.0),
					fid3.m_y + coordinateOffset - fidRadius * sin(-angle-M_PI/4.0));
			dc.DrawLine(fid3.m_x + coordinateOffset + fidRadius * cos(-angle+M_PI/4.0),
					fid3.m_y + coordinateOffset + fidRadius * sin(-angle+M_PI/4.0),
					fid3.m_x + coordinateOffset - fidRadius * cos(-angle+M_PI/4.0),
					fid3.m_y + coordinateOffset - fidRadius * sin(-angle+M_PI/4.0));

		}else if(ObjectTypeCombo->GetValue() == wxT("Marker")){
			wxPaintDC dc(ImageField);
			dc.SetPen(wxPen(wxColour(color, color, color), 2, wxSOLID));
			dc.SetBrush(wxBrush(wxColour(0, 0, 0), wxTRANSPARENT));dc.DrawLine(QRCorner + wxPoint(coordinateOffset, coordinateOffset), OppositeCorner + wxPoint(coordinateOffset, coordinateOffset));

			double distance = sqrt(pow(double(QRCorner.x - OppositeCorner.x), 2) + pow(double(QRCorner.y - OppositeCorner.y), 2));
			double angle = atan2(double(QRCorner.y - OppositeCorner.y), double(OppositeCorner.x - QRCorner.x));

			dc.SetPen(wxPen(wxColour(255-color, color, color), 2, wxSOLID));
			dc.DrawCircle(QRCorner.x + coordinateOffset, QRCorner.y + coordinateOffset, distance);
			dc.DrawLine(QRCorner.x + coordinateOffset + distance * cos(-angle-M_PI/4.0),
					QRCorner.y + coordinateOffset + distance * sin(-angle-M_PI/4.0),
					QRCorner.x + coordinateOffset - distance * cos(-angle-M_PI/4.0),
					QRCorner.y + coordinateOffset - distance * sin(-angle-M_PI/4.0));
			dc.DrawLine(QRCorner.x + coordinateOffset + distance * cos(-angle+M_PI/4.0),
					QRCorner.y + coordinateOffset + distance * sin(-angle+M_PI/4.0),
					QRCorner.x + coordinateOffset - distance * cos(-angle+M_PI/4.0),
					QRCorner.y + coordinateOffset - distance * sin(-angle+M_PI/4.0));

		}else if(ObjectTypeCombo->GetValue() == wxT("QR code")){
			wxPaintDC dc(ImageField);
			dc.SetPen(wxPen(wxColour(color, color, color), 2, wxSOLID));
			dc.SetBrush(wxBrush(wxColour(0, 0, 0), wxTRANSPARENT));
			dc.DrawLine(QRCorner + wxPoint(coordinateOffset, coordinateOffset), OppositeCorner + wxPoint(coordinateOffset, coordinateOffset));

			double distance = sqrt(pow(double(QRCorner.x - OppositeCorner.x), 2) + pow(double(QRCorner.y - OppositeCorner.y), 2));
			double angle = atan2(double(QRCorner.y - OppositeCorner.y), double(OppositeCorner.x - QRCorner.x));

			float QRCodeLineLength = sqrt(pow(QR_CODE_SIDE, 2) * 2);
			float length_marker = (distance / QRCodeLineLength ) * QR_MARKER_SIDE;

			dc.SetPen(wxPen(wxColour(255-color, color, color), 2, wxSOLID));
			dc.SetBrush(wxBrush(wxColour(0, 0, 0), wxTRANSPARENT));

			// Translate the outer points back to the centers
			float offset = sqrt(pow(QR_MARKER_SIDE / 2.0 , 2) * 2.0) * (distance/QRCodeLineLength);
			wxPoint2DDouble f1(fid1.m_x + coordinateOffset - offset * sin(-angle), fid1.m_y + coordinateOffset + offset * cos(-angle));
			wxPoint2DDouble f2(fid2.m_x + coordinateOffset - offset * cos(-angle), fid2.m_y + coordinateOffset - offset * sin(-angle));
			wxPoint2DDouble f3(fid3.m_x + coordinateOffset + offset * sin(-angle), fid3.m_y + coordinateOffset - offset * cos(-angle));

			angle += (M_PI/4.0);
			wxPoint pt1(f2.m_x + (length_marker / 2.0) * cos(-angle) - (length_marker / 2.0) * sin(-angle),
					f2.m_y + (length_marker / 2.0) * cos(-angle) + (length_marker / 2.0) * sin(-angle));
			wxPoint pt2(f2.m_x - (length_marker / 2.0) * cos(-angle) - (length_marker / 2.0) * sin(-angle),
					f2.m_y + (length_marker / 2.0) * cos(-angle) - (length_marker / 2.0) * sin(-angle));
			wxPoint pt3(f2.m_x - (length_marker / 2.0) * cos(-angle) + (length_marker / 2.0) * sin(-angle),
					f2.m_y - (length_marker / 2.0) * cos(-angle) - (length_marker / 2.0) * sin(-angle));
			wxPoint pt4(f2.m_x + (length_marker / 2.0) * cos(-angle) + (length_marker / 2.0) * sin(-angle),
					f2.m_y - (length_marker / 2.0) * cos(-angle) + (length_marker / 2.0) * sin(-angle));

			dc.DrawLine(pt1, pt2);
			dc.DrawLine(pt2, pt3);
			dc.DrawLine(pt3, pt4);
			dc.DrawLine(pt4, pt1);
			dc.DrawLine(pt3, pt1);
			dc.DrawLine(pt4, pt2);

			pt1 = wxPoint(f1.m_x + (length_marker / 2.0) * cos(-angle) - (length_marker / 2.0) * sin(-angle),
					f1.m_y + (length_marker / 2.0) * cos(-angle) + (length_marker / 2.0) * sin(-angle));
			pt2 = wxPoint(f1.m_x - (length_marker / 2.0) * cos(-angle) - (length_marker / 2.0) * sin(-angle),
					f1.m_y + (length_marker / 2.0) * cos(-angle) - (length_marker / 2.0) * sin(-angle));
			pt3 = wxPoint(f1.m_x - (length_marker / 2.0) * cos(-angle) + (length_marker / 2.0) * sin(-angle),
					f1.m_y - (length_marker / 2.0) * cos(-angle) - (length_marker / 2.0) * sin(-angle));
			pt4 = wxPoint(f1.m_x + (length_marker / 2.0) * cos(-angle) + (length_marker / 2.0) * sin(-angle),
					f1.m_y - (length_marker / 2.0) * cos(-angle) + (length_marker / 2.0) * sin(-angle));

			dc.DrawLine(pt1, pt2);
			dc.DrawLine(pt2, pt3);
			dc.DrawLine(pt3, pt4);
			dc.DrawLine(pt4, pt1);
			dc.DrawLine(pt3, pt1);
			dc.DrawLine(pt4, pt2);

			pt1 = wxPoint(f3.m_x + (length_marker / 2.0) * cos(-angle) - (length_marker / 2.0) * sin(-angle),
					f3.m_y + (length_marker / 2.0) * cos(-angle) + (length_marker / 2.0) * sin(-angle));
			pt2 = wxPoint(f3.m_x - (length_marker / 2.0) * cos(-angle) - (length_marker / 2.0) * sin(-angle),
					f3.m_y + (length_marker / 2.0) * cos(-angle) - (length_marker / 2.0) * sin(-angle));
			pt3 = wxPoint(f3.m_x - (length_marker / 2.0) * cos(-angle) + (length_marker / 2.0) * sin(-angle),
					f3.m_y - (length_marker / 2.0) * cos(-angle) - (length_marker / 2.0) * sin(-angle));
			pt4 = wxPoint(f3.m_x + (length_marker / 2.0) * cos(-angle) + (length_marker / 2.0) * sin(-angle),
					f3.m_y - (length_marker / 2.0) * cos(-angle) + (length_marker / 2.0) * sin(-angle));

			dc.DrawLine(pt1, pt2);
			dc.DrawLine(pt2, pt3);
			dc.DrawLine(pt3, pt4);
			dc.DrawLine(pt4, pt1);
			dc.DrawLine(pt3, pt1);
			dc.DrawLine(pt4, pt2);
		}
	}

	if(zoomWidth != 0 && zoomHeight != 0 &&
			zoomX != 0 && zoomY != 0 &&
			ZoomCheckBox->GetValue() && !zoom){
		wxPaintDC dc(ImageField);
		dc.SetPen(wxPen(wxColour(color, color, 255-color), 2, wxSHORT_DASH));
		dc.SetBrush(wxBrush(wxColour(0, 0, 0), wxTRANSPARENT));
		dc.DrawRectangle(wxPoint(zoomX+coordinateOffset, zoomY+coordinateOffset), wxSize(zoomWidth, zoomHeight));
	}
}

void CrateAppImpl::NextImage() {
	currentObjectNumber = 0;

	imagePathsIt++;
	if (imagePathsIt < imagePaths.end()) {

		MessageLabel->SetLabel(wxT("Next Image"));
		if (XMLOption == Edit) {
			//Disables/enables the skip button
			SkipButton->Enable(false);
			foreach( boost::property_tree::ptree::value_type& tempValue, pt.get_child("metadata") )
			{

				boost::filesystem::path temp = tempValue.second.get("<xmlattr>.path", "");
				if (temp.leaf() == (*imagePathsIt).leaf()) {

					int objectCount = 0;
					foreach( boost::property_tree::ptree::value_type& imageCategory,
					tempValue.second.get_child(tempValue.second.data()) )
					{
						if (imageCategory.first == "category") {

							const char* s =
									(imageCategory.second.get(
											"<xmlattr>.value",
											"")).c_str();
							if (imageCategory.second.get(
									"<xmlattr>.name", "")
									== "background") {
								BackgroundComboBox->SetValue(
										wxString(s,
												wxConvLocal));
							} else if (imageCategory.second.get(
									"<xmlattr>.name", "")
									== "light") {
								LightingComboBox->SetValue(
										wxString(s,
												wxConvLocal));
							} else if (imageCategory.second.get(
									"<xmlattr>.name", "")
									== "perspective") {
								PerspectiveComboBox->SetValue(
										wxString(s,
												wxConvLocal));
							}

						} else if (imageCategory.first
								== "object") {
							objectCount++;
						}
					}
					std::stringstream s;
					s << objectCount;
					const wxString temp = wxString(s.str().c_str(),
							wxConvLocal);

					MessageLabel->SetLabel(
							wxT("Image is already available within the xml file, press skip button to go to next image."));
					SkipButton->Enable(true);

					break;
				}
			}
		}

		std::stringstream globalfile;
		globalfile << (*imagePathsIt).parent_path() << "/global.xml";
		if(boost::filesystem::exists(globalfile.str().c_str()) && boost::filesystem::is_regular_file(globalfile.str().c_str())){
			boost::property_tree::ptree temp;
			boost::property_tree::read_xml(globalfile.str().c_str(), temp);
			foreach( boost::property_tree::ptree::value_type& values, temp.get_child("global_values") )
			{
				if (values.first == "category") {

					const char* s = (values.second.get("<xmlattr>.value", "")).c_str();

					if (values.second.get("<xmlattr>.name", "")== "background") {
						BackgroundComboBox->SetValue( wxString(s, wxConvLocal));
					} else if (values.second.get( "<xmlattr>.name", "") == "light") {
						LightingComboBox->SetValue( wxString(s, wxConvLocal));
					} else if (values.second.get( "<xmlattr>.name", "")	== "perspective") {
						PerspectiveComboBox->SetValue( wxString(s, wxConvLocal));
					} else if (values.second.get( "<xmlattr>.name", "")	== "qrcode") {
						QRCodeTextBox->SetValue( wxString(s, wxConvLocal));
					}
				}
			}
		}

		std::stringstream s;
		s << *imagePathsIt;
		wxString wxStringTemp((s.str().c_str()), wxConvLocal);
		setCurrentImagePath(wxStringTemp);
	} else {


		ZoomButton->Enable(false);
		SkipButton->Enable(false);
		ImageField->Enable(false);
		ResetButton->Enable(false);
		ColorSlider->Enable(false);
		CrateLineRDB->Enable(false);
		ZoomCheckBox->Enable(false);
		QRCodeTextBox->Enable(false);
		m_staticText11->Enable(false);
		m_staticText29->Enable(false);
		QRCodeCornerRDB->Enable(false);
		ObjectTypeCombo->Enable(false);
		ObjectTypeCombo->Enable(false);
		NextImageButton->Enable(false);
		LightingComboBox->Enable(false);
		NextObjectButton->Enable(false);
		ZoomBox_radioBtn->Enable(false);
		QRCodeCornerLabel->Enable(false);
		OppositeCornerRDB->Enable(false);
		BackgroundComboBox->Enable(false);
		OppositeCornerLabel->Enable(false);
		PerspectiveComboBox->Enable(false);
		OriginalImageButton->Enable(false);

		MessageLabel->SetLabel(wxT("All images handled, press done to finish"));
	}
}

void CrateAppImpl::Start(){
	if (XMLOption == NewXML) {
		pt.add("metadata", "");
	}else{
		if (!boost::filesystem::is_regular_file(xmlPath.string()) || !boost::filesystem::exists(xmlPath.string())) {
			this->Close(true);
			return;
		}

		boost::property_tree::read_xml(xmlPath.string().c_str(), pt);
	}

	imagePathsIt = imagePaths.begin() -1;
	NextImage();
}

CrateAppImpl::CrateAppImpl(wxWindow* parent, wxWindowID id,
		const wxString& title,
		const wxPoint& pos,
		const wxSize& size,
		long style) :
		CrateApp(parent, id, title, pos, size, style){

	wxImage::AddHandler(new wxJPEGHandler);
	wxImage::AddHandler(new wxPNGHandler);

	CrateLineRDB->SetValue(true);

	Scale = 0;
	LargestScale = 1;

	QRCorner = wxPoint(0, 0);
	OppositeCorner = wxPoint(0, 0);

	zoom = false;
	zoomX = 0;
	zoomY = 0;
	zoomWidth = 0;
	zoomHeight = 0;
	coordinateOffset = 5;

	mousePressedInImageField = false;

	OriginalImageButton->Enable(false);
	SkipButton->Enable(false);

    QRCodeCornerLabel->SetLabel(wxString(("(0.00,0.00)"), wxConvLocal));
    OppositeCornerLabel->SetLabel(wxString(("(0.00,0.00)"), wxConvLocal));
    QRCodeCornerRDB->SetLabel(wxString(("QR code"), wxConvLocal));
    OppositeCornerRDB->SetLabel(wxString(("Opposite"), wxConvLocal));
}

CrateAppImpl::~CrateAppImpl(){
	this->GetParent()->Close(true);
}
