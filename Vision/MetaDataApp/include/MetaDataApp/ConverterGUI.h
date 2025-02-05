//******************************************************************************
//
//                 Low Cost Vision
//
//******************************************************************************
// Project:        MetaDataApp
// File:           ConverterGUI.h
// Description:    The implementation of the functions in the GUI
// Author:         Glenn Meerstra
// Notes:          ...
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
#ifndef __ConverterGUI__
#define __ConverterGUI__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/combobox.h>
#include <wx/radiobut.h>
#include <wx/statline.h>
#include <wx/checkbox.h>
#include <wx/filepicker.h>
#include <wx/slider.h>
#include <wx/frame.h>
#include <wx/scrolwin.h>
#include <wx/dc.h>
#include <wx/dcclient.h>

#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "CrateGUI.h"

/*
 using namespace std;
 using namespace boost::filesystem;
 */

typedef enum XMLOption {
	Add, Edit, NewXML
} XMLOption;

/**
 * @brief This class is made for an GUI
 * @details With this GUI we can create meta data for images in a selected directory by doing the options within the GUI
 * @author Glenn
 * @version 2.0
 * @date 10-2011
 */
class GUIFrame: public wxFrame {

private:
	///@brief a constant for the width of the image
	static const int imageWidth = 640;
	///@brief a constant for the heigth of the image
	static const int imageHeight = 480;
	///@brief this becomes our xml with all the nodes
	boost::property_tree::ptree pt;
	boost::property_tree::ptree *tempValue;

	CrateGUI* crateGUI;

	///@brief The top left X coordinate for the box for surrounding the object
	int x;
	///@brief The top left Y coordinate for the box for surrounding the object
	int y;
	///@brief The width for the box for surrounding the object
	int width;
	///@brief The height for the box for surrounding the object
	int height;
	///@brief The top left X coordinate for the box for surrounding the object
	int zoomX;
	///@brief The top left Y coordinate for the box for surrounding the object
	int zoomY;
	///@brief The width for the box for surrounding the object
	int zoomWidth;
	///@brief The height for the box for surrounding the object
	int zoomHeight;
	///@brief The top X coordinate for the center line for the object
	int TopX;
	///@brief The top Y coordinate for the center line for the object
	int TopY;
	///@brief The bottom X coordinate for the center line for the object
	int BottomX;
	///@brief The bottom Y coordinate for the center line for the object
	int BottomY;
	///@brief An images in a xml file can be added, edit and a new one can be created
	int editXML;
	///@brief A variable for the current object number
	unsigned int currentObjectNr;
	///@brief The total amount of objects in the current image specified by the user
	unsigned long int AmountOfObjects;
	///@brief A variable for the rotation for the current object
	double rotation;
	///@brief A variable for the height scale at which the image is resized
	double heightScale;
	///@brief A variable for the width scale at which the image is resized
	double widthScale;
	///@brief A variable for keeping track of the left mouse button is pressed
	bool mousePressedInImage;
	///@brief A variable for the name of the current object
	char* objectName;
	///@brief is an variable for which is set if the image is available in the xml
	bool AlreadyInXML;
	bool noMarkerValues;
	bool zoom;

	///@brief A variable for the current image
	wxImage image;
	wxImage zoomImage;
	///@brief A variable for stepping through all the images
	boost::filesystem::directory_iterator imgIt;
	///@brief A variable for the path which you selected
	boost::filesystem::path dirPath;
	///@brief A vector variable for each image path in dirPath/Images
	std::vector<boost::filesystem::path> imagePaths;
	///@brief A variable for stepping through all the images
	std::vector<boost::filesystem::path>::iterator imagePathsIt;
	///@brief
	wxString currentImagePath;
	boost::filesystem::path XMLPath;

	/**
	 * @fn UpdateImageField()
	 * @brief Updates the image in the image field with the image viraible
	 */
	void UpdateImageField();
	/**
	 * @fn DrawOnImageAccordingToRadioButtonOption(int eventX, int eventY)
	 * @brief Draws a box or line depending on the radio button option (Surround box and Center line)
	 * @param eventX the x from the click event
	 * @param eventY the y from the click event
	 */
	void DrawOnImageAccordingToRadioButtonOption(int eventX, int eventY);
	/**
	 * @fn DrawBoxAndRotationLineOnImage()
	 * @brief Draws a box from x, y, width, height. \n and a line for the rotation with TopX, TopY, BottomX and BottomY
	 */
	void DrawBoxAndRotationLineOnImage();
	/**
	 * @fn ChangeImagePath(const wxString& path)
	 * @brief Changes the image path
	 * @param objectName The path were the image is located
	 */
	void ChangeImagePath(const wxString& path);
	/**
	 * @fn GetPathFromChosenDir(boost::filesystem::path &path)
	 * @brief this function takes the image path and subtracts the path of dirPath \n
	 * and returns this in path
	 */
	void GetPathFromChosenDir(boost::filesystem::path &path);
	/**
	 * @fn GetRotation()
	 * @brief Gets the rotation from TopX, TopY, BottomX and BottomY in degrees
	 */
	double GetRotation();

protected:
	wxBoxSizer* bSizer121;
	wxStaticText* BackgroundTextField;
	wxFilePickerCtrl* filePicker;
	wxStaticBitmap* ImageField;
	wxDirPickerCtrl* dirPicker;
	wxStaticText* AmountOfObjectsLabel;
	wxTextCtrl* AmountOfObjectsTxtField;
	wxComboBox* ObjectComboBox;
	wxComboBox* BackgroundComboBox;
	wxComboBox* LightingComboBox;
	wxComboBox* PerspectiveComboBox;
	wxRadioButton* SurroundBox_radioBtn;
	wxButton* CrateButton;
	wxRadioButton* LUC_RadioBtn;
	wxStaticText* LUC_Label;
	wxRadioButton* RLC_RadioBtn;
	wxStaticText* RLC_Label;
	wxCheckBox* NoRotationCheckBox;
	wxRadioButton* CenterLine_radioBtn;
	wxRadioButton* CenterTop_RadioBtn;
	wxStaticText* CenterTop_Label;
	wxStaticText* Rotation_Label;
	wxStaticText* CenterPointValue_label;
	wxStaticText* CenterPoint_label;
	wxStaticText* RotationValue_label;
	wxRadioButton* CenterBottom_RadioBtn;
	wxStaticText* CenterBottom_Label;
	wxButton* NextObjectButton;
	wxButton* DoneButton;
	wxButton* SkipButton;
	wxButton* ResetButton;
	wxStaticText* MessageLabel;
	wxSlider* ColorSlider;
	wxRadioButton* ZoomBox_radioBtn;
	wxRadioButton* LUZC_RadioBtn;
	wxRadioButton* RLZC_RadioBtn;
	wxButton* ZoomButton;
	wxButton* OriginalImageButton;
	wxCheckBox* ZoomCheckBox;

	/**
	 * @brief The function which is called when the next button is pressed
	 * @details when this button is pressed the inputed values are written to an xml
	 * also is either an new image loaded are values cleared.
	 * @param event the event created when the next button is pressed
	 */
	virtual void OnNextObjectButton(wxCommandEvent& event);
	/**
	 * @brief The function which is called when the focus on AmountOfObjectsTxtField is lost
	 * @details when there are more than one objects within the image the next buttons
	 * label changes into next image or into next object
	 * @param event the event created the focus is lost
	 */
	virtual void OnAmountOfObjects(wxFocusEvent& event);
	/**
	 * @brief The function which is called when the left mouse left mouse button is released within the image field
	 * @details only when the mouse is released the values inputed by the user are updated
	 * @param event the event created when the left mouse button is released within the image field
	 */
	virtual void OnLeftMouseRelease(wxMouseEvent& event);
	/**
	 * @brief The function which is called when the left mouse button is pressed within the image field
	 * @details while the left mouse button is pressed the a rectangle or line is drawn real time onto the image
	 * @param event the event created when the left mouse button is pressed within the image field
	 */
	virtual void OnLeftMousePressed(wxMouseEvent& event);
	/**
	 * @fn OnImageMotion(wxMouseEvent& event)
	 * @brief The function which is called when the left mouse button is pressed and moved within the image field
	 * @details draws the first rectangle or line
	 * @param event the event created when the left mouse button is pressed and moved within the image field
	 */
	virtual void OnImageMotion(wxMouseEvent& event);
	/**
	 * @brief The function which is called when the left mouse button is pressed on the box
	 * @details makes the center line option (in)visible
	 * @param event the event created when the left mouse button is pressed on the box
	 */
	virtual void NoRotation(wxMouseEvent& event);
	/**
	 * @brief The function which is called when the skip button is pressed
	 * @details calls NextImage()
	 * @param event the event created when the skip button is pressed
	 */
	virtual void OnSkip(wxMouseEvent& event);
	/**
	 * @brief The function which is called when the mouse leaves the image field
	 * @param event the event created when the image field is left
	 */
	virtual void OnLeftImageField(wxMouseEvent& event) {
		mousePressedInImage = false;
	}
	/**
	 * @brief This function is called when a value is selected in a combo box
	 * @param event the event created when the image field is left
	 */
	virtual void OnComboSelect(wxCommandEvent& event);
	/**
	 * @brief This function is called when the crate button is pressed
	 * @param event the event created when the image field is left
	 */
	virtual void OnCrateButton(wxCommandEvent& event);
	/**
	 * @brief The function which is called when the window size changes
	 * @details values concerning the location of the current object are set to 0
	 * @param event the event created when the window size changes
	 */
	virtual void OnSizeChange(wxSizeEvent& event);
	/**
	 * @fn OnDoneButton(wxCommandEvent& event)
	 * @brief The function which is called when the Done button is pressed
	 * @param event the event created when the Done button is pressed
	 */
	virtual void OnDoneButton(wxCommandEvent& event);
	/**
	 * @brief The function which is called when the Reset button is pressed
	 * @details resets all the input values for the whole image
	 * @param event the event created when the Done button is pressed
	 */
	virtual void OnReset(wxCommandEvent& event);
	/**
	 * @brief The function which is called when the slider for the drawing color changes
	 * @details the color for each object changes
	 * @param event the event created when the slider moves
	 */
	virtual void OnColorSlider( wxScrollEvent& event );
	/**
	 * @brief The function which is called when the zoom button is pressed
	 * @details zooms in on the part which is selected and places it in the image field
	 * @param event the event created when the zoom button is pressed
	 */
	virtual void OnZoom( wxCommandEvent& event );
	/**
	 * @brief The function which is called when the original button is pressed
	 * @details places the original image in the image field
	 * @param event the event created when the original button is pressed
	 */
	virtual void OnOriginal( wxCommandEvent& event );
	/**
	 * @brief The function which is called when either the zoom On check box is changed ore the radio button is pressed
	 * @details inverts the values of these two objects
	 * @param event the event created when either the zoom On check box is changed ore the radio button is pressed
	 */
	virtual void OnZoomChange( wxMouseEvent& event );

public:
	///@brief Constructor for the GUIFrame
	GUIFrame(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title =
			wxT("GUI"), const wxPoint& pos = wxDefaultPosition,
			const wxSize& size = wxSize(600, 590),
			long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
	///@brief Destructor for the GUIFrame
	~GUIFrame();

	/**
	 * @fn Start()
	 * @brief call this function to start going through all the images in the image directory
	 * @details sets all the variables, and makes same GUI object invisible
	 */
	void Start();
	/**
	 * @fn AddBackground(wxString objectName)
	 * @brief Adds a background type to the drop down box in the GUI
	 * @param backgroundName The name of the object you want to add to the combo box list
	 */
	void AddBackground(const wxString &backgroundName);
	/**
	 * @fn AddLight(wxString objectName)
	 * @brief Adds a light type to the drop down box in the GUI
	 * @param lightName The name of the object you want to add to the combo box list
	 */
	void AddLight(const wxString &lightName);
	/**
	 * @fn AddPerspective(wxString objectName)
	 * @brief Adds a perspective type to the drop down box in the GUI
	 * @param perspectiveName The name of the object you want to add to the combo box list
	 */
	void AddPerspective(const wxString &perspectiveName);
	/**
	 * @fn AddObject(wxString objectName)
	 * @brief Adds an object type to the drop down box in the GUI
	 * @param objectName The name of the object you want to add to the combo box list
	 */
	void AddObject(const wxString &objectName);
	/**
	 * @fn ChangeImagePaths(vstd::vector<boost::filesystem::path> &imagePaths)
	 * @brief changes the vector with image paths
	 * @param imagePaths A vector variable for each image path in dirPath/Images
	 */
	void ChangeImagePaths(
			const std::vector<boost::filesystem::path> &imagePaths);
	/**
	 * @fn EditXML(int edit)
	 * @brief sets the variable editXML
	 * @param edit the value at which editXML is set
	 */
	void EditXML(int edit);
	/**
	 * @fn SetDirPath(boost::filesystem::path path)
	 * @brief sets the directory path
	 * @param path the value at which dirPath is set
	 */
	void SetDirPath(const boost::filesystem::path path);
	/**
	 * @fn NextImage()
	 * @brief Selects the next image in the image directory and calls the ChangeImage function
	 */
	void NextImage();
	/**
	 * @fn SetXMLPath(const boost::filesystem::path &XMLPath)
	 * @brief Selects the next image in the image directory and calls the ChangeImage function
	 * @param XMLPath the location of the xml.
	 */
	void SetXMLPath(const boost::filesystem::path &XMLPath);
};

#endif //__ConverterGUI__
