///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 21 2016)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __WXFBWINDOWS_H__
#define __WXFBWINDOWS_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/radiobut.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class FBMainWindow
///////////////////////////////////////////////////////////////////////////////
class FBMainWindow : public wxFrame 
{
	private:
	
	protected:
		wxStaticText* txtTitle;
		wxStaticText* labAlgo;
		wxRadioButton* radioFP;
		wxRadioButton* radioBM;
		wxStaticText* labImg;
		wxTextCtrl* txtImg;
		wxButton* btnExecute;
		wxButton* m_button1;
		
		// Virtual event handlers, overide them in your derived class
		virtual void execute( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnButtonClose( wxCommandEvent& event ) { event.Skip(); }
		
	
	public:
		
		FBMainWindow( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("RECONSTRUCCIÓN 3D"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 378,207 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
		
		~FBMainWindow();
	
};

#endif //__WXFBWINDOWS_H__
