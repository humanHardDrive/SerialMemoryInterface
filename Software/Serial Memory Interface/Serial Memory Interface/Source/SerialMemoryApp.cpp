#include "SerialMemoryApp.h"

#include "SerialMemoryFrame.h"

DECLARE_APP(SerialMemoryApp)
IMPLEMENT_APP(SerialMemoryApp)

bool SerialMemoryApp::OnInit()
{
	SerialMemoryFrame* frame = new SerialMemoryFrame(wxT("Title"));
	frame->Show();

	return true;
}