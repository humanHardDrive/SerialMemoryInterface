#include "SerialMemoryFrame.h"

BEGIN_EVENT_TABLE(SerialMemoryFrame, wxFrame)
	EVT_MENU(wxID_EXIT, SerialMemoryFrame::OnQuit)
END_EVENT_TABLE()

void SerialMemoryFrame::OnQuit(wxCommandEvent& event)
{
	Close();
}

SerialMemoryFrame::SerialMemoryFrame(const wxString & title) :
	wxFrame(NULL, wxID_ANY, title)
{
}
