#define WIN32_LEAN_AND_MEAN

#include "SerialMemoryFrame.h"
#include "SerialMemoryApp.h"

SerialMemoryFrame::SerialMemoryFrame(const wxString & title) :
	MemoryView(NULL, wxID_ANY, title)
{
}

void SerialMemoryFrame::OnBtnLoadFile(wxCommandEvent & event)
{
}

void SerialMemoryFrame::OnWordSizeLoseFocus(wxFocusEvent & event)
{
}
