#define WIN32_LEAN_AND_MEAN

#include "SerialMemoryApp.h"

#include "SerialMemoryFrame.h"

IMPLEMENT_APP(SerialMemoryApp)

bool SerialMemoryApp::OnInit()
{
	SerialMemoryFrame* frame = new SerialMemoryFrame(wxT("Title"));
	frame->Show();

	return true;
}