#pragma once

#include <wx/wx.h>
#include <wx/grid.h>

#include <string>

class SerialMemoryFrame : public wxFrame
{
public:
	SerialMemoryFrame(const wxString& title);

	void OnQuit(wxCommandEvent& event);	

private:
	DECLARE_EVENT_TABLE()

	void LoadFile(const std::string& sPath);

	wxGrid* m_MemoryGrid;
};