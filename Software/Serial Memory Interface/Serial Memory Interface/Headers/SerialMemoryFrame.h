#pragma once

#include <wx/wx.h>

class SerialMemoryFrame : public wxFrame
{
public:
	SerialMemoryFrame(const wxString& title);

	void OnQuit(wxCommandEvent& event);	

private:
	DECLARE_EVENT_TABLE()
};