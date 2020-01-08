///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/font.h>
#include <wx/grid.h>
#include <wx/gdicmn.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/button.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class MemoryView
///////////////////////////////////////////////////////////////////////////////
class MemoryView : public wxFrame
{
	private:

	protected:
		wxGrid* m_GridMemory;
		wxButton* m_BtnLoadFile;
		wxStaticText* m_TextWordSize;
		wxTextCtrl* m_TextBoxWordSize;

		// Virtual event handlers, overide them in your derived class
		virtual void OnBtnLoadFile( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnWordSizeLoseFocus( wxFocusEvent& event ) { event.Skip(); }


	public:

		MemoryView( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("MemoryViewBase"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 830,470 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );

		~MemoryView();

};

