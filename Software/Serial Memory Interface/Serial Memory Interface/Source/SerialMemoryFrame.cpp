#include "SerialMemoryFrame.h"
#include "SerialMemoryApp.h"

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
	m_MemoryGrid = new wxGrid(this, wxID_ANY, wxPoint(0, 0), wxSize(400, 300));
	m_MemoryGrid->CreateGrid(10, 10);
	m_MemoryGrid->HideColLabels();
	m_MemoryGrid->HideRowLabels();
}
