///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 21 2016)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "wxfbWindows.h"

///////////////////////////////////////////////////////////////////////////

FBMainWindow::FBMainWindow( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, false, wxEmptyString ) );
	this->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	txtTitle = new wxStaticText( this, wxID_ANY, wxT("Reconstrucción 3D utilizando OpenCV y OpenGL"), wxDefaultPosition, wxSize( -1,20 ), 0 );
	txtTitle->Wrap( -1 );
	txtTitle->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );
	
	bSizer1->Add( txtTitle, 0, wxALIGN_CENTER|wxALL, 5 );
	
	wxGridSizer* gSizer3;
	gSizer3 = new wxGridSizer( 0, 2, 0, 0 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	labAlgo = new wxStaticText( this, wxID_ANY, wxT("Algoritmos"), wxDefaultPosition, wxDefaultSize, 0 );
	labAlgo->Wrap( -1 );
	labAlgo->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );
	
	bSizer4->Add( labAlgo, 0, wxALL, 5 );
	
	radioFP = new wxRadioButton( this, wxID_ANY, wxT("Feature points"), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
	radioFP->SetValue( true ); 
	bSizer4->Add( radioFP, 0, wxALL, 5 );
	
	radioBM = new wxRadioButton( this, wxID_ANY, wxT("Block matching"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( radioBM, 0, wxALL, 5 );
	
	
	gSizer3->Add( bSizer4, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxVERTICAL );
	
	labImg = new wxStaticText( this, wxID_ANY, wxT("Imágenes"), wxDefaultPosition, wxDefaultSize, 0 );
	labImg->Wrap( -1 );
	labImg->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );
	
	bSizer5->Add( labImg, 0, wxALL, 5 );
	
	txtImg = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( txtImg, 0, wxALIGN_TOP|wxALL, 5 );
	
	
	gSizer3->Add( bSizer5, 1, wxEXPAND, 5 );
	
	
	bSizer1->Add( gSizer3, 1, wxEXPAND, 5 );
	
	
	bSizer1->Add( 0, 0, 1, wxEXPAND, 5 );
	
	wxGridSizer* gSizer2;
	gSizer2 = new wxGridSizer( 0, 2, 0, 0 );
	
	btnExecute = new wxButton( this, wxID_ANY, wxT("Ejecutar"), wxDefaultPosition, wxDefaultSize, 0 );
	btnExecute->SetDefault(); 
	gSizer2->Add( btnExecute, 0, wxALIGN_CENTER|wxALIGN_TOP|wxALL, 5 );
	
	m_button1 = new wxButton( this, wxID_ANY, wxT("Cerrar"), wxDefaultPosition, wxDefaultSize, 0 );
	gSizer2->Add( m_button1, 0, wxALIGN_CENTER|wxALIGN_RIGHT|wxALL, 5 );
	
	
	bSizer1->Add( gSizer2, 1, wxEXPAND, 5 );
	
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	// Connect Events
	btnExecute->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FBMainWindow::execute ), NULL, this );
	m_button1->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FBMainWindow::OnButtonClose ), NULL, this );
}

FBMainWindow::~FBMainWindow()
{
	// Disconnect Events
	btnExecute->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FBMainWindow::execute ), NULL, this );
	m_button1->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( FBMainWindow::OnButtonClose ), NULL, this );
	
}
