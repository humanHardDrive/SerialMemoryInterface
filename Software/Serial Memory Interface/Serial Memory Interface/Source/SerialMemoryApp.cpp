#include "SerialMemoryApp.h"

#include "SerialMemoryFrame.h"

IMPLEMENT_APP(SerialMemoryApp)

bool SerialMemoryApp::OnInit()
{
	SerialMemoryFrame* frame = new SerialMemoryFrame(wxT("Title"));
	frame->Show();

	return true;
}