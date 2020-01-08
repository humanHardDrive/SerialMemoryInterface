///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "MemoryViewBase.h"

///////////////////////////////////////////////////////////////////////////

MemoryView::MemoryView( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );

	m_GridMemory = new wxGrid( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );

	// Grid
	m_GridMemory->CreateGrid( 5, 16 );
	m_GridMemory->EnableEditing( true );
	m_GridMemory->EnableGridLines( true );
	m_GridMemory->EnableDragGridSize( false );
	m_GridMemory->SetMargins( 0, 0 );

	// Columns
	m_GridMemory->AutoSizeColumns();
	m_GridMemory->EnableDragColMove( false );
	m_GridMemory->EnableDragColSize( false );
	m_GridMemory->SetColLabelSize( 30 );
	m_GridMemory->SetColLabelAlignment( wxALIGN_CENTER, wxALIGN_CENTER );

	// Rows
	m_GridMemory->EnableDragRowSize( false );
	m_GridMemory->SetRowLabelSize( 80 );
	m_GridMemory->SetRowLabelAlignment( wxALIGN_CENTER, wxALIGN_CENTER );

	// Label Appearance

	// Cell Defaults
	m_GridMemory->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	bSizer1->Add( m_GridMemory, 0, wxALL, 5 );

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer( wxVERTICAL );

	m_BtnLoadFile = new wxButton( this, wxID_ANY, wxT("Load File"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer2->Add( m_BtnLoadFile, 0, wxALL, 5 );


	bSizer2->Add( 0, 0, 0, wxEXPAND, 5 );

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxHORIZONTAL );

	m_TextWordSize = new wxStaticText( this, wxID_ANY, wxT("Word Size"), wxDefaultPosition, wxDefaultSize, 0 );
	m_TextWordSize->Wrap( -1 );
	bSizer3->Add( m_TextWordSize, 0, wxALL, 5 );

	m_TextBoxWordSize = new wxTextCtrl( this, wxID_ANY, wxT("1"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_TextBoxWordSize, 0, wxALL, 5 );


	bSizer2->Add( bSizer3, 1, wxEXPAND, 5 );


	bSizer1->Add( bSizer2, 1, wxEXPAND, 5 );


	this->SetSizer( bSizer1 );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	m_BtnLoadFile->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MemoryView::OnBtnLoadFile ), NULL, this );
	m_TextBoxWordSize->Connect( wxEVT_KILL_FOCUS, wxFocusEventHandler( MemoryView::OnWordSizeLoseFocus ), NULL, this );
}

MemoryView::~MemoryView()
{
	// Disconnect Events
	m_BtnLoadFile->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MemoryView::OnBtnLoadFile ), NULL, this );
	m_TextBoxWordSize->Disconnect( wxEVT_KILL_FOCUS, wxFocusEventHandler( MemoryView::OnWordSizeLoseFocus ), NULL, this );

}
