//******************************************************************************
//
//                 Low Cost Vision
//
//******************************************************************************
// Project:        MetaDataApp
// File:           ChoiceFrame.h
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
#ifndef __CHOICEFRAME__
#define __CHOICEFRAME__

#include "ConverterGUI.h"

#include <wx/string.h>
#include <wx/filepicker.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/filedlg.h>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

/**
 * @class ChoiceFrame ChoiceFrame.h "src/ChoiceFrame"
 * @brief this class makes an frame at which 3 choices can be made and  a directory needs to be chosen
 * \n the choices are make a new xml file,
 * \n edit an old xml or add only new images
 * \n the directory needs to contain an directory 'Image', and a document 'Values.txt'
 */
class ChoiceFrame: public wxFrame {

private:
	bool Load;
	GUIFrame *frame;
	std::vector<boost::filesystem::path> imagePaths;

protected:
	///Object to select a directory
	wxDirPickerCtrl* dirPicker;
	///Object to select the xml file to output to
	wxFilePickerCtrl* XMLPicker;
	///Object to set message in
	wxStaticText* MessageField;
	///Exit button
	wxButton* ExitButton;
	///Ok button
	wxButton* OKButton;
	///Object to select the create new xml option
	wxRadioButton* CreateNewXMLradioBtn;
	///Object to select the edit a xml option
	wxRadioButton* EditExistingXMLradioBtn;
	///Object to select the add to existing xml option
	wxRadioButton* AddToExistingXMLradioBtn;

	/**
	 * @fn OnOK( wxMouseEvent& event )
	 * @brief The function which is called when the OK button is pressed
	 * @details when this button is pressed the inputed the comboboxen from GUIFrame are filled
	 * and all the image paths are collected
	 * @param event the event created when the OK button is pressed
	 */
	virtual void OnOK(wxMouseEvent& event);
	/**
	 * @fn OnXMLOption( wxMouseEvent& event )
	 * @brief The function which is called when one of the radio buttons is pressed
	 * @param event the event created when the OK button is pressed
	 */
	virtual void OnXMLOption(wxMouseEvent& event);
	/**
	 * @fn OnExit( wxMouseEvent& event )
	 * @brief The function which is called when the Exit button is pressed
	 * @details this frame and the GUIFrame are closed
	 * @param event the event created when the Exit button is pressed
	 */
	virtual void OnExit(wxMouseEvent& event);
	/**
	 * @fn LoadImagePaths(boost::filesystem::path itPath)
	 * @brief Loads all the image paths from the image directory
	 * @param itPath the root map from where the images are supposed to be loaded
	 * @param xmlPath the path where the xml is located
	 */
	void LoadImagePaths(const boost::filesystem::path &itPath,
			const boost::filesystem::path &xmlPath);

public:
	///@brief Constructor for ChoiceFrame
	ChoiceFrame(wxWindow* parent, wxWindowID id = wxID_ANY,
			const wxString& title = wxEmptyString, const wxPoint& pos =
					wxDefaultPosition, const wxSize& size = wxSize(-1, -1),
			long style = wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL);
	~ChoiceFrame();

	/**
	 * @fn Start()
	 * @brief this function creates an GUIFrame and shows the ChoiceFrame
	 */
	void Start();
};

#endif //__CHOICEFRAME__
